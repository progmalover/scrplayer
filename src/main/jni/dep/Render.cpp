#include "Render.h"
#include <algorithm>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <sstream>


using namespace std;
using namespace std::placeholders;

static IH264Render*  g_render;

/*
void  Check(int sig)
{
	g_render->NoDataToHide();
}
*/

IH264Render::IH264Render(MouseFuncType _mouse,struct  ILogHandle* log )
{
    this->log.Setlog(log);
    MouseDealer  = _mouse;
    window = 0;
    render = 0;
    texture = 0;
    display_width = 0;
    display_height = 0;
    output_codec_context = 0;
    rgb_frame = 0;
    buffer  = 0;
    iCount  = 0;
    bEnableCodec  =  false;
    bEnableSdl  = false;

    attention.notify = bind(&IH264Render::OnNotify,this, _1);

    SDL_Init( SDL_INIT_EVERYTHING );
    Init();
    signal(SIGALRM,Check);
    g_render =  this;
}

void IH264Render::Flush(uint8_t* yuv,int length)
{
	log.handle("IH264Render::Flush");

	if (bEnableCodec)
	{
		++iCount;
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
				struct SwsContext *img_convert_ctx = sws_getContext(attention.width, attention.height,  
					attention.formt,  
					display_width,
					display_height,  
					PIX_FMT_YUV420P,
					SWS_POINT, NULL, NULL, NULL);

				sws_scale(img_convert_ctx, (const uint8_t * const *)frame->data, frame->linesize,  
						0, attention.height, rgb_frame->data, rgb_frame->linesize); 
				if (render)
				{

					if(texture == 0)
						texture = SDL_CreateTexture(render,SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, display_width,display_height);
					if (texture)
					{
		      			  SDL_UpdateTexture(texture, NULL,buffer, display_width);
		      			  SDL_RenderClear(render);
					      SDL_RenderCopy(render, texture, NULL, NULL);
					      SDL_RenderPresent(render);
					      //SDL_DestroyTexture(texture);
					      //texture  = 0;
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

		//attention.iShow ? Show() : Hide();
	}
}

void  IH264Render::Show()
{
	window    = SDL_CreateWindow( "YUV", 0, 0, 1280, 720,  SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
	if(!window)  
	{  
        log.handle(std::string("RenderMoudle: SDL_CreateWindow failed:") + SDL_GetError());
		return; 
	}
    log.handle("Create SDL Window Successed");

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

	alarm(3);

	MouseKeybroadLimit(true);
}



void IH264Render::MouseKeybroadLimit(bool  bEnable)
{
}

bool IH264Render::LoadFilter()
{
	Release();
	av_register_all();
	rgb_frame = avcodec_alloc_frame();
	int buffer_size = avpicture_get_size(PIX_FMT_YUV420P,display_width,display_height);
	buffer = (uint8_t *)malloc(sizeof(uint8_t) * buffer_size);
	avpicture_fill((AVPicture*)rgb_frame,(uint8_t *)buffer,PIX_FMT_YUV420P,display_width,display_height);
	AVCodec *output_codec          = NULL;
	if (!(output_codec = avcodec_find_decoder(CODEC_ID_H264)))
	{
        log.handle("RenderModule:avcodec_find_decoder(CODEC_ID_H264)");
		return false;
	}
	output_codec_context = avcodec_alloc_context3(output_codec);
	output_codec_context->codec_type = AVMEDIA_TYPE_VIDEO;
	output_codec_context->width =  attention.width;
	output_codec_context->height =  attention.height;
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
		render  =  0;
	}
    log.handle("Destroy  SDL Render  Successed");

	if (window)
	{
		SDL_DestroyWindow(window);
		window = 0;
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
	display_width = 1280;
	display_height = 720;
}

IH264Render::~IH264Render()
{
	SDL_Quit();
	Release();
	Hide();
    log.Setlog(0);
}

void IH264Render::NoDataToHide()
{
	if (iCount == 0)
	{
        log.handle("NoDataToHide:attention.SetShow(0)");
		attention.SetShow(0);
	}
	else
	{
		iCount = 0;
		log.handle("NoDataToHide:alarm(1);");
		alarm(1);
	}
}
