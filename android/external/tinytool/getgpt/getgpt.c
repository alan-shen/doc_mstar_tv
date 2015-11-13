#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define DEBUG (0)
#define DBG if(DEBUG)printf

#define MMCDEV 		"/dev/block/mmcblk0"

/*
 * Size Of
 */
#define BLOCKSIZE		(512)
#define MBRSIZE			(BLOCKSIZE)	// 512
#define GPTHEADERSIZE		(BLOCKSIZE)	// 512
#define GPTENTRYSIZE		(BLOCKSIZE/4)	// 128
#define MAXENTRYNUM		(128)

/*
 * GUID Partition Table Scheme
 * 
 *     --------------------------------------------
 *     | LBA0    -    Protective MBR
 *     --------------------------------------------
 *     | LBA1    -    Primary GPT Header
 *     --------------------------------------------
 *     | LBA2    -    Entry 001~004
 *     | LBA3    -    Entry 005~008
 *     | LBA4    -    Entry 009~012
 *     | ... ... 
 *     | LBA33   -    Entry 125~128    (total 128)
 *     --------------------------------------------
 *     | LBA34  
 *     | ... ... 
 */

enum {
	ENUM_BLKADDR_MBR		= 0,
	ENUM_BLKADDR_PRIGPT_HEAD	= 1,
	ENUM_BLKADDR_PRIGPT_ENTRY_START	= 2,
	ENUM_BLKADDR_PRIGPT_ENTRY_END	= 33
};

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

struct gpt_header {
	char sig_name[8];
	int version[4];
	int header_size[4];
	int pri_head_addr;
	int sec_head_addr;
	int part_start;
	int part_end;
}gGPTPriHeader, gGPTSecHeader;

int check_if_efi(char* gptheader)
{
	int i;
	char efi_sig[8] = {0x45, 0x46, 0x49, 0x20, 0x50, 0x41, 0x52, 0x54}; // "EFI PART"

	dump_mem("primary gpt header", gptheader, 96);

	for(i=0; i<8; i++){
		if( gptheader[i] != efi_sig[i] ){
			printf("This is not a EFI partition.\n");
			return 1;
		}
	}

	int part_can_use=0;
	for(i=0; i<4; i++){
		gGPTPriHeader.pri_head_addr |= ((gptheader[24+i]&0xFF)<<(i*2*4));
		gGPTPriHeader.sec_head_addr |= ((gptheader[32+i]&0xFF)<<(i*2*4));
		gGPTPriHeader.part_start    |= ((gptheader[40+i]&0xFF)<<(i*2*4));
		gGPTPriHeader.part_end      |= ((gptheader[48+i]&0xFF)<<(i*2*4));
	}
	part_can_use = gGPTPriHeader.part_end - gGPTPriHeader.part_start +1;
	
	printf("\n");
	printf("  Primary GPT header block address : 0x%08X(%d)\n", gGPTPriHeader.pri_head_addr, gGPTPriHeader.pri_head_addr);
	printf("Secondary GPT header block address : 0x%08X(%d)\n", gGPTPriHeader.sec_head_addr, gGPTPriHeader.sec_head_addr);
	printf("     Partition start block address : 0x%08X(%d)\n", gGPTPriHeader.part_start, gGPTPriHeader.part_start);
	printf("                 end block address : 0x%08X(%d)\n", gGPTPriHeader.part_end, gGPTPriHeader.part_end);
	printf("                     block can use : 0x%08X(%d)\n\n", part_can_use, part_can_use);

	return 0;
}

int parse_gpt_part(char* gpttable)
{
	int i, j;
	unsigned int lba_start, lba_end, psize_k, psize_m;
	char part_name[80];

	printf("\t          START         END       TOTAL                 NAME\n");
	printf("\t--------------------------------------------------------------------------------\n");

	for( j=0; j<MAXENTRYNUM; j++ ){
		/* Check if this is the last partition */
		for(i=0; i<32; i++)
			if( gpttable[i+j*GPTENTRYSIZE] != 0 )
				break;
		if(i==32)
			break;

		/* parse the part name */
		for(i=0; i<72; i++){
			part_name[i] = gpttable[56+i*2+j*GPTENTRYSIZE]&0xFF;
		}

		/* Check the start block address */
		lba_start &= 0x00000000;
		for( i=4; i<8; i++ ){
			lba_start |= ((gpttable[32+8-1-i+j*GPTENTRYSIZE]&0xFF)<<((8-1-i)*2*4));
		}

		/* Check the end block address */
		lba_end &= 0x00000000;
		for( i=4; i<8; i++ ){
			lba_end |= ((gpttable[32+8+8-1-i+j*GPTENTRYSIZE]&0xFF)<<((8-1-i)*2*4));
		}

		/* caluate the partition size of KB */
		psize_k = (lba_end - lba_start+1)/2;
		/* caluate the partition size of MB */
		psize_m = (lba_end - lba_start+1)/2/1024;

		/* Show partition info */
		printf("\t[%02d] 0x%08X, 0x%08X, %10d - %10dK - %10dM - (%s)\n", j+1, lba_start, lba_end, (lba_end-lba_start+1), psize_k, psize_m, part_name);
	}
	
	printf("\t--------------------------------------------------------------------------------\n");
	return 0;
}

/*
 * Main Entry
 */
int main(int argc, char **args)
{
	int fd_mmc;
	char *buffer;

	/* open the original block device */
	fd_mmc = open(MMCDEV, O_RDWR);
	if( fd_mmc < 0 ){
		printf("Open the block dev(%s) error\n", MMCDEV);
		return -1;
	}

	/* alloc memory for read the MBR + Primary_GPT */
	int bufsize = BLOCKSIZE*( ENUM_BLKADDR_PRIGPT_ENTRY_END+1 );
	buffer = malloc( bufsize );
	if( buffer == NULL ){
		printf("Alloc memmory error\n");
		return -1;
	}
	memset( buffer, 0, bufsize );

	/* read back the whole MBR + Primary_GPT */
	if( bufsize != read( fd_mmc, buffer, bufsize ) ){
		printf("Read error.\n");
	}

	/* check if the partition is a efi partition? */
	if( check_if_efi( &buffer[ENUM_BLKADDR_PRIGPT_HEAD*BLOCKSIZE] ) == 0 ){
		/* parse the partition table... */
		parse_gpt_part( &buffer[ENUM_BLKADDR_PRIGPT_ENTRY_START*BLOCKSIZE] );
	}

	/* clean */
	free(buffer);
        return 0;
}

