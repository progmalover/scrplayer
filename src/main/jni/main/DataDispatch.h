//
// Created by root on 16-10-28.
//

#ifndef VDI_SCRPLAYER_DATADISPATCH_H
#define VDI_SCRPLAYER_DATADISPATCH_H

#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "tcpmoduledefine.h"
typedef struct Data{
    uint8_t *data;
    int len;
} DATA;

typedef std::list<DATA>  QQueue;
class DataDispatch {
    class QDataQueue{
       public:
            QDataQueue();
            ~QDataQueue();

       protected:
            void Run();

       public:
            void calcuQ();
            void push(uint8_t *,int);
            void SetDispachFunc( dealpacketfunctype);
       private:
            QQueue  mQ;
            std::mutex mqtx;
            std::mutex metx;
            std::thread *mQTrd;
            std::condition_variable mcv;
            dealpacketfunctype  _dispfunc;
    };
    public:
        DataDispatch(dealpacketfunctype dispfunc);
        DataDispatch();
        ~DataDispatch();

        void bindDspFunc(dealpacketfunctype dispfunc);
        bool Dispatch(ID,uint8_t *, int );
        static DataDispatch *getInst();

    protected:
        QDataQueue mDQ;

};


#endif //VDI_SCRPLAYER_DATADISPATCH_H
