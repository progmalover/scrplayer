#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mmcApp.h"
#include "SDL_logger.h"

void  ExcuteUnit()
{
	App  app;
	app.Init();
	app.wait();
}

static bool running = false;
int  main(int argc,char**argv)
{
	if(running)
		return 0;

	running = true;

	LOGE("main start");
	ExcuteUnit();
	LOGE("main end");
    return  0;
}
