#include "binder_test.h"

using namespace android;

int main(int argc, char *argv[])
{
	sp<IBinder> binder;
	sp<ProcessState> proc(ProcessState::self());
	sp<IServiceManager> sm = defaultServiceManager();

	do {
		binder = sm->getService(String16("service.testmanager"));
		if(binder != 0)
			break;
		sleep(1);
	} while(true);

	const sp<ITest>& bts = interface_cast<ITest>(binder);

	printf("bindertest client is starting.....\n");
	printf("\n== Client, test getTest() ==\n");
	bts->getTest();

	printf("\n== Client, test getName() ==\n");
	bts->getName();

	return 0;  
} 
