#ifndef __MEMORY_DEBUG_H_
#define __MEMORY_DEBUG_H_

#define MEMDUMP_FILE "/data/local/tmp/malloc.dump"

#ifndef LOG_TAG
#define LOG_TAG "MemoryDebug"
#endif

//#define LOG_NDEBUG 0

#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <cutils/properties.h>
#include <utils/Log.h>

static FILE *fp = NULL;

extern "C" void get_malloc_leak_info(uint8_t** info,
                                     size_t* overallSize,
		                     size_t* infoSize,
                                     size_t* totalMemory,
                                     size_t* backtraceSize);
extern "C" void free_malloc_leak_info(uint8_t* info);

#define SIZE_FLAG_ZYGOTE_CHILD  (1<<31)
#define BACKTRACE_SIZE          32
/*
 * This is a qsort() callback.
 *
 * See dumpNativeHeap() for comments about the data format and sort order.
 */
static int compareHeapRecords(const void* vrec1, const void* vrec2)
{
    const size_t* rec1 = (const size_t*) vrec1;
    const size_t* rec2 = (const size_t*) vrec2;
    size_t size1 = *rec1;
    size_t size2 = *rec2;

    if (size1 < size2) {
        return 1;
    } else if (size1 > size2) {
        return -1;
    }

    intptr_t* bt1 = (intptr_t*)(rec1 + 2);
    intptr_t* bt2 = (intptr_t*)(rec2 + 2);
    for (size_t idx = 0; idx < BACKTRACE_SIZE; idx++) {
        intptr_t addr1 = bt1[idx];
        intptr_t addr2 = bt2[idx];
        if (addr1 == addr2) {
            if (addr1 == 0)
                break;
            continue;
        }
        if (addr1 < addr2) {
            return -1;
        } else if (addr1 > addr2) {
            return 1;
        }
    }

    return 0;
}

static void dumpNativeHeap(FILE* fp)
{
    uint8_t* info = NULL;
    size_t overallSize, infoSize, totalMemory, backtraceSize;

    if (fp == NULL) {
        ALOGE("Error create output dump\n");
        return;
    }

    get_malloc_leak_info(&info,
                         &overallSize,
                         &infoSize,
                         &totalMemory,
                         &backtraceSize);
    if (info == NULL) {
        #define USAGE "Native heap dump not available. To enable, run these" \
                      " commands (requires root):\n" \
                      "$ adb shell setprop libc.debug.malloc 1\n" \
                      "$ adb shell stop\n" \
                      "$ adb shell start\n"
        ALOGE(USAGE);
        fprintf(fp, USAGE);
        #undef USAGE
	fclose(fp); // fflush() will be called inside fclose()
        return;
    }

    if (infoSize != 0)
	ALOGE("Error, infoSize=%d\n",infoSize);
    if (overallSize % infoSize == 0)
	ALOGE("Error, overallSize=%d, infoSize=%d\n",overallSize, infoSize);

    //fprintf(fp, "Android Native Heap Dump v1.0\n\n");
    time_t t;
    struct tm tm;
    char buffer[256];

    memset(buffer, 256, 0);
    time(&t);
    localtime_r(&t, &tm);
    strftime(buffer, 256, "%F:%T", &tm);
    fprintf(fp, "\n%s\n",buffer);

    size_t recordCount = overallSize / infoSize;
    fprintf(fp, "Total memory: %zu\n", totalMemory);
    fprintf(fp, "Allocation records: %zd\n", recordCount);
    if (backtraceSize != BACKTRACE_SIZE) {
        fprintf(fp, "WARNING: mismatched backtrace sizes (%d vs. %d)\n",
            backtraceSize, BACKTRACE_SIZE);
    }

    /* re-sort the entries */
    qsort(info, recordCount, infoSize, compareHeapRecords);

    /* dump the entries to the file */
    const uint8_t* ptr = info;
    for (size_t idx = 0; idx < recordCount; idx++) {
        size_t size = *(size_t*) ptr;
        size_t allocations = *(size_t*) (ptr + sizeof(size_t));
        intptr_t* backtrace = (intptr_t*) (ptr + sizeof(size_t) * 2);

        fprintf(fp, "z %d  sz %8zu  num %4zu  bt",
                (size & SIZE_FLAG_ZYGOTE_CHILD) != 0,
                size & ~SIZE_FLAG_ZYGOTE_CHILD,
                allocations);
        for (size_t bt = 0; bt < backtraceSize; bt++) {
            if (backtrace[bt] == 0)
                break;
            else
                fprintf(fp, " %08x", backtrace[bt]);
        }

        fprintf(fp, "\n");
        ptr += infoSize;
    }

    free_malloc_leak_info(info);

    fprintf(fp, "MAPS\n");
    const char* maps = "/proc/self/maps";
    FILE* in = fopen(maps, "r");
    if (in == NULL) {
        fprintf(fp, "Could not open %s\n", maps);
        return;
    }
    char buf[BUFSIZ];
    while (size_t n = fread(buf, sizeof(char), BUFSIZ, in)) {
        fwrite(buf, sizeof(char), n, fp);
    }
    fclose(in);

    fprintf(fp, "END\n");

}

static void sigusr1_handler(int sig_number)
{
    if (fp == NULL)
        return;

    dumpNativeHeap(fp);
    fflush(fp);
}

static void exit_handler(void)
{
    if (fp == NULL) return;
    fclose(fp); fp = NULL;
}

static int enable_memorydebug(void)
{
    fp = fopen(MEMDUMP_FILE,"a+");
    if (fp == NULL) {
        ALOGE("%s\n", strerror(errno));
    }
    signal(SIGUSR1, sigusr1_handler);
    atexit(exit_handler);
    return 0;
}

#endif /* __MEMORY_DEBUG_H_ */
