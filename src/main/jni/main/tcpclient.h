#ifndef _tcp_client_h_20150606_lc_
#define _tcp_client_h_20150606_lc_


#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include "logSystem.h"
#include "tcpmoduledefine.h"
#include "WinError.h"

#include <thread>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
typedef int SOCKET;
typedef sockaddr_in SOCKADDR_IN;

#include "DataCacheWrapper.h"

typedef  std::function<std::string()>  get_connect_ip_func;

class tcpclient
{
public:
	tcpclient(get_connect_ip_func _func,short port,ILogHandle* _log);
	~tcpclient(void);
	void init(dealpacketfunctype func,closecallbackfunctype _close);
	void addnotify(onnotify _notify);
	void wait();
	void flush(uint8_t* buf,int length);
private:
	virtual void deal_packet(ID id,uint8_t* buf,int length);
	void  route(ID id,uint8_t* buf,int length); 
	void  Close();
	void  worker();
	bool  ReceiveOnPackage(int length);
	bool  _connect();
	bool  Send(uint8_t* buf,int length);
private:
	get_connect_ip_func get_connect_ip;
	onnotify  notify;
	ID        id;
	short     port;
	bool      bConnected;
	LogWrapper   log;
	closecallbackfunctype  _close;
    std::thread *       trun;
	SOCKET             m_socket;
	dealpacketfunctype  _func;
};

#endif
