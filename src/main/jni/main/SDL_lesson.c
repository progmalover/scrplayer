// tutorial02.c
// A pedagogical video player that will stream through every video frame as fast as it can.
//
// This tutorial was written by Stephen Dranger (dranger@gmail.com).
//
// Code based on FFplay, Copyright (c) 2003 Fabrice Bellard, 
// and a tutorial by Martin Bohme (boehme@inb.uni-luebeckREMOVETHIS.de)
// Tested on Gentoo, CVS version 5/01/07 compiled with GCC 4.1.1
//
// Use the Makefile to build all examples.
//
// Run using
// tutorial02 myvideofile.mpg
//
// to play the video stream on your screen.


#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"

#include "SDL.h"
#include "SDL_main.h"
#include <stdio.h>
#include "SDL_logger.h"

int main(int argc, char *argv[])
{
  AVFormatContext *pFormatCtx = NULL;
  int             i, videoStream;
  AVCodecContext  *pCodecCtx = NULL;
  AVCodec         *pCodec = NULL;
  AVFrame         *pFrame = NULL; 
  AVFrame         *pFrameYUV420p = NULL; 
  AVPacket         packet;
  int             frameFinished;
  //float           aspect_ratio;

  AVDictionary    *optionsDict = NULL;
  struct SwsContext *sws_ctx = NULL;

  //SDL_Overlay     *bmp = NULL;
  //SDL_Surface     *screen = NULL;
  SDL_Rect        rect;
  SDL_Event       event;
  
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  SDL_Texture * pTexture = NULL;

    int video_width,video_height;

  // Register all formats and codecs
  av_register_all();
  
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
    fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
    exit(1);
  }

  // Open video file
  if(avformat_open_input(&pFormatCtx, argv[1], NULL, NULL)!=0)
    return -1; // Couldn't open file
  
  // Retrieve stream information
  if(avformat_find_stream_info(pFormatCtx, NULL)<0)
    return -1; // Couldn't find stream information
  
  // Dump information about file onto standard error
  av_dump_format(pFormatCtx, 0, argv[1], 0);
  
  // Find the first video stream
  videoStream=-1;
  for(i=0; i<pFormatCtx->nb_streams; i++)
    if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
      videoStream=i;
      break;
    }
  if(videoStream==-1)
    return -1; // Didn't find a video stream
  
  // Get a pointer to the codec context for the video stream
  pCodecCtx=pFormatCtx->streams[videoStream]->codec;

    video_width = pCodecCtx->width;

    video_height = pCodecCtx->height;
  
  // Find the decoder for the video stream
  pCodec=avcodec_find_decoder(pCodecCtx->codec_id);

  if(pCodec==NULL) {
    fprintf(stderr, "Unsupported codec!\n");
    return -1; // Codec not found
  }
  
  // Open codec
  if(avcodec_open2(pCodecCtx, pCodec, &optionsDict)<0)
    return -1; // Could not open codec
  
  // Allocate video frame
  pFrame=avcodec_alloc_frame();
  pFrameYUV420p = avcodec_alloc_frame();
  uint8_t *buffer;
  int numBytes;
  // Determine required buffer size and allocate buffer
  numBytes=avpicture_get_size(PIX_FMT_YUV420P, video_width,video_height);
  buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
  if(buffer==NULL)
  {
    LOGE("buffer is null");return -1;
  }
  avpicture_fill((AVPicture *)pFrameYUV420p, buffer, PIX_FMT_YUV420P, video_width, video_height);

  // Make a screen to put our video
  window = SDL_CreateWindow("My Game Window",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          0, 0,
                          SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
	renderer = SDL_CreateRenderer(window, -1, 0);
	pTexture = SDL_CreateTexture(renderer,
                               SDL_PIXELFORMAT_YV12,
                               SDL_TEXTUREACCESS_STREAMING,
                               video_width,video_height);
  if(pTexture==NULL)
  {
    LOGE("SDL_CreateTexture faild!");
    return -1;
  }
	//计算yuv一行数据占的字节数
    int iPitch = video_width*SDL_BYTESPERPIXEL(SDL_PIXELFORMAT_YV12);
						  
  if(!window) {
    fprintf(stderr, "SDL: could not set video mode - exiting\n");
    exit(1);
  }
  
  // Allocate a place to put our YUV image on that screen
  /*bmp = SDL_CreateYUVOverlay(pCodecCtx->width,
				 pCodecCtx->height,
				 SDL_YV12_OVERLAY,
				 screen);*/
  
  sws_ctx =
    sws_getContext
    (
        pCodecCtx->width,
        pCodecCtx->height,
        pCodecCtx->pix_fmt,
        video_width,
        video_height,
        PIX_FMT_YUV420P,
        SWS_BILINEAR,
        NULL,
        NULL,
        NULL
    );

  // Read frames and save first five frames to disk
  i=0;
  while(av_read_frame(pFormatCtx, &packet)>=0) {
    // Is this a packet from the video stream?
    if(packet.stream_index==videoStream) {
      // Decode video frame
      avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, 
			   &packet);
      
      // Did we get a video frame?
      if(frameFinished) {
	//SDL_LockYUVOverlay(bmp);

        LOGE("before AVPicture pict");
	AVPicture pict;
	pict.data[0] = pFrameYUV420p->data[0];
	pict.data[1] = pFrameYUV420p->data[2];
	pict.data[2] = pFrameYUV420p->data[1];

	pict.linesize[0] = pFrameYUV420p->linesize[0];
	pict.linesize[1] = pFrameYUV420p->linesize[2];
	pict.linesize[2] = pFrameYUV420p->linesize[1];

	// Convert the image into YUV format that SDL uses
  LOGE("before sws_scale");
    sws_scale
    (
        sws_ctx, 
        (uint8_t const * const *)pFrame->data, 
        pFrame->linesize, 
        0,
        pCodecCtx->height,
        pict.data,
        pict.linesize
    );
	
	//DL_UnlockYUVOverlay(bmp);
	LOGE("after sws_scale");
	rect.x = 0;
	rect.y = 0;
	rect.w = pCodecCtx->width;
	rect.h = pCodecCtx->height;
	//SDL_DisplayYUVOverlay(bmp, &rect);
		int i = SDL_UpdateTexture( pTexture, NULL, buffer, iPitch );
    LOGE("after SDL_UpdateTexture");
        //SDL_RenderClear( renderer );
        //LOGE("after SDL_RenderClear");
        SDL_RenderCopy( renderer, pTexture, NULL, NULL );
        LOGE("after SDL_RenderCopy");
        SDL_RenderPresent( renderer );
        LOGE("after SDL_RenderPresent");

        //Sleep(40);
      
      }
    }
    
    // Free the packet that was allocated by av_read_frame
    av_free_packet(&packet);
    SDL_PollEvent(&event);
    switch(event.type) {
    case SDL_QUIT:
      SDL_Quit();
      exit(0);
      break;
    default:
      break;
    }

  }
  
  // Free the YUV frame
  av_free(pFrame);
  
  // Close the codec
  avcodec_close(pCodecCtx);
  
  // Close the video file
  avformat_close_input(&pFormatCtx);

if ( pTexture != NULL )
    {
        SDL_DestroyTexture( pTexture );
        pTexture = NULL    ;
    }

    if ( renderer != NULL )
    {
        SDL_DestroyRenderer( renderer );
        renderer = NULL;
    }


    if ( NULL != window )
    {
        SDL_DestroyWindow( window );
        window = NULL;
    }
  
  return 0;
}
