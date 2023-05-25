#include "gselector.h"
#include "gworker1.h"
#include "gworker2.h"
#include "gworker3.h"
#include "gworker4.h"

gpointer get_type_data(const gchar* worker_type)
{
    if (g_strcmp0(worker_type, "Worker1") == 0) return (gpointer)&data_W1;
    if (g_strcmp0(worker_type, "Worker2") == 0) return (gpointer)&data_W2;
    if (g_strcmp0(worker_type, "Worker3") == 0) return (gpointer)&data_W3;
    if (g_strcmp0(worker_type, "Worker4") == 0) return (gpointer)&data_W4;
    return NULL;
}