#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define DEBUG (0)
#define DBG if(DEBUG)printf

struct bootloader_message {
    char command[32];
    char status[32];
    char recovery[768];
    char stage[32];
    char reserved[224];
};
#define PARA_PARTITION "/dev/block/platform/mtk-msdc.0/by-name/para"

char *stage=NULL;
int main(int argc, char *argv[])
{
	int dev = -1; 
	char dev_name[256];
	int count, count_1;
    struct bootloader_message boot;
	strncpy(dev_name, PARA_PARTITION, sizeof(PARA_PARTITION));

	////////////////////////////////////////////////////////////////////////
	// WRITE
	////////////////////////////////////////////////////////////////////////
#ifdef __WRITE__
    memset(&boot, 0, sizeof(boot));
	dev = open(dev_name, O_WRONLY | O_SYNC);
	if(dev<0){
		printf("error: open error %s\n", strerror(errno));
	}

	strlcpy(boot.command,  "boot-recovery", sizeof(boot.command));
	strlcpy(boot.recovery, "recovery\n",    sizeof(boot.recovery));

	count_1 = write(dev, &boot, sizeof(struct bootloader_message));
	if(count_1<0){
		printf("count : %d %d (%s)\n", count_1, sizeof(struct bootloader_message), strerror(errno));
	}
	close(dev);
#endif
	////////////////////////////////////////////////////////////////////////
	// READ
	////////////////////////////////////////////////////////////////////////
#ifdef __READ__
    memset(&boot, 0, sizeof(boot));
	dev = open(dev_name, O_RDONLY);
	if(dev<0){
		printf("error: open error %s\n", strerror(errno));
	}
	count_1 = read(dev, &boot, sizeof(struct bootloader_message));
	if(count_1<0){
		printf("count : %d %d (%s)\n", count_1, sizeof(struct bootloader_message), strerror(errno));
	}
    printf("\tBoot  command: %.*s\n", (int)sizeof(boot.command),  boot.command);
    printf("\tBoot   status: %.*s\n", (int)sizeof(boot.status),   boot.status);
    printf("\tBoot recovery: %.*s\n", (int)sizeof(boot.recovery), boot.recovery);
	close(dev);
#endif
	////////////////////////////////////////////////////////////////////////

	return 0;
}
