#ifndef _LOG_SYSTEM_H_2014_04_02_LC_
#define _LOG_SYSTEM_H_2014_04_02_LC_


#include <unistd.h>
#include <fcntl.h>

//#include <boost/thread.hpp>
#include <mutex>
#include "SafetyDelete.h"
#include "AsciiUnicodeConvertor.h"

#include <iostream>
#include  <fstream>
#include <string>
#include "SDL_logger.h"

inline std::string get_time_string()
{
  return "mmc";
} 


struct  ILogHandle
{
	virtual  void  handle(std::string mes) = 0;
	virtual  ~ILogHandle(){}
};

struct  CConsoleHandle  : public  ILogHandle
{
public:   
	static  CConsoleHandle*   instance()
	{
		return  new  CConsoleHandle();
	}
public:
	virtual void handle(std::string code)
	{
		/*std::wcout.imbue(std::locale("C"));
		std::wcout << code << std::endl;
		std::wcout.imbue(std::locale(""));*/
		__android_log_print(ANDROID_LOG_DEBUG,"multi-bin-jni","%s",code.c_str());
	}
};

/*class   CTextLog  : public  ILogHandle
{
public:
	static  ILogHandle*      instance(std::string path)
	{
		return  new  CTextLog(path);
	}
	~CTextLog()
	{
  	    stream.close();
  	    stream.imbue(std::locale("C"));
	}
    
public:
	virtual void handle(std::wstring code)
	{
		std::string text  = get_time_string();
		stream << s2ws(text) << L":  " << code << std::endl;
	}
	std::wofstream stream;
public:
	CTextLog(std::string path)
	{
	    std::wifstream  istream;
	    istream.open(path.c_str());
	    if (istream.is_open())
	    {
	    	istream.seekg(0,std::ios::end);
	    	int length = istream.tellg();
	    	istream.close();
	    	if (length > 2 * 1024 *1024) 
	    	{
	    		remove(path.c_str());
	    	}
	    }
		std::locale::global(std::locale(""));
		stream.open(path.c_str(),std::ios::app);
	}
    
	CTextLog(CTextLog&){}
};*/

class   LogWrapper
{
private:
	ILogHandle*  log;
    mutex  mutx;
public:
	void  handle(std::string code)
	{
		if (log)  log->handle(code);
	}
	void  handle(const char* code)
	{
		if (log)
		{
		  log->handle(std::string(code));
		}
	}
	void  Setlog(ILogHandle* log)
	{
		this->log = log;
	}
	LogWrapper(ILogHandle* ptr){log = ptr;}
	LogWrapper(){log = 0;}
	void  SetLog(ILogHandle* ptr)
	{
		log = ptr;
	}
	ILogHandle*   operator&(){return  log;}
};

#endif
