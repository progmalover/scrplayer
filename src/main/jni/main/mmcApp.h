#ifndef  _mmc_app_h_2015_06_10_lc_
#define  _mmc_app_h_2015_06_10_lc_

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <functional>
#include <system_error>
#include "StudentLogic.h"

#include "tcpclient.h"
#include  <map>
#include "Mac.h"
#include "SDL_logger.h"
#include "DataDispatch.h"
using namespace std;
using namespace std::placeholders;
/*
inline bool    is_exist(std::string local)
{
    system::error_code  ec;
    return filesystem::exists(local,ec) && !ec;
}*/



class App
{
private:
	//Attention attention;
	StudentLogic*  logic;
	tcpclient*    client;
	DataDispatch*dataDispatch;
	LogWrapper    log;

	std::string   connectip;
	std::string   filterip;
	std::string   clientip;
	std::string   clientmac;

public:
	void   Init();


	void   wait();

	std::string  get_connect_ip();
	
	std::string  get_filter_ip();

};

#endif
