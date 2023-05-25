#ifndef GCOMMON_H_INCLUDED
#define GCOMMON_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gio/gio.h>

//a shorter alias for a very long function name
#define dbus_error(x, y, z) (g_dbus_method_invocation_return_dbus_error(x, y, z))

typedef struct WorkerData
{
    unsigned long* global_counter;
    GVariant* (*new_func)();
    GDestroyNotify clean_func;
    GDBusInterfaceMethodCallFunc meth_handler;
    GDBusInterfaceGetPropertyFunc prop_getter;
    GDBusInterfaceSetPropertyFunc prop_setter;
    const gchar* introspection_xml;
}
WORKER_DATA;

#endif /* GCOMMON_H_INCLUDED */
