#ifndef _CMCC_SECURE_H
#define _CMCC_SECURE_H

#ifdef __cplusplus
extern "C" {
#endif

#define SECURITY_PARTITION "/dev/block/platform/mtk-msdc.0/by-name/security"

#define CMCC_FLAG_SIZE          (50)

#define CMCC_FLAG_OFFSET_BIND		(00)
#define CMCC_FLAG_OFFSET_LOCK		(CMCC_FLAG_SIZE + CMCC_FLAG_OFFSET_BIND)
#define CMCC_FLAG_OFFSET_WIPE		(CMCC_FLAG_SIZE + CMCC_FLAG_OFFSET_LOCK)
#define CMCC_FLAG_OFFSET_ACTIVATE	(CMCC_FLAG_SIZE + CMCC_FLAG_OFFSET_WIPE)	/* 20150204, to jianfeng, for sava the flag of activate state */

#define WIPE_INIT	(0)
#define WIPE_WAIT	(1)
#define WIPE_OVER	(2)

enum ecmcc_flag{
	eCMCCFLAGOFFSET_BIND = 0,
	eCMCCFLAGOFFSET_LOCK = 50,
	eCMCCFLAGOFFSET_WIPE = 100,
	eCMCCFLAGOFFSET_ACTIVATE = 150,												/* 20150204, as same as "CMCC_FLAG_OFFSET_ACTIVATE" */
};

struct cmcc_security_state {
    char bind[50];
    char lock[50];
    int  wipe;
};

int get_cmcc_secure_state(struct cmcc_security_state *out);
int set_cmcc_security_wipe_finish(void);

#ifdef __cplusplus
}
#endif

#endif
