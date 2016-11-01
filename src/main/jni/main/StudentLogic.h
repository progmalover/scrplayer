#ifndef _STUDENT_LOGIC_H_2014_10_27_LC_
#define _STUDENT_LOGIC_H_2014_10_27_LC_


#include "Render2.h"
#include "AudioRender.h"
#include "MessageDefine.h"
#include <thread>
#include "tcpmoduledefine.h"
#include "ParseWrapper.h"
#include "logSystem.h"

typedef  std::function<std::string()>  get_filter_ip_func;
typedef  std::function<void(uint8_t*,int)>  flush_func;

class   StudentLogic
{
public:
	struct   Variable
	{
		enum  NotifyType {IMAGESIZE};
        typedef  std::function<void(NotifyType)>  OnNotifyFuncType;
		Variable()
		{
			image_width = 0;
			image_height = 0;
			fullscreen  =  1;
		}
		int  image_width;
		int  image_height;
		int  fullscreen;
		OnNotifyFuncType  notify;
		void SetImageSize(int,int,int);
	};

	void     OnNotify(StudentLogic::Variable::NotifyType type);

public:
	StudentLogic(std::string mac,std::string ip);

    void	Load(get_filter_ip_func,flush_func,struct ILogHandle*);

	void	UnLoad();

	void    NotifyFromClientServerLib(ID id,notify_type type,std::string externs);

	void	GetDataFromTeacher(ID,uint8_t* buf,int length);

private:
    LogWrapper	 log;
	IH264Render*	     render;
	Variable	         variable;
	std::string		mac;
	std::string		ip;
	get_filter_ip_func   get_filter_ip;
	flush_func           flush;

	AuidoRender*	 micRender;
	AuidoRender*     bkRender;
	AuidoRender*     mp3Render;
};
#endif
