#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "cmcc_secure.h"

#define DEBUG (0)
#define DBG if(DEBUG)printf

//#define __WRITE__
//#define __READ__
/*
 * Main Entry
 */
int main(int argc, char *argv[])
{
	int dev = -1; 
	char dev_name[256];
	int count;

	strncpy(dev_name, SECURITY_PARTITION, sizeof(SECURITY_PARTITION));

#ifdef __WRITE__
	char tmp_bind[CMCC_FLAG_SIZE];
	char tmp_lock[CMCC_FLAG_SIZE];
	int  tmp_wipe;

	/* open block device(partition) */
	dev = open(dev_name, O_WRONLY | O_SYNC);

	/* write bind flag */
    strlcpy(tmp_bind, "bind", CMCC_FLAG_SIZE);
	lseek(dev, CMCC_FLAG_OFFSET_BIND, SEEK_SET);
	write(dev, tmp_bind, sizeof(tmp_bind));

	/* write lock flag */
    strlcpy(tmp_lock, "lock", CMCC_FLAG_SIZE);
	lseek(dev, CMCC_FLAG_OFFSET_LOCK, SEEK_SET);
	write(dev, tmp_lock, sizeof(tmp_lock));

	/* write wipe flag */
	if(argc > 1){
		tmp_wipe = atoi(argv[1]);
		printf("\n>> will set wipe flag to [%d]\n\n", tmp_wipe);
	}
	else{
		tmp_wipe = 1;
		printf("\n>> will set default wipe flag [%d]\n\n", tmp_wipe);
	}
	lseek(dev, CMCC_FLAG_OFFSET_WIPE, SEEK_SET);
	write(dev, &tmp_wipe, sizeof(tmp_wipe));

	close(dev);
#endif	

#ifdef __READ__
#if 0
	char rd_bind[CMCC_FLAG_SIZE];
	char rd_lock[CMCC_FLAG_SIZE];
	int  rd_wipe;

	/* read back */
	dev = open(dev_name, O_RDONLY);

	/* bind flag */
	lseek(dev, CMCC_FLAG_OFFSET_BIND, SEEK_SET);
	read(dev, &rd_bind, sizeof(rd_bind));
	printf("\t[CMCC][BIND] %s\n", rd_bind);

	/* lock flag */
	lseek(dev, CMCC_FLAG_OFFSET_LOCK, SEEK_SET);
	read(dev, &rd_lock, sizeof(rd_lock));
	printf("\t[CMCC][LOCK] %s\n", rd_lock);

	/* wipe flag */
	lseek(dev, CMCC_FLAG_OFFSET_WIPE, SEEK_SET);
	read(dev, &rd_wipe, sizeof(rd_wipe));
	printf("\t[CMCC][WIPE] 0x%x\n\n", rd_wipe);

	close(dev);
#else
	struct cmcc_security_state cmcc;

	dev = open(dev_name, O_RDONLY);
	read(dev, &cmcc, sizeof(cmcc));
	printf("\t[CMCC][BIND] %s\n",     cmcc.bind);
	printf("\t[CMCC][LOCK] %s\n",     cmcc.lock);
	printf("\t[CMCC][WIPE] 0x%x\n\n", cmcc.wipe);
	close(dev);

#endif
#endif

	return 0;
}
