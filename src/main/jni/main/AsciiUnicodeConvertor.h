#ifndef _ASCII_UNICODE_CONVERTOR_H_

#define _ASCII_UNICODE_CONVERTOR_H_

#include <stdio.h>
#include <cstdlib>
#include "auto_tchar.h"

inline   void    tchtombs(const TCHAR* src,char* dest)
{	
#ifndef _UNICODE
	strcpy(dest,src);
#else
	int i = wcstombs( dest, src, 256);
	dest[i] = '\0';
#endif
}

inline    void  tchtowbs(const TCHAR* src,wchar_t* dest)
{
#ifndef _UNICODE
	int i = mbstowcs( dest, src, 256);
	dest[i] = '\0';
#else
	wcscpy( dest, src);
#endif
}

inline   void    mbstotch(const char* src,TCHAR* dest)
{	
#ifndef _UNICODE
	strcpy(dest,src);
#else
	int i = mbstowcs( dest, src, 256);
	dest[i] = '\0';
#endif
}

inline    void  wbstotch(const wchar_t* src,TCHAR* dest)
{
#ifndef _UNICODE
	int i = wcstombs( dest, src, 256);
	dest[i] = '\0';
#else
	wcscpy( dest, src);
#endif
}


#include <string> 


  

using namespace std;  
  

inline string ws2s(const wstring& ws)  
{   
    const wchar_t * pw = ws.c_str();
    string val = "";  
    if(!pw)  
    {  
        return val;  
    }  
    size_t size= wcslen(pw)*sizeof(wchar_t);  
    char *pc = NULL;  
    if(!(pc = (char*)malloc(size)))  
    {  
        return val;  
    }  
    size_t destlen = wcstombs(pc,pw,size);  
    /*转换不为空时，返回值为-1。如果为空，返回值0*/  
    if (destlen ==(size_t)(0))  
    {  
        return val;  
    }  
    val = pc;  
    delete pc;  
    return val; 
}

inline wstring s2ws(const string& s)  
{   
    const char * pc = s.c_str();
    wstring val = L"";  
  
    if(NULL == pc)  
    {  
        return val;  
    }  
    //size_t size_of_ch = strlen(pc)*sizeof(char);  
    //size_t size_of_wc = get_wchar_size(pc);  
    size_t size_of_wc;  
    size_t destlen = mbstowcs(0,pc,0);  
    if (destlen ==(size_t)(-1))  
    {  
        return val;  
    }  
    size_of_wc = destlen+1;  
    wchar_t * pw  = new wchar_t[size_of_wc];  
    mbstowcs(pw,pc,size_of_wc);  
    val = pw;  
    delete pw;  
    return val;  
}


#endif