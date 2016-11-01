#ifndef _H264_DECODER_FILTER_H
#define _H264_DECODER_FILTER_H

#include <string>
#include <iostream>
#include <SDL.h>
#include <functional>

extern "C"{
#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
# include <stdint.h>
#endif
}

extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include "MessageDefine.h"
#include "logSystem.h"



typedef std::function<void(uint8_t*,int)> FuncType;
typedef std::function<void(struct Mouse&)>  MouseFuncType;

struct FrameRatio
{
	int width,height;
	int xstart,ystart;
};

class  IH264Render
{
public:
    explicit IH264Render(MouseFuncType _mouse,struct ILogHandle* log);
	~IH264Render();

	struct  Attention
	{
        enum   NotifyType{SRCSIZE,FULLSCREEN,SHOW};
        typedef std::function<void( NotifyType )>  OnNotifyFuncType;

        PixelFormat  formt;
		int  width;
		int  height;
		int  iFullscreen;
		int  iShow;
        OnNotifyFuncType  notify;

        Attention()
		{
			width = 0;
			height = 0;
			formt  = (PixelFormat)-1;
			iFullscreen  =  -1;
			iShow  =  0;
		}

		void SetSrcSize(int width,int height,PixelFormat format)
		{
			if (this->width != width || this->height != height || this->formt != format)
			{
				this->width  = width;
				this->height = height;
				this->formt  = format;
				notify(SRCSIZE);
			}
		}

		void SetFullScreen(int ifull)
		{
			if (this->iFullscreen != ifull)
			{
				this->iFullscreen  = ifull;
				notify(FULLSCREEN);
			}
		}

		void SetShow(int iShow)
		{
			if (this->iShow != iShow)
			{
				this->iShow  = iShow;
				notify(SHOW);
			}
		}
	};

public:
	virtual void Flush(uint8_t* buf,int length);
	virtual void SetAttention(int width,int height,std::wstring fmt);
	
private:
    void	OnNotify(Attention::NotifyType type);
	void	Show();
	void    Hide();
	bool	LoadFilter();
	void	Release();
	void    Init();
	void    MouseKeybroadLimit(bool  bEnable);
	FrameRatio * CaculateFrameRatio();
private:
	unsigned int display_width;
	unsigned int display_height;
	unsigned int display_format;

	SDL_Window*		  window;
  	SDL_Renderer*     render;
  	SDL_Texture*	  texture;

  	AVFrame*          rgb_frame;
  	uint8_t* 		  buffer;
	AVCodecContext *output_codec_context;

	bool			 bEnableCodec;
	bool			 bEnableSdl;

    Attention        attention;
    FrameRatio		 frameRatio;

    LogWrapper  log;
private:
    MouseFuncType  MouseDealer;

};

#endif
