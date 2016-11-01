#ifndef _AUDIO_RENDER_H_2014_05_20_LC_
#define _AUDIO_RENDER_H_2014_05_20_LC_

#include "stringexcept.h"
#include "DataCacheWrapper.h"
//#include "ParseWrapper.h"

#include <thread>
#include <functional>
#include <list>
#include <mutex>

#include "logSystem.h"



//static  const  int  SAMPLE_RATE = 44100;
typedef  short  SAMPLE;
static   const  int  SAMPLE_SILENCE  = 0;

class   AuidoRender
{
public:
	typedef  std::list<DataCache*>  DataCacheList;
	typedef  std::list<DataCache*>::iterator  DataCacheListIter;

	struct  Attention
	{
		Attention()
		{
			log  = 0;
			channel  = 0;
		}

		int  		channel;
		int         buffer_length;
		int			sample_rate;
		ILogHandle *  log;

		typedef  std::function<void(int)>  OnNotifyFuncType;
		OnNotifyFuncType  notify;
		void SetSoundDesc(int channel , int len)
		{
			if (this->channel != channel || this->buffer_length != len)
			{
				this->channel  = channel;
				this->buffer_length = len;

				notify(0);
			}
		}
	};


	AuidoRender(ILogHandle *plog);
	~AuidoRender();

	void OnNotify(int type);
	void SetAttention(int channel,int size);
	DataCache *  Query();
	void  Release();
	void  Enable(bool enable);

	void  LoadDevice();
	void  Flush(uint8_t* rpm,int length);
	void  Outlog(std::string str);

public:
	DataCacheList	    caches;
	LogWrapper          log;
	volatile 	bool	bQuit;
	std::thread	        tQuery;
	Attention           attention;
	bool                bEnable;
	int					nDevice;
	bool				bMutex;
	std::mutex 			mtx;
};

#endif
