#include "Render2.h"
#include <algorithm>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <sstream>


using namespace std;
using namespace std::placeholders;

static IH264Render*  g_render;
//static int RENDER_FRAME_BUFFER = 10;

IH264Render::IH264Render(MouseFuncType _mouse,struct  ILogHandle* log )
{
    this->log.Setlog(log);
    MouseDealer  = _mouse;
    window = nullptr;
    render = nullptr;
    texture = nullptr;
    display_width = 0;
    display_height = 0;
    display_format = 0;
    output_codec_context = nullptr;
    rgb_frame = nullptr;
    buffer  = nullptr;

    bEnableCodec  =  false;
    bEnableSdl  = false;

    attention.notify = bind(&IH264Render::OnNotify,this, _1);

    SDL_Init( SDL_INIT_EVERYTHING );
    Init();
    g_render =  this;
}

void IH264Render::Flush(uint8_t* yuv,int length)
{

	if (bEnableCodec)
	{

		attention.SetShow(1);
		if (bEnableSdl)
		{
			int len = 0; 
			int got_frame = 0;
			AVFrame*frame = avcodec_alloc_frame();
			AVPacket  packet;
			av_init_packet(&packet);

			packet.data = yuv;
			packet.size  = length;

			len = avcodec_decode_video2(output_codec_context, frame, &got_frame, &packet);
			if (len >= 0 && got_frame)
			{

				struct SwsContext *img_convert_ctx = sws_getContext(frame->width, frame->height,
					attention.formt,
					frameRatio.width,
					frameRatio.height,
					attention.formt,
					SWS_FAST_BILINEAR, NULL, NULL, NULL);


				sws_scale(img_convert_ctx, (const uint8_t * const *)frame->data, frame->linesize,
						0, attention.height, rgb_frame->data, rgb_frame->linesize);

				if (render)
				{

					if(texture == nullptr)
					{
						texture = SDL_CreateTexture(render,SDL_PIXELFORMAT_YV12,  SDL_TEXTUREACCESS_TARGET, frameRatio.width,frameRatio.height);
					}

					if (texture)
					{
		      			  SDL_UpdateTexture(texture, NULL,buffer, frameRatio.width);

		      			  SDL_Rect srcrect ={0,0,frameRatio.width,frameRatio.height};
		      			  SDL_Rect dstrect = {frameRatio.xstart,frameRatio.ystart,frameRatio.width,frameRatio.height};
					      SDL_RenderCopy(render, texture,&srcrect, &dstrect);
					      SDL_RenderPresent(render);
					}
				}
				sws_freeContext(img_convert_ctx);
			}
			av_free(frame);
		}
	}	
}

void IH264Render::SetAttention(int width,int height,std::wstring fmt)
{
	PixelFormat  format;
	if (fmt == L"yuv420p")    format  =  PIX_FMT_YUV420P;
	else if (fmt == L"rgb32")  format  =  PIX_FMT_BGRA;
	else if (fmt == L"rgb24")  format  = PIX_FMT_BGR24;
	attention.SetSrcSize(width,height,format);
}

void IH264Render::OnNotify(Attention::NotifyType type)
{
    if (type == Attention::SRCSIZE)
	{
		bEnableCodec = LoadFilter();
		std::ostringstream ss;
		ss << "onNotify():" << "Attention::SRCSIZE";
		log.handle(ss.str());
	}
    else if(type ==  Attention::FULLSCREEN)
	{
    	std::ostringstream ss;
    	ss << "onNotify():" << "Attention::FULLSCREEN";
    	log.handle(ss.str());
	}
    else if (type ==  Attention::SHOW)
	{
    	std::ostringstream ss;
    	ss << "onNotify():" << "Attention::SHOW";
    	log.handle(ss.str());
    	if(attention.iShow)Show();
		attention.iShow ? Show() : Hide();
	}
}

void  IH264Render::Show()
{
	SDL_Init(SDL_INIT_VIDEO);

	if(window != nullptr)
		SDL_DestroyWindow(window);

	window = SDL_CreateWindow( "YUV", 0, 0,display_width,display_height,   SDL_WINDOW_OPENGL);
	if(!window)  
	{  
        log.handle(std::string("RenderMoudle: SDL_CreateWindow failed:") + SDL_GetError());
		return; 
	}

	std::ostringstream ss;
	ss << "w " << attention.width << " h" << attention.height;
    log.handle("Create SDL Window Successed " + ss.str());

    if(render != nullptr)
    	SDL_DestroyRenderer(render);

	render  = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!render)
	{
        log.handle(std::string("RenderMoudle: SDL_CreateRenderer failed:") + SDL_GetError());
		SDL_DestroyWindow(window);
		window  = 0;
		return; 
	}

    log.handle("Create SDL Render Successed");
	SDL_SetRelativeMouseMode(SDL_TRUE);

	//bPause = false;

    log.handle("Load SDL Module Successed");
	bEnableSdl = true;

	//alarm(3);
	MouseKeybroadLimit(true);
}



void IH264Render::MouseKeybroadLimit(bool  bEnable)
{
}

FrameRatio * IH264Render::CaculateFrameRatio()
{
	int picw = attention.width;
	int pich = attention.height;

	int dspw = display_width;
	int dsph = display_height;

	float ratioDsp = (float)dspw/(float)dsph;
    float ratioPic = (float)picw/(float)pich;

    if(ratioDsp == ratioPic)
    {
    	frameRatio.width = dspw;
		frameRatio.height = dsph;
		frameRatio.xstart = 0;
		frameRatio.ystart = 0;

		return &frameRatio;
    }

    if(ratioPic > ratioDsp) //
    {
    	pich = display_width / ratioPic;
    	pich = (pich + 4) &~3;

    	picw = display_width;

    }else
    {
    	pich = display_height;
    	picw = display_height * ratioPic;
    	picw = (picw + 4) &~3;
    }

    frameRatio.width = picw;
    frameRatio.height = pich;
    frameRatio.xstart = (display_width - picw) / 2;
    frameRatio.ystart = (display_height - pich) / 2;

    return &frameRatio;
}

bool IH264Render::LoadFilter()
{
	Release();

	CaculateFrameRatio();

	av_register_all();
	rgb_frame = avcodec_alloc_frame();

	//directly translate to screen display size.
	FrameRatio * pframeRatio = CaculateFrameRatio();

	std::ostringstream ss;
	ss << "loadFilter: ratio width " << pframeRatio->width << "  height "<< pframeRatio->height;
	log.handle(ss.str());

	int buffer_size = avpicture_get_size(PIX_FMT_YUV420P,pframeRatio->width,pframeRatio->height);
	buffer = (uint8_t *)malloc(sizeof(uint8_t) * buffer_size);

	if(texture != nullptr)
		SDL_DestroyTexture(texture);
	if(render != nullptr)
	{
		 SDL_RenderClear(render);
		texture = SDL_CreateTexture(render,SDL_PIXELFORMAT_YV12,  SDL_TEXTUREACCESS_TARGET, frameRatio.width,frameRatio.height);
	}

	avpicture_fill((AVPicture*)rgb_frame,(uint8_t *)buffer,PIX_FMT_YUV420P,pframeRatio->width,pframeRatio->height);
	AVCodec *output_codec          = NULL;
	if (!(output_codec = avcodec_find_decoder(CODEC_ID_H264)))
	{
        log.handle("RenderModule:avcodec_find_decoder(CODEC_ID_H264)");
		return false;
	}

	output_codec_context = avcodec_alloc_context3(output_codec);
	output_codec_context->codec_type = AVMEDIA_TYPE_VIDEO;
	output_codec_context->width =  attention.width;
	output_codec_context->height = attention.height;
	output_codec_context->pix_fmt = PIX_FMT_YUV420P;

	if (avcodec_open2(output_codec_context, output_codec, NULL) < 0) 
	{
        log.handle("RenderModule:avcodec_open2 failed...");
		return  false;
	}
    log.handle("LoadFilter Successed");
	return  true;
}

void IH264Render::Hide()
{
	if (render)
	{
		SDL_DestroyRenderer(render);
		render  =  nullptr;
	}
    log.handle("Destroy  SDL Render  Successed");

	if (window)
	{
		SDL_DestroyWindow(window);
		window = nullptr;
	}

    log.handle("Destroy  SDL Window  Successed");

	SDL_SetRelativeMouseMode(SDL_FALSE);

	bEnableSdl  = false;

	MouseKeybroadLimit(false);
}

void IH264Render::Release()
{
	if(output_codec_context)
	{
		avcodec_close(output_codec_context);output_codec_context = 0;
	}

	if (rgb_frame)
	{
		av_free(rgb_frame);
		rgb_frame = 0;
		/* code */
	}
	SafeDeleteWrapper::SafeDelete(buffer);
	bEnableCodec  = false;
}

void IH264Render::Init()
{
	SDL_DisplayMode mode;
	SDL_GetDisplayMode(0,0,&mode);

	display_width  = mode.w;
	display_height = mode.h;
	display_format = mode.format;

	memset(&frameRatio,0, sizeof(frameRatio));
	frameRatio.width = display_width;
	frameRatio.height = display_height;

}

IH264Render::~IH264Render()
{
	SDL_Quit();
	Release();
	Hide();
    log.Setlog(0);
}

