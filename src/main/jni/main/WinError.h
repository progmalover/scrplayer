#ifndef _LINUX_ERROR_H_2015_06_09_LC_
#define _LINUX_ERROR_H_2015_06_09_LC_

#include "unistd.h"
#include <string>

inline std::wstring ResolveErrorW()
{
	std::wstring  wserror =  s2ws(strerror(errno));
	return  wserror;
}

inline std::string ResolveErrorA()
{
	std::string  serror =  strerror(errno);
	return  serror;
}

#define ResolveError  ResolveErrorA

#endif