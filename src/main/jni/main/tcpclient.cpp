#include "unistd.h"
#include "tcpclient.h"
#include <cerrno>
#include <sstream>
#ifdef OS_WINDOWS
#include <mstcpip.h>
#endif

#define   PerSendBufferBytes   8192

#include "SDL_logger.h"
#include "../SDL2-2.0.4/src/core/android/SDL_android.h"

using namespace std;

tcpclient::tcpclient(get_connect_ip_func _func,short port,ILogHandle* _log)
{
	log.SetLog(_log);
	bConnected = false;
	get_connect_ip = _func;
	this->port = port;
}

tcpclient::~tcpclient(void)
{
}
bool tcpclient::_connect()
{

	if (get_connect_ip().empty())
	{
		return  false;
	}

	if (!bConnected)
	{
		log.handle("start connection...");

		try{
			id.ip = get_connect_ip();
			sockaddr_in  addr;
			memset(&addr,0,sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr= inet_addr(id.ip.c_str());
			addr.sin_port = htons(port);

			m_socket = socket(AF_INET,SOCK_STREAM,0);

			//SO_SNDTIMEO:Send data timeout,used for connection timeout.


			struct timeval timeo = {3, 0};
			socklen_t len = sizeof(timeo);
			setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, &timeo, len);
			//this connection need timeout time.


			if (m_socket == INVALID_SOCKET)
			{
				log.handle(std::string("create") + ResolveError());
			}
			else if (connect(m_socket,(sockaddr*)&addr,sizeof(addr))) // timeout in 10 secs
			{

				close(m_socket);
				m_socket  = INVALID_SOCKET;
				log.handle(std::string("connect") + ResolveError());
			}
			else
			{
				bConnected = true;
				notify(id,connected,"");
				log.handle("connected to teachering");
			}
		}catch(...)
		{
			bConnected = false;
			return false;
		}
	}
	return  bConnected;
}

bool tcpclient::ReceiveOnPackage(int length)
{
	int  dwRecieveBytes = 0;
	uint8_t*  buf = new uint8_t[length];
	bool  bSuccessed  = true;
	while(length > dwRecieveBytes)
	{ 
		int  nCurRecvBytes = (length - dwRecieveBytes) >= PerSendBufferBytes ? PerSendBufferBytes :(length - dwRecieveBytes);
		if ((nCurRecvBytes = recv(m_socket,(reinterpret_cast<char*>(buf)) + dwRecieveBytes,nCurRecvBytes,0)) >=0 )
		{
			dwRecieveBytes += nCurRecvBytes;
		}
		else if(nCurRecvBytes == SOCKET_ERROR)
		{
			bSuccessed  = false;
			break;
		}
	}
	if (length == dwRecieveBytes)
	{
		route(id,buf,length);
	}
	//SafeDeleteWrapper::SafeDelete(buf);
	return  bSuccessed;
}

//int timeoutCount = 0;
const int exit_event_id = 501;
void tcpclient::worker()
{
	log.handle("start worker... ");

	while(true)
	{
		if (_connect())
		{
			timeval   waiter={2,0}; //waiting data for 2 secs
			fd_set   revicors;
			FD_ZERO(&revicors);
			FD_SET(m_socket,&revicors);
			try{

				if (select(m_socket + 1,&revicors,0,0,&waiter) > 0 && FD_ISSET(m_socket,&revicors))
				{
					int  length = 0;
					int  bytesRecv  =  recv(m_socket,(char*)&length,sizeof(length),0);
					if (bytesRecv <= 0 || ! ReceiveOnPackage(length))
					{
						//data error , if it is peer reset,exit app .otherwise, continue to receive data....
						std::ostringstream ss;
						ss << "socket error:" << errno;
						log.handle(ss.str() + ResolveError());
						//send error to activity,and handle it...
						Android_JNI_SendMessage(exit_event_id,0);//killme...
						break;
					}
				}
				else //timeout
				{
					log.handle("timeout 10 secs,no recieve data,so killme!");
					Android_JNI_SendMessage(exit_event_id,0);//killme...
					break;
				}

			}catch(...)
			{
				log.handle("timeout 10 secs,no recieve data,so killme!");
				Android_JNI_SendMessage(exit_event_id,0);//killme...
			    break;
			}
		}
		else
		{
			//wating for 2 secs ,and retry connection...
		    //Android_JNI_SendMessage(errno,0);

		    	Android_JNI_SendMessage(exit_event_id,0);//killme...
		    	break;
		   // std::this_thread::sleep_for (std::chrono::seconds(2));
		}
	}

	sleep(2); // wait for finish.
	exit(0);
}

bool tcpclient::Send(uint8_t* buf,int length)
{
	bool   bSuccessed  =  true;
	int  dwSendTotals = 0;
	while(length >  dwSendTotals)
	{ 
		int  nCurSendBytes = (length - dwSendTotals) >= PerSendBufferBytes ? PerSendBufferBytes :(length - dwSendTotals);

		if ((nCurSendBytes = send(m_socket,(reinterpret_cast<char*>(buf)) + dwSendTotals,nCurSendBytes,0)) >=0 )
		{
			dwSendTotals += nCurSendBytes;
		}
		else if(nCurSendBytes == SOCKET_ERROR)
		{
	        log.handle(ResolveError());
			bSuccessed   =  false;
			break;
		}
	}
	return   bSuccessed;
}


void tcpclient::init(dealpacketfunctype func,closecallbackfunctype _close)
{
	_func = func;
	this->_close  = _close;
    trun =  new  thread(bind(&tcpclient::worker,this));
}

void tcpclient::deal_packet( ID id,uint8_t* buf,int length )
{
	_func(id,buf,length);
}

void tcpclient::route( ID id,uint8_t* buf,int length )
{
	deal_packet(id,buf,length);
	//assigner.flush(id,buf,length);
}

void tcpclient::wait()
{
	trun->join();
}

void tcpclient::Close()
{
	bConnected  = false;
	notify(id,disconnect,"socket disconnect!");
	if (m_socket != INVALID_SOCKET)
	{
		close(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

void tcpclient::addnotify( onnotify _notify )
{
	notify = _notify;
}

void  tcpclient::flush(uint8_t* buf,int length)
{
	log.handle(" tcpclient::flush");
	if (bConnected)
	{
		DataCache* cache  = new DataCache(buf,length);
		Send(cache->get(),cache->size());
		SafeDeleteWrapper::SafeDelete(cache);
	}
}
