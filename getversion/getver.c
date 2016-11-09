/* Header Files */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#define LENGTH_VERSION (2)

#define PATH_IA9Q_ALL "/system/etc/firmware/IA9Q_total.bin"
#define PATH_IA9Q_SPK "/system/etc/firmware/IA9Q_spk.bin"
#define PATH_IA9Q_MIC "/system/etc/firmware/IA9Q_mic.bin"

#define OFFSET_MODE1  (0x3f1c)
#define OFFSET_MODE2  (0x7f1c)
#define OFFSET_SPK    (0x1f1c)
#define OFFSET_MIC    (0x1f1c)

int _getversion(char *path, int offset, char *ver)
{
	FILE* fd;
	fd = fopen(path, "r" );
	if ( fd == NULL ){
		printf( "\tOpen file [%s] failed\n", path );
		return -1;
	}
	fseek(fd, offset, SEEK_SET);
	fread(ver, 1, 2, fd);
	fclose(fd);
	return 0;
}

int main( int argc, char** argv )
{
	unsigned char ver[2];

	_getversion(PATH_IA9Q_ALL, OFFSET_MODE1, ver);
	printf("[dongle][mode1] %02x.%02x\n", ver[0], ver[1]);

	_getversion(PATH_IA9Q_ALL, OFFSET_MODE2, ver);
	printf("[dongle][mode2] %02x.%02x\n", ver[0], ver[1]);

	_getversion(PATH_IA9Q_SPK, OFFSET_SPK, ver);
	printf("          [spk] %02x.%02x\n", ver[0], ver[1]);

	_getversion(PATH_IA9Q_MIC, OFFSET_MIC, ver);
	printf("          [mic] %02x.%02x\n", ver[0], ver[1]);

	return 0;
}
