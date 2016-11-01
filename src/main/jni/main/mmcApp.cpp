#include "mmcApp.h"
extern "C"{
extern char ** getIpConfig();
};

void   App:: Init()
{
	LOGE("Start");
/*	std::string spath(getenv("HOME"));
	if(spath[spath.size()-1] !='/') spath +="/";
	spath +="mmc.log";
	log.Setlog(new  CTextLog(spath));*/
	log.Setlog(new CConsoleHandle);

	char **ipconfigs =  getIpConfig();
	connectip= ipconfigs[0];
	filterip= ipconfigs[1];
	clientip = ipconfigs[2];
	clientmac = ipconfigs[3];


	log.handle("2");
	logic  =  new  StudentLogic(clientmac,clientip);
    dataDispatch = DataDispatch::getInst();
    dataDispatch->bindDspFunc(bind(&StudentLogic::GetDataFromTeacher,logic,_1,_2,_3));
	client = new tcpclient(bind(&App::get_connect_ip,this),9002,&log);

	logic->Load(bind(&App::get_filter_ip,this),bind(&tcpclient::flush,client,_1,_2),&log);
	log.handle("3");
	client->addnotify(bind(&StudentLogic::NotifyFromClientServerLib,logic,_1,_2,_3));
	log.handle("4");
	client->init(bind(&DataDispatch::Dispatch,dataDispatch,_1,_2,_3),0);
	log.handle("5");
}

void  App::  wait()
{
	log.handle("6");
	client->wait();
	logic->UnLoad();
	log.handle("7");
	SafeDeleteWrapper::SafeDelete(client);
	SafeDeleteWrapper::SafeDelete(logic);
	SafeDeleteWrapper::SafeDelete(&log);
}

std::string App:: get_connect_ip()
{
	if(connectip.empty())
	{
		char **ipconfigs =  getIpConfig();
		connectip= ipconfigs[0];
	}
	//log.handle("connect ip :"+connectip);
	return   connectip;
}

std::string  App:: get_filter_ip()
{

	if(filterip.empty())
	{
		char **ipconfigs =  getIpConfig();
		filterip= ipconfigs[1]; //student vms ip
	}

	return  filterip;
}
