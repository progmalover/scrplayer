#include "jni.h"
#include "AudioCallback.h"
#include "AudioCallback.h"
#include "AudioRender.h"
#include "SDL_audio.h"
#include "SDL_logger.h"
#include "stringexcept.h"
#include "DataCacheWrapper.h"
#include "SDL.h"
#include <sstream>

 //class  DataCache;
void SDLCALL MyAudioCallback(void *userdata, uint8_t* stream, int len)
{
	 //printf("The audio callback is running!\n");

	 AuidoRender  *prender = (AuidoRender *)userdata;
	 std::ostringstream oss;
	 oss << "MyAudioCallback():The audio callback is running want len " << len;

	 prender->Outlog(oss.str());

	 //SDL_memset(stream, 0, len);  // just silence.
	 if(prender == nullptr)
	 {
		 SDL_memset(stream, 0, len);  // just silence.
		 return;
	 }

	 DataCache *pcache = prender->Query();
	 if(pcache != nullptr)
	 {
		 LOGW("MyAudioCallback() copy audio data to stream\n");
		 SDL_memcpy(stream, pcache-> get() + 4, len);
		 SafeDeleteWrapper::SafeDelete(pcache);
	 }else
	 {
		 LOGW("MyAudioCallback() get cache data null\n");
		 SDL_memset(stream, 0, len);  // just silence.
	 }

}
