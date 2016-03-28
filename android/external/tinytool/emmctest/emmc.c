#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define DEBUG (0)
#define DBG if(DEBUG)printf

#define MMCDEV 		"/dev/block/system1"

/*
 * Size Of
 */
#define BLOCKSIZE		(512)
#define MBRSIZE			(BLOCKSIZE)	// 512
#define GPTHEADERSIZE		(BLOCKSIZE)	// 512
#define GPTENTRYSIZE		(BLOCKSIZE/4)	// 128
#define MAXENTRYNUM		(128)

void dump_mem(char* name, char* mem, int size)
{
	int i;
	int addr = 0;

	DBG("\n%s\n", name);
	DBG("-----------------------------------------------------------\n");
	for(i=0; i<size; i++){
		if( i%16==0 ){
			DBG("(0x%04X) ", addr);
			addr += 16;
		}
		DBG(" %02X", mem[i]&0xFF );
		if( i%8==7 ){
			DBG("  ");
		}
		if( i%16==15 ){
			DBG("\n");
		}
	}
	DBG("\n");
}

int main(int argc, char **args)
{
	int fd_mmc;
	char *buf_wr, *buf_rd;
	int i;
	int blknum;
	int bufsize = BLOCKSIZE;
	int testtimes = 0;
	int errblks = 0;

	/* open the original block device */
	fd_mmc = open(MMCDEV, O_RDWR);
	if( fd_mmc < 0 ){
		printf("Open the block dev(%s) error\n", MMCDEV);
		return -1;
	}

	/* Alloc memory */
	buf_wr = malloc( bufsize );
	if( buf_wr == NULL ){
		printf("Alloc memmory error\n");
		return -1;
	}

	buf_rd = malloc( bufsize );
	if( buf_rd == NULL ){
		printf("Alloc memmory error1\n");
		return -1;
	}

	// test 500 MB, blknum = 500*1024*1024/512
	blknum = 500*1024*2;

	while(1){
		testtimes ++;
		errblks = 0;
		if(testtimes%2){
			memset( buf_wr, 0x5A, bufsize );
			printf("==> %d use partten 0x5A\n", testtimes);
		}
		else{
			memset( buf_wr, 0xA5, bufsize );
			printf("==> %d use partten 0xA5\n", testtimes);
		}
		for(i=0; i<blknum; i++){
			//write
			lseek(fd_mmc, bufsize*i, SEEK_SET);
			if(write(fd_mmc, buf_wr, bufsize) != bufsize){
				errblks++;
				printf("\twrite addr: %d blk error!\n", i);
				continue;
			}

			//read
			lseek(fd_mmc, bufsize*i, SEEK_SET);
			if( read( fd_mmc, buf_rd, bufsize) != bufsize){
				errblks++;
				printf("\tread  addr: %d blk error!\n", i);
				continue;
			}

			//compare
			if(0 != memcmp(buf_wr, buf_rd, bufsize)){
				errblks++;
				printf("\tcompare addr: %d blk error!\n", i);
				dump_mem("wr", buf_wr, bufsize);
				dump_mem("rd", buf_rd, bufsize);
			}
			if(errblks){
				printf("\t\t=> err blks %d\n", errblks);
			}
			if(i%1000 == 0){
				printf(".");
			}
		}
	}

	/* clean */
	free(buf_wr);
	free(buf_rd);
    return 0;
}

