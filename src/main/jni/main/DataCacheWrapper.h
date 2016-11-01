#ifndef _DATACACHE_WRAPPER_H_2014_05_14_LC_
#define _DATACACHE_WRAPPER_H_2014_05_14_LC_
#include "SafetyDelete.h"
#include <stdint.h>
#include <fstream>

class  DataCache
{
private:
	int   length;
	uint8_t* buf;
public:
	DataCache(uint8_t* rgb,int _length) :buf(0)
	{
		length  =  _length;
		buf = new uint8_t[length + sizeof(int)];
		memcpy(buf,(char*)&length,sizeof(int));
		memcpy(buf + sizeof(int),rgb,length);
	}

	DataCache(int  fixbytes)
	{
		buf = new uint8_t[fixbytes];
		length  =  0;
	}
	uint8_t*&   get(){return  buf;}
	uint8_t*     data(){return buf + sizeof(int);}
	int    len(){return  length;}
	int    size(){return length + sizeof(int);}
	void  copy(uint8_t* rgb,int _length)
	{
		length  = _length;
		memcpy(buf,(char*)&length,sizeof(int));
		memcpy(buf + sizeof(int),rgb,length);
	}
	~DataCache()
	{
		SafeDeleteWrapper::SafeDelete(buf);
	}
};


class SimpleCache
{
private:
	int   length;
	uint8_t* buf;
public:
	SimpleCache(uint8_t* rgb,int _length) :buf(0)
	{
		buf = new uint8_t[_length];
		memcpy(buf,rgb,_length);
		length  = _length;
	}
	uint8_t*&   get(){return  buf;}
	int    size(){return length;}
	~SimpleCache()
	{
		SafeDeleteWrapper::SafeDelete(buf);
	}
};


class FixedCache
{
private:
	int   length;
	uint8_t* buf;
	int   totals;
public:
	FixedCache(int totals)
	{
		buf = new uint8_t[totals];
		length = 0;
		this->totals = totals;
	}
	uint8_t*&   get(){return  buf;}
	int			len(){return  length;}
	void		add(int offset,uint8_t* rgb,int _length)
	{
		length += _length;
		memcpy(buf + offset,rgb,length);
	}

	bool        IsCompleted()
	{
		return  totals == length;
	}

	~FixedCache()
	{
		SafeDeleteWrapper::SafeDelete(buf);
	}
};

template<int N>
class  Cache
{
public:
	Cache()
	{
		data = new uint8_t[N];
		cur = data;
		length = 0;
	}
	~Cache()
	{
		SafeDeleteWrapper::SafeDelete(data);
	}
private:
	uint8_t* data;
	uint8_t* cur;
	int      length;
public:
	template<typename T>  void Str(T& v,int elments = 1)
	{
		memcpy(cur,v.c_str(),(v.size()+1) * elments);

		cur += (v.size()+1) * elments;

		length += elments * (v.size()+1);
	}

	template<typename T> void   write(T& ptr)
	{
		memcpy(cur,&ptr,sizeof(T));
		cur    += sizeof(T);
		this->length += sizeof(T);
	}

	void   write(uint8_t* buf,int length)
	{
		memcpy(cur,buf,length);
		cur    += length;
		this->length += length;
	}

	void   tobackup(std::string path)
	{
		std::ofstream  stream;
		stream.open(path.c_str(),std::ios_base::out | std::ios_base::binary);
		stream.write((const char*)data,length);
		stream.close();
	}

	uint8_t*  get()
	{
		return  data;
	}

	int    size()
	{
		return  length;
	}

	template<int M> void  cloneto(Cache<M>& cache)
	{
		cache.write(data,length);
	}

	void   reset(int offset = 0)
	{
		cur = data + offset;
		length  = offset;
	}

};
#endif
