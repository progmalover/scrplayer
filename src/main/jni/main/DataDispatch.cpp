//
// Created by zhangxiangyang on 16-10-28.
//

#include "DataDispatch.h"
#include <condition_variable>
#include <functional>
#include <algorithm>
#include "SDL_logger.h"
#include "../SDL2-2.0.4/src/core/android/SDL_android.h"
  DataDispatch::DataDispatch(dealpacketfunctype dispfunc){
      mDQ.SetDispachFunc(dispfunc);
  }

  DataDispatch::DataDispatch(){

  }

  DataDispatch::~DataDispatch(){

  }

  void DataDispatch::bindDspFunc(dealpacketfunctype dispfunc){
      mDQ.SetDispachFunc(dispfunc);
  }

  bool DataDispatch::Dispatch(ID,uint8_t *data, int len)
  {
     mDQ.push(data,len);
     return true;
  }


  DataDispatch * DataDispatch::getInst()
  {
        static DataDispatch  *_inst = nullptr;
        if(_inst == nullptr)
        {
            _inst = new DataDispatch();
        }

        return _inst;
  }
 ////////////////////////////////////////////////////////////////////////////////////////////////////////
 // QDataQueue
  DataDispatch::QDataQueue::QDataQueue():mQTrd(nullptr),_dispfunc(nullptr){}
  DataDispatch::QDataQueue::~QDataQueue(){}

  const int  QCAPA_SIZE = 5 ; //buffer for 5 frames.
  void DataDispatch::QDataQueue::calcuQ()
  {
      int qsize = mQ.size();

       //LOGI("mq size:%d",qsize);
      if(qsize <= 0 && qsize < QCAPA_SIZE)
        return;

      int retiredFrame = qsize - QCAPA_SIZE;
      for(auto it = mQ.begin(); it != mQ.end();it++)
       {
          DATA &d = *it;
          delete d.data;
          LOGI("SDL is busy,Free one frame....");
          mQ.pop_front();
          retiredFrame--;
          if(retiredFrame < 0)
            break;
       }
       //mQ.clear();

  }

   void DataDispatch::QDataQueue::SetDispachFunc( dealpacketfunctype func)
   {
        _dispfunc = func;
   }

  //process datas
  void DataDispatch::QDataQueue::Run()
  {

        while(true)
        {
            //waiting for condation occured then starting data processing.
            //
            std::unique_lock<std::mutex> ulck(metx);
            mcv.wait(ulck);
            {
                int len = 0;
                uint8_t *Data = nullptr;
                int sz = mQ.size();
              //process all datas in the q
                while(sz > 0)
                {
                   // get data from q in multi-thread safe
                    do{
                    std::lock_guard<std::mutex> lck(mqtx);
                    DATA &data = *mQ.begin();
                    len = data.len;
                    Data = data.data;
                    mQ.pop_front();
                    sz = mQ.size();
                    }while(0);
                    //parse and play data
                    ID id;
                    if(_dispfunc != nullptr)
                    _dispfunc(id,Data,len);

                    delete(Data);

                }

            }

        }

  }

  //ｉｎｓｅｒｔ data to queque ,this function will be called by another thread　
  //lock_guard used for mq protection 
  void DataDispatch::QDataQueue::push(uint8_t *Data,int len)
  {
        std::lock_guard<std::mutex> lck(mqtx);

        if(mQTrd == nullptr)
        {
            mQTrd = new std::thread(std::bind(&DataDispatch::QDataQueue::Run,this));
            //mQTrd->start();
        }

        calcuQ();//

        DATA data={Data,len};
        mQ.push_back(data);
        mcv.notify_all();
  }
