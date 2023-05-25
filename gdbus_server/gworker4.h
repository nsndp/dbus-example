#include "gcommon.h"

extern WORKER_DATA data_W4;

typedef struct Worker4
{
    char** strings;
    unsigned int strings_count;
    char* joined;
}
WORKER4;