#ifndef __BINDER_TEST_H  
#define __BINDER_TEST_H  
#include <binder/Parcel.h>  
#include <binder/IPCThreadState.h>  
#include <binder/ProcessState.h>  
#include <binder/IServiceManager.h>  
#include <binder/IBinder.h>  
#include <binder/IInterface.h>  
#include <unistd.h>  
#include <stdio.h>  
#include <utils/misc.h>  
#include <utils/Atomic.h>  
#include <utils/Log.h>  
  
namespace android  
{
	/****************************** BnTest **********************************/  
    class ITest : public IInterface  
    {  
        public:  
            DECLARE_META_INTERFACE(Test);  
            virtual void getTest() = 0;  
    };  
  
    class BnTest: public BnInterface<ITest>  
    {  
        public:  
            virtual status_t onTransact( uint32_t code,  
                    const Parcel& data,  
                    Parcel* reply,  
                    uint32_t flags = 0);  
    };  
  
    class Test : public BnTest  
    {  
        public:  
            //Test(){ALOGE("got the service");};  
            //~Test();  
            virtual void getTest(){ALOGE("got the service");};  
            void print(){ALOGE("got the service");};  
    };  
  
	/****************************** BpTest **********************************/  
    enum {  
        PRINT = IBinder::FIRST_CALL_TRANSACTION,  
    };  
  
    class BpTest : public BpInterface<ITest>  
    {  
        public:  
            BpTest(const sp<IBinder>& impl ):BpInterface<ITest>(impl)  
        {  
  
        }  
            virtual void getTest()  
            {  
                printf("in the get Test\n");  
                Parcel data, reply;  
                data.writeInterfaceToken(ITest::getInterfaceDescriptor());  
                remote()->transact(PRINT, data, &reply);  
                printf("send Print %d\n",reply.readInt32());  
            }  
  
    };  
  
	/******************************  **********************************/  
    IMPLEMENT_META_INTERFACE(Test,"android.TestServer.ITest");  
  
	/****************************** BnTest.onTransact **********************************/  
    status_t BnTest::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)  
    {  
        switch(code)  
        {  
            case PRINT:  
                {  
                    printf("got the client msg\n");  
                    CHECK_INTERFACE(ITest, data, reply);  
                    getTest();  
                    reply->writeInt32(100);  
  
                    return NO_ERROR;  
                }break;  
            default:break;  
        }  
        return NO_ERROR;  
    }  
  
}// namespace android  
  
  
  
#endif 
