////////////////////////////////////////////////////////////////////////////////
//
//  http://blog.csdn.net/linweig/article/details/5193976
//
//  udevmonitor
//
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <getopt.h>

#define UEVENT_MSG_LEN 4096

/*************************************** Version Info ************************************* */
#define DESCRIPTION ""
#define VERSION     "alpha 0.1.0"
#define AUTHOR      "shenpengru@gmail.com"
/*************************************** Version Info **************************************/
#define SHOW_VERSION    (1)
#define HIDE_VERSION    (0)
void help(int version)
{
    if(version){
        printf("\n\t========== print uevent info =============\n\n");
        printf("\tDescription : %s\n", DESCRIPTION);
        printf("\tVersion     : %s\n", VERSION );
        printf("\tAuthor      : %s\n", AUTHOR );
        printf("\n\t==========================================\n\n");
    }
    else{
        printf("\n\tUsage:\n" );
        printf("\t\tgetuevent --filter block\n");
    }
}

struct luther_gliethttp {
    const char *action;
    const char *path;
    const char *subsystem;
    const char *firmware;
    int major;
    int minor;
};

static int  open_luther_gliethttp_socket(void);
static void parse_event(const char *msg, struct luther_gliethttp *luther_gliethttp);
static char filter[50]={"NO"};

static int open_luther_gliethttp_socket(void)
{
    struct sockaddr_nl addr;
    int sz = 64*1024;
    int s;

    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();
    addr.nl_groups = 0xffffffff;

    s = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (s < 0)
        return -1;

    setsockopt(s, SOL_SOCKET, SO_RCVBUFFORCE, &sz, sizeof(sz));

    if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        close(s);
        return -1;
    }

    return s;
}

static void parse_event(const char *msg, struct luther_gliethttp *luther_gliethttp)
{
    luther_gliethttp->action = "";
    luther_gliethttp->path = "";
    luther_gliethttp->subsystem = "";
    luther_gliethttp->firmware = "";
    luther_gliethttp->major = -1;
    luther_gliethttp->minor = -1;

    /* currently ignoring SEQNUM */
    //printf("========================================================\n");
    while (*msg) {

        //printf("%s\n", msg);

        if (!strncmp(msg, "ACTION=", 7)) {
            msg += 7;
            luther_gliethttp->action = msg;
        } else if (!strncmp(msg, "DEVPATH=", 8)) {
            msg += 8;
            luther_gliethttp->path = msg;
        } else if (!strncmp(msg, "SUBSYSTEM=", 10)) {
            msg += 10;
            luther_gliethttp->subsystem = msg;
        } else if (!strncmp(msg, "FIRMWARE=", 9)) {
            msg += 9;
            luther_gliethttp->firmware = msg;
        } else if (!strncmp(msg, "MAJOR=", 6)) {
            msg += 6;
            luther_gliethttp->major = atoi(msg);
        } else if (!strncmp(msg, "MINOR=", 6)) {
            msg += 6;
            luther_gliethttp->minor = atoi(msg);
        }

        /* advance to after the next /0 */
        while(*msg++)
            ;
    }

#if 0
    printf("event { '%s', '%s', '%s', '%s', %d, %d }\n",
                    luther_gliethttp->action, luther_gliethttp->path,luther_gliethttp->subsystem,
                    luther_gliethttp->firmware, luther_gliethttp->major,luther_gliethttp->minor);
	printf("SUBSYSTEM: %s\n", luther_gliethttp->subsystem);
	printf("   ACTION: %s\n", luther_gliethttp->action);
	printf("  DEVPATH: %s\n", luther_gliethttp->path);
	printf(" FIRMWARE: %s\n", luther_gliethttp->firmware);
	printf("    MAJOR: %d, MINOR: %d\n", luther_gliethttp->major, luther_gliethttp->minor);
	printf("\n");
#else
	if(!strncmp(filter, "NO", 2)){
		printf("[%3d:%-3d], '%s', '%s', '%s', '%s'\n", luther_gliethttp->major, luther_gliethttp->minor,
					luther_gliethttp->subsystem, luther_gliethttp->action,luther_gliethttp->path,luther_gliethttp->firmware);
	}
	else{
		if (!strncmp(luther_gliethttp->subsystem, filter, sizeof(filter))) {
			printf("[%3d:%-3d], '%s', '%s', '%s', '%s'\n", luther_gliethttp->major,luther_gliethttp->minor,luther_gliethttp->subsystem,
						luther_gliethttp->action,luther_gliethttp->path,luther_gliethttp->firmware);
		}
	}
#endif
}

int main(int argc, char* argv[])
{
    int device_fd = -1;
    char msg[UEVENT_MSG_LEN+2];
    int n;

/* shenpengru: 20150303 add */
#if 1
    int opt;
    int option_index = 0;
    char *optstring = ":h:";
    static struct option long_options[] = {
        { "filter", required_argument, NULL, 'f'},
        {   "help", no_argument,       NULL, 'h'},
        {"version", no_argument,       NULL, 'v'},
        {0, 0, 0, 0}
    };

    while( (opt = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1 ){
        switch( opt ){
            case 'f':
                //printf("The filter is %s\n", optarg);
                strcpy(filter, optarg);
                break;
            case 'h':
                help(HIDE_VERSION);
                exit(0);
                break;
            case 'v':
                help(SHOW_VERSION);
                exit(0);
            case '?':
            default:
                printf("\n\tERROR: Unknown options!\n");
                exit(1);
                break;
        }
    }
#endif
/* shenpengru: 20150303 add */
    
    device_fd = open_luther_gliethttp_socket();
    printf("device_fd = %d\n", device_fd);

    do {
        while((n = recv(device_fd, msg, UEVENT_MSG_LEN, 0)) > 0) {
            struct luther_gliethttp luther_gliethttp;

            if(n == UEVENT_MSG_LEN) /* overflow -- discard */
                continue;

            msg[n] = '/0';
            msg[n+1] = '/0';

            parse_event(msg, &luther_gliethttp);
        }
    } while(1);
}
