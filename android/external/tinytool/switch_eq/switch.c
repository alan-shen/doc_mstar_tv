/* Header Files */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <tinyalsa/asoundlib.h>

#define MATRIX_CODEC_EQTBL_STANDARD "table_0"
#define MATRIX_CODEC_EQTBL_NEWS     "news"
#define MATRIX_CODEC_EQTBL_MOVIE    "movie"

#define CTRLS_EQDRC_ENABLE_EN        (0)
#define CTRLS_EQDRC_ENABLE_DIS       (1)

#define CTRLS_NAME_EQTBL_AMP         "AMP EQ Mode"

//Dolby/DTS wiil not use console/adb to connect TV, just save the state here
static int get_aml_card() {
	int card = -1, err = 0;
	int fd = -1;
	unsigned fileSize = 512;
	char *read_buf = NULL, *pd = NULL;
	static const char *const SOUND_CARDS_PATH = "/proc/asound/cards";

	fd = open(SOUND_CARDS_PATH, O_RDONLY);
	if (fd < 0) {
		printf("ERROR: failed to open config file %s error: %d\n",
			SOUND_CARDS_PATH, errno);
		close(fd);
		return -EINVAL;
	}

	read_buf = (char *) malloc(fileSize);
	if (!read_buf) {
		printf("Failed to malloc read_buf");
		close(fd);
		return -ENOMEM;
	}
	memset(read_buf, 0x0, fileSize);
	err = read(fd, read_buf, fileSize);
	if (fd < 0) {
		printf("ERROR: failed to read config file %s error: %d\n",
			SOUND_CARDS_PATH, errno);
		close(fd);
		return -EINVAL;
	}
	pd = strstr(read_buf, "AML");
	card = *(pd - 3) - '0';
OUT:
	free(read_buf);
	close(fd);
	return card;
}

/* for matrix only */
static int switch_amp_codec_eq(char *mode) {
	struct mixer *pmixer;
	struct mixer_ctl *pEqMode_Amp;
	int card_id;
	int ret = 0;

	card_id = get_aml_card();

	pmixer = mixer_open(card_id);
	if (NULL == pmixer) {
		printf("[%s:%d] Failed to open mixer\n", __FUNCTION__, __LINE__);
		goto err_exit;
	}

	pEqMode_Amp = mixer_get_ctl_by_name(pmixer, CTRLS_NAME_EQTBL_AMP);

	if (NULL != pEqMode_Amp) {
		printf("[%s:%d] Switch Codec's EQ table to tbl_%s\n", __FUNCTION__, __LINE__, mode);
		ret = mixer_ctl_set_enum_by_string(pEqMode_Amp, mode);
		if (ret) {
			printf("ret: %d\n", ret);
		}
	} else {
		printf("fail to set mixer!\n");
	}

	if (NULL != pmixer) {
		mixer_close(pmixer);
	}
	return 0;
err_exit:
	return -1;
}

int CodecSetEqMode(int mode) {
    printf("%s,%d: %d\n",__FUNCTION__, __LINE__, mode);

	switch (mode) {
	/* standard */
	case 0:
		switch_amp_codec_eq(MATRIX_CODEC_EQTBL_STANDARD);
		break;
	/* news */
	case 1:
		switch_amp_codec_eq(MATRIX_CODEC_EQTBL_NEWS);
		break;
	/* movie */
	case 2:
		switch_amp_codec_eq(MATRIX_CODEC_EQTBL_MOVIE);
		break;
	default:
		switch_amp_codec_eq(MATRIX_CODEC_EQTBL_STANDARD);
		break;
	}

	return 0;
}

int main( int argc, char** argv )
{
	unsigned char ver[2];

	printf("argc: %d\n", argc);
	if (argc ==2) {
		printf("argv: %s,%s\n", argv[0], argv[1]);
		if (!strcmp(argv[1], "std"))
			CodecSetEqMode(0);
		else if (!strcmp(argv[1], "movie"))
			CodecSetEqMode(2);
		else if (!strcmp(argv[1], "news"))
			CodecSetEqMode(1);
		else
			CodecSetEqMode(0);
	} else {
		CodecSetEqMode(0);
	}

	return 0;
}
