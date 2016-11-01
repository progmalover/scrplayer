#ifndef _PARSE_WRAPPER_H_2014_05_08_LC_
#define _PARSE_WRAPPER_H_2014_05_08_LC_

#include <string>

struct  ParseWrapper
{
	static  std::wstring  Parse(std::wstring params,int  index,std::wstring sp = L"/")
	{
		int count = 0,start  =  0,end   =  0;
		std::wstring src;
		do 
		{
			start  =   params.find(sp,start);
			start +=1;
			++count; 
		} while (count != (index + 1));
		end  =  params.find(sp,start);
		src  =  params.substr(start,end - start);
		return  src;
	}
	static  void Integrate(std::wstring t,std::wstring& s,std::wstring sp = L"/")
	{
		s += t;
		s += sp;
	}

	static  std::string  ParseA(std::string params,int  index,std::string sp = "/")
	{
		int count = 0,start  =  0,end   =  0;
		std::string src;
		do 
		{
			start  =   params.find(sp,start);
			start +=1;
			++count; 
		} while (count != (index + 1));
		end  =  params.find(sp,start);
		src  =  params.substr(start,end - start);
		return  src;
	}

	static  void IntegrateA(std::string t,std::string& s,std::string sp = "/")
	{
		s += t;
		s += sp;
	}

};

#endif // _DEBUG
