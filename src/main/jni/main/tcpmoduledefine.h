#ifndef _tcpmoduledefine_h_20150606_lc_
#define _tcpmoduledefine_h_20150606_lc_

#include <string>
#include <functional>

struct ID
{
	std::string ip;
};


enum notify_type
{
	disconnect,
	connected
};

typedef std::function<void(ID,notify_type,std::string)> onnotify;
typedef  std::function<void(ID,uint8_t*,int)> dealpacketfunctype;
typedef  std::function<void(ID)>  closecallbackfunctype;

#endif
