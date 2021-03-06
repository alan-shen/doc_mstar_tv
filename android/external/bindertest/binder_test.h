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

/****************************** ITest **********************************/
class ITest : public IInterface
{
	public:
	DECLARE_META_INTERFACE(Test);
	virtual void getTest() = 0;
	virtual void getName() = 0;
};

/****************************** BnTest **********************************/
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
	virtual void getTest() {printf("Class Test:public BnTest, getTest()\n");};
	virtual void getName() {printf("Class Test:public BnTest, getName()\n");};
	void         print()   {printf("Class Test:public BnTest, print()  \n");};
};

/********************** COMMAND DEFINE **********************************/
enum {
	PRINT = IBinder::FIRST_CALL_TRANSACTION,
	NAME,
};

/****************************** BpTest **********************************/
class BpTest : public BpInterface<ITest>
{
	public:
	BpTest(const sp<IBinder>& impl ):BpInterface<ITest> (impl) {

	}
	virtual void getTest() {
		printf("BpTest - getTest()\n");
		Parcel data, reply;
		data.writeInterfaceToken(ITest::getInterfaceDescriptor());
		printf("BpTest - getTest() - transact()->PRINT\n");
		remote()->transact(PRINT, data, &reply);
		printf("BpTest - getTest() - result: %d\n",reply.readInt32());
	}

	virtual void getName()
	{
		printf("BpTest - getName()\n");
		Parcel data, reply;
		data.writeInterfaceToken(ITest::getInterfaceDescriptor());
		printf("BpTest - getName() - transact()->NAME \n");
		remote()->transact(NAME, data, &reply);
		printf("BpTest - getName() - result: %d\n",reply.readInt32());
	}
};

/******************************  **********************************/
IMPLEMENT_META_INTERFACE(Test,"android.TestServer.ITest");

/****************************** BnTest.onTransact **********************************/
status_t BnTest::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
	if (!flags)
		printf("\n");

	switch(code) {
	case PRINT:
		{
			printf("BnTest::onTransact() Command PRINT\n");
			CHECK_INTERFACE(ITest, data, reply);
			getTest();
			reply->writeInt32(100);
			return NO_ERROR;
		}
		break;
	case NAME:
		{
			printf("BnTest::onTransact() Command NAME\n");
			CHECK_INTERFACE(ITest, data, reply);
			getName();
			reply->writeInt32(90);
			return NO_ERROR;
		}
		break;
	default:
		break;
	}

	return NO_ERROR;
}

}// namespace android

#endif
