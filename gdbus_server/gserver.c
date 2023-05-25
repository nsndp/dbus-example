/*
GDBus has multiple ways to implement server-side functions and export objects.

The official utility "gdbus-codegen" generates code that uses "g_dbus_interface_skeleton_export",
which is the most OOP way but also leads to a LOT of boilerplate, as well as the need to understand
GObjects (see example: https://github.com/NilanjanaLodh/test_GLib_APIs/tree/master/testDBUS/simple).

We use a simpler approach with "g_dbus_connection_register_object" and manually defined callbacks
like here https://github.com/bratsche/glib/blob/master/gio/tests/gdbus-example-server.c,
and then storing objects data in plain C structs. The rest is structured to allow for the
easiest possible expansion of worker types, so one can easily add Worker5,6,7,8 and so on
with just one line to gselector.c and an analogous pair of gworker<N>.c/h files.

Extra:
-various types example: https://github.com/joprietoe/gdbus/blob/master/gdbus-testserver.c
-docs for reference (gio): https://docs.gtk.org/gio/index.html
-docs for reference (glib): https://docs.gtk.org/glib/index.html
*/ 

#include <stdio.h>
#include <time.h>
#include "gselector.h"

static const gchar introspection_xml[] =
    "<node>"
    "    <interface name='exmpl.factory'>"
    "        <method name='StopService'/>"
    "        <method name='Create'>"
    "            <arg direction='in' name='worker_type' type='s'/>"
    "            <arg direction='out' name='object_name' type='s'/>"
    "        </method>"
    "        <method name='Destroy'>"
    "            <arg direction='in' name='object_name' type='s'/>"
    "        </method>"
    "     </interface>"
    "</node>";

static short had_connection = 0;
static GBusType bus_type = G_BUS_TYPE_SESSION;
static GMainLoop* loop = NULL;
static GHashTable* reg_ids = NULL;

static gchar* create_object(GDBusConnection* connection,
                            const gchar* worker_type,
                            WORKER_DATA wd)
{
    gchar* object_name = g_strdup_printf("/%s/Instance%lu", worker_type, *wd.global_counter + 1);
    GVariant* obj = wd.new_func();
    
    GDBusInterfaceVTable vtable = {wd.meth_handler, wd.prop_getter, wd.prop_setter};
    GDBusNodeInfo* introspec = g_dbus_node_info_new_for_xml(wd.introspection_xml, NULL);
    g_assert(introspec != NULL);
    guint reg_id = 0;
    GError* error = NULL;
    reg_id = g_dbus_connection_register_object(connection, object_name, introspec->interfaces[0],
                                               &vtable, obj, wd.clean_func, &error);
    g_assert_no_error(error);
    g_dbus_node_info_unref(introspec);
     
    GVariant* reg_container = g_variant_ref_sink(g_variant_new_uint32(reg_id));
    g_hash_table_insert(reg_ids, g_strdup(object_name), reg_container);
    (*wd.global_counter)++;
    return object_name;
}

static void handle_method_call(GDBusConnection* connection, const gchar* sender,
                               const gchar* object_path, const gchar* interface_name,
                               const gchar* method_name, GVariant* parameters,
                               GDBusMethodInvocation* invocation, gpointer user_data)
{
    if (g_strcmp0(method_name, "StopService") == 0)
    {
        g_dbus_method_invocation_return_value(invocation, NULL);
        g_main_loop_quit(loop);
        printf("STOP\n");
    }
    else if (g_strcmp0(method_name, "Create") == 0)
    {
        const gchar* worker_type;
        g_variant_get(parameters, "(&s)", &worker_type);
        gpointer wd = get_type_data(worker_type);
        if (wd == NULL)
        {
            dbus_error(invocation, "com.exmpl.create_error", "Unknown worker type for 'Create'");
            return;
        }
        gchar* object_name = create_object(connection, worker_type, *((WORKER_DATA*)wd));
        g_dbus_method_invocation_return_value(invocation, g_variant_new("(s)", object_name));
        g_free(object_name);
    }
    else if (g_strcmp0(method_name, "Destroy") == 0)
    {
        const gchar* object_name;
        g_variant_get(parameters, "(&s)", &object_name);
        GVariant* rid = (GVariant*)g_hash_table_lookup(reg_ids, object_name);
        if (rid == NULL)
        {
            dbus_error(invocation, "com.exmpl.destroy_error", "Unknown object name for 'Destroy'");
            return;
        }
        g_assert(rid != NULL);
        g_hash_table_remove(reg_ids, object_name);
        g_dbus_connection_unregister_object(connection, g_variant_get_uint32(rid));
        g_variant_unref(rid);
        g_dbus_method_invocation_return_value(invocation, NULL);
    }
    else
    { 
        dbus_error(invocation, "com.exmpl.method_error", "Unknown method name");
    }
}

static void on_bus_acquired(GDBusConnection* connection, const gchar* name, gpointer user_data)
{  
    GDBusNodeInfo* introspection_data = NULL;
    introspection_data = g_dbus_node_info_new_for_xml(introspection_xml, NULL);
    g_assert(introspection_data != NULL);

    GDBusInterfaceVTable vt = {handle_method_call, NULL, NULL};
    guint registration_id = 0;
    registration_id = g_dbus_connection_register_object(connection, "/factory",
                      introspection_data->interfaces[0], &vt, NULL, NULL, NULL);
    g_assert(registration_id > 0);
    
    g_dbus_node_info_unref(introspection_data);
}

static void on_name_acquired(GDBusConnection* connection, const gchar* name, gpointer user_data)
{
    if (bus_type == G_BUS_TYPE_SESSION) printf("START\n");
    else printf("START (SYSTEM BUS)\n");
    had_connection = 1;
}

static void on_name_lost(GDBusConnection* connection, const gchar* name, gpointer user_data)
{
    if (connection == NULL && bus_type == G_BUS_TYPE_SESSION)
    {
        char msg[500];
        if (had_connection == 1) strcpy(msg, "Lost connection to D-Bus session bus. ");
        else strcpy(msg, "Unable to connect to D-Bus session bus. ");
        strcat(msg, "Make sure that dbus-daemon is running and DBUS_SESSION_BUS_ADDRESS "
                    "environment variable is set (export DBUS_SESSION_BUS_ADDRESS "
                    "=$(dbus-daemon --session --fork --print-address)).");
        printf("%s\n", msg);
    }
    if (connection == NULL && bus_type == G_BUS_TYPE_SYSTEM)
    {
        char msg[500];
        if (had_connection == 1) strcpy(msg, "Lost connection to D-Bus system bus. ");
        else strcpy(msg, "Unable to connect to D-Bus system bus. ");
        strcat(msg, "Make sure that dbus service is running "
                    "(\"/etc/init.d/dbus start\" or \"dbus-daemon --system --fork\").");
        printf("%s\n", msg);
    }
    if (connection != NULL && bus_type == G_BUS_TYPE_SESSION)
    {
        printf("Unable to own the name on D-Bus session bus. "
                "Perhaps the process is already running.\n");
    }
    if (connection != NULL && bus_type == G_BUS_TYPE_SYSTEM)
    {
        printf("Unable to own the name on D-Bus system bus. "
               "If the process isn't already running, make sure that \"/etc/dbus-1/system.d\" "
               "has a file named \"com.exmpl.v0001.conf\" with necessary permissions inside "
               "(stackoverflow.com/a/65764137).\n");
    }
    exit(1);
}

int main(int argc, char** argv)
{
   	#if !GLIB_CHECK_VERSION(2,35,0)
	g_type_init();
	#endif
    if (argc > 1 && g_strcmp0(argv[1], "--system") == 0)
    {
        bus_type = G_BUS_TYPE_SYSTEM;
    }
    reg_ids = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    guint id = g_bus_own_name(bus_type, "com.exmpl.v0001", G_BUS_NAME_OWNER_FLAGS_NONE,
                              on_bus_acquired, on_name_acquired, on_name_lost, NULL, NULL);
    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
    g_main_loop_unref(loop);
    g_bus_unown_name(id);
    return 0;
}
