#include "AudioRender.h"
#include <stdlib.h>
#include <sstream>
#include "SDL.h"
#include "SDL_audio.h"
#include "AudioCallback.h"
#include "logSystem.h"

void  AuidoRender:: OnNotify(int type)
	{
		try
		{
			if(this->nDevice != 0)
			{
				this->Release();
			}

			LoadDevice();
			//bEnable = true;
		}
		catch(string_exception& e)
		{
			//bEnable = false;
			Enable(false);
			log.handle(e.what());
		}
	}

 void AuidoRender:: SetAttention(int channel,int size)
{
	attention.SetSoundDesc(channel,size);
}

void AuidoRender:: Outlog(std::string str)
{
	this->log.handle(str);
}

AuidoRender:: AuidoRender(ILogHandle *plog)
{
	bMutex = false;

	nDevice = 0;
	attention.notify =std::bind(&AuidoRender::OnNotify,this,
							std::placeholders::_1);

	attention.buffer_length = 44100; // buffer length
	attention.sample_rate = 44100; // samples rate

	log.SetLog(plog);
}

AuidoRender:: ~AuidoRender()
{
	Release();
}

DataCache * AuidoRender:: Query()
{
	log.handle("AuidoRender:: Query() start DataCache::Query()...");
	std::lock_guard<std::mutex> lck (mtx);
	if (caches.empty())
	{
		Enable(false);
		log.handle("AuidoRender:: Query() caches not initlized!");
		return nullptr;
	}

	log.handle("AuidoRender:: Query() get from caches...");
	DataCache*  cache  =  caches.front();
	caches.pop_front();

	return cache;
	//SafeDeleteWrapper::SafeDelete(cache);
}

void  AuidoRender:: Release()
{
	log.handle("Release Start");

	Enable(false);

	while(caches.size())
	{
		DataCacheListIter   iter   = caches.begin();
		SafeDeleteWrapper::SafeDelete(*iter);
		caches.erase(iter);
	}

	SDL_CloseAudioDevice(nDevice);
	log.handle("Release End");
}

void  AuidoRender:: Enable(bool enable)
{
	if(nDevice <= 0)
		return;

	//if(bEnable)
	if(enable)
		SDL_UnlockAudioDevice(nDevice);
		//SDL_PauseAudioDevice(nDevice, 0); // start audio playing.
	else
		//SDL_PauseAudioDevice(nDevice, 1); // stop audio playing.
		SDL_LockAudioDevice(nDevice);


	bEnable = enable;
}

void  AuidoRender:: LoadDevice()
{

	if(nDevice  != 0)
		return;

	SDL_AudioSpec want, have;

	SDL_memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
	want.freq = attention.sample_rate;
	want.format = AUDIO_S16SYS;
	want.channels = attention.channel;
	want.samples =  attention.buffer_length/(attention.channel*2);//samples;//44100/4;
	want.silence = 1;
	want.userdata = this;

	want.callback = MyAudioCallback;  // you wrote this function elsewhere.
	nDevice = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

	if (nDevice < 0) {
		std::ostringstream ss;
		ss << "Open audio failed sdl err:" << 	SDL_GetError();
		log.handle(ss.str());
	}

	if (have.format != want.format)
	{
		std::ostringstream ss;
		ss<<"hava format:"<<have.format<<"want format:"<<want.format;
		log.handle("format not equ:" + ss.str());
	}

	//Enable(false);
	SDL_PauseAudioDevice(nDevice, 0); // start audio playing.

}

void AuidoRender::  Flush(uint8_t* rpm,int length)
{

	std::lock_guard<std::mutex> lck (mtx);

	std::ostringstream ss;
	ss << "audiorender.Flush():Flush audio length :" << length;
	log.handle(ss.str());


	DataCache*  cache = new DataCache(rpm,length);
	caches.push_back(cache);

	Enable(true);

}
