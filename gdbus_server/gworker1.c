#include "gworker1.h"

static unsigned long counter_W1 = 0;

static GVariant* new_W1()
{
    WORKER1* obj = (WORKER1*)malloc(sizeof(WORKER1));
    g_assert(obj != NULL);
    obj->n1 = 0.0;
    obj->n2 = 0.0;
    obj->n3 = 0.0;
    obj->n4 = 0.0;
    obj->summ = 0.0;
    obj->diff = 0.0;
    return (GVariant*)obj;
}

static void clean_W1(gpointer p)
{
    WORKER1* obj = (WORKER1*)p;
    free(obj);
}

static void on_method_call_W1(GDBusConnection* connection, const gchar* sender,
                              const gchar* object_path, const gchar* interface_name,
                              const gchar* method_name, GVariant* parameters,
                              GDBusMethodInvocation* invocation, gpointer user_data)
{
    WORKER1* obj = (WORKER1*)user_data;
    if (g_strcmp0(method_name, "Calc") == 0)
    {
        obj->summ = obj->n1 + obj->n2 + obj->n3 + obj->n4;
        obj->diff = obj->n1 - obj->n2 - obj->n3 - obj->n4;
        g_dbus_method_invocation_return_value(invocation, g_variant_new("(i)", 0));
    }
    else dbus_error(invocation, "com.exmpl.method_error", "Unknown method name");
}

static GVariant* on_get_property_W1(GDBusConnection* connection, const gchar* sender,
                                    const gchar* object_path, const gchar* interface_name,
                                    const gchar* property_name,
                                    GError** error, gpointer user_data)
{
    WORKER1* obj = (WORKER1*)user_data;
    if (g_strcmp0(property_name, "N1") == 0) return g_variant_new("d", obj->n1);
    else if (g_strcmp0(property_name, "N2") == 0) return g_variant_new("d", obj->n2);
    else if (g_strcmp0(property_name, "N3") == 0) return g_variant_new("d", obj->n3);
    else if (g_strcmp0(property_name, "N4") == 0) return g_variant_new("d", obj->n4);
    else if (g_strcmp0(property_name, "Summ") == 0) return g_variant_new("d", obj->summ);
    else if (g_strcmp0(property_name, "Diff") == 0) return g_variant_new("d", obj->diff);
    else g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Unknown property %s", property_name);
    return NULL;
}
                                         
static gboolean on_set_property_W1(GDBusConnection* connection, const gchar* sender,
                                   const gchar* object_path, const gchar* interface_name,
                                   const gchar* property_name,
                                   GVariant* value, GError** error, gpointer user_data)
{
    WORKER1* obj = (WORKER1*)user_data;
    if (g_strcmp0(property_name, "N1") == 0) obj->n1 = g_variant_get_double(value);
    else if (g_strcmp0(property_name, "N2") == 0) obj->n2 = g_variant_get_double(value);
    else if (g_strcmp0(property_name, "N3") == 0) obj->n3 = g_variant_get_double(value);
    else if (g_strcmp0(property_name, "N4") == 0) obj->n4 = g_variant_get_double(value);
    else if (g_strcmp0(property_name, "Summ") == 0 || g_strcmp0(property_name, "Diff") == 0)
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Read-only property %s", property_name);
    else g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Unknown property %s", property_name);
    return *error == NULL;
}

WORKER_DATA data_W1 =
{
    .global_counter = &counter_W1,
    .new_func = new_W1,
    .clean_func = clean_W1,
    .meth_handler = on_method_call_W1,
    .prop_getter = on_get_property_W1,
    .prop_setter = on_set_property_W1,
    .introspection_xml = 
        "<node>"
        "    <interface name='exmpl.worker1'>"
        "        <property name='N1' type='d' access='readwrite'/>"
        "        <property name='N2' type='d' access='readwrite'/>"
        "        <property name='N3' type='d' access='readwrite'/>"
        "        <property name='N4' type='d' access='readwrite'/>"
        "        <property name='Summ' type='d' access='read'/>"
        "        <property name='Diff' type='d' access='read'/>"
        "        <method name='Calc'>"
        "            <arg direction='out' name='result' type='i'/>"
        "        </method>"
        "    </interface>"
        "</node>"
};