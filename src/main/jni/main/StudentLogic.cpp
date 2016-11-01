#include "StudentLogic.h"
#include "MessageDefine.h"
#include <algorithm>
#include "Mac.h"
#include "DataCacheWrapper.h"

#include <sstream>

using namespace std;
using namespace std::placeholders;


void StudentLogic::Load(get_filter_ip_func _filter,flush_func _flush,struct ILogHandle* _log)
{
	get_filter_ip  =  _filter;

	flush  = _flush;

    log.Setlog(_log);
	
    variable.notify  = bind(&StudentLogic::OnNotify,this,_1);

    render  = new  IH264Render((MouseFuncType)0,_log);
    micRender = new AuidoRender(_log);
    bkRender =  new AuidoRender(_log);
    mp3Render =  new AuidoRender(_log);

}

void StudentLogic::UnLoad()
{
	SafeDeleteWrapper::SafeDelete(render);
    SafeDeleteWrapper::SafeDelete(&log);

    log.Setlog(0);
}

StudentLogic::StudentLogic(std::string mac,std::string ip)
{
	this->mac = mac;
	this->ip = ip;
}

void StudentLogic::NotifyFromClientServerLib(ID id,notify_type type,std::string externs)
{
	if (type  == disconnect)
	{
        log.handle("DisConnect  to " + id.ip);

	}

	else if (type == connected)
	{
   	    Cache<1024>  cache;
		Header  header;
		header.id  = MAC;
		header.operation  = 0;
		header.deliver    = 0;
		std::string sdata  = "/";
		ParseWrapper::IntegrateA(ip,sdata);
		ParseWrapper::IntegrateA(mac,sdata);
		ParseWrapper::IntegrateA(get_filter_ip(),sdata);
		header.externeds  = sdata.size(); 
		cache.write(header);
		cache.write((uint8_t*)sdata.c_str(),sdata.size());
		flush(cache.get(),cache.size());
        log.handle("sended mac information");
	}
}

void StudentLogic::OnNotify( Variable::NotifyType type )
{
	if (type == Variable::IMAGESIZE)
	{
		std::ostringstream oss;
				oss << "OnNotify()-SetAttention yuv420:w " << variable.image_width << " h "<< variable.image_height;
				log.handle(oss.str());

		render->SetAttention(variable.image_width,variable.image_height,L"yuv420p");
	} 
}

void StudentLogic::GetDataFromTeacher(ID,uint8_t* buf,int length)
{
	Header  header  =   *(Header*)buf;

	::Classification  id  =  (::Classification)(header.id);

	if (id == IMAGE)
	{
		//then render
		ImageConfig& image = *(ImageConfig*)(buf + sizeof(header) + header.externeds);
		//std::ostringstream oss;
		//oss << "GetDataFromTeacher():w " << image.width << " h "<< image.heigth;
		//log.handle(oss.str());

		variable.SetImageSize(image.width,image.heigth,image.iFullScreen);
		render->Flush(
		buf + sizeof(header) + sizeof(ImageConfig) + header.externeds,
		length - sizeof(header)- sizeof(ImageConfig) - header.externeds);
	}
	else if (id == AUDIO)
	{
		AudioConfig&  audioConfig =  *(AudioConfig*)(buf + sizeof(header) + header.externeds);
		int  itags  =  audioConfig.itags;

		std::ostringstream ss;
		ss << "id = audio ,itags = "<<itags<<" length＝　"
					<<length << " channel " << audioConfig.ichannel ;

		log.handle(ss.str());

		int datalen = length - sizeof(header) - sizeof(audioConfig) - header.externeds;
		if (itags == 0)
		{
			bkRender->SetAttention(audioConfig.ichannel,datalen);
			bkRender->Flush(buf + sizeof(header) + sizeof(audioConfig) + header.externeds,datalen);

		}
		else if (itags == 1)
		{

			micRender->SetAttention(audioConfig.ichannel,datalen);
			micRender->Flush(buf + sizeof(header) + sizeof(audioConfig) + header.externeds,datalen);
		}
		else if(itags == 2)
		{
			mp3Render->SetAttention(audioConfig.ichannel,length);
			mp3Render->Flush(buf + sizeof(header) + sizeof(audioConfig) + header.externeds,datalen);
		}
	}
	/*
	else if (id ==  MP3STOP)
	{
			log.handle("Mp3 stop...");

			if (header.operation == 1)
			{
				mp3Render->Enable(false);

			}
			else if (header.operation == 0)
			{
				mp3Render->Enable(true);
			}
	}*/else
	{

		std::ostringstream ss;
		ss << "no implment data id  = " << id ;
		log.handle(ss.str());

	}


}

void StudentLogic::Variable::SetImageSize( int width,int height,int ifullscreen)
{
	if (this->image_width != width || this->image_height != height || this->fullscreen != ifullscreen)
	{

		this->image_width =  width;
		this->image_height = height;
		this->fullscreen  =  ifullscreen;
		notify(StudentLogic::Variable::IMAGESIZE);
	}
}



