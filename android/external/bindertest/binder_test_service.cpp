#include "binder_test.h"
#include <cutils/log.h>

using namespace android;

int main(int argc, char *argv[])
{
	sp<ProcessState> proc(ProcessState::self());
	sp<IServiceManager> sm = defaultServiceManager();

	sm->addService(String16("service.testmanager"), new Test());

	ALOGE("bindertest service is starting.....");
	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();

	return 0;
}
