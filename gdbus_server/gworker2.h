#include "gcommon.h"

extern WORKER_DATA data_W2;

typedef struct Worker2
{
    double* ns;
    unsigned int len;
    double summ;
    double diff;
}
WORKER2;