#include "gworker2.h"

static unsigned long counter_W2 = 0;

static GVariant* new_W2()
{
    WORKER2* obj = (WORKER2*)malloc(sizeof(WORKER2));
    g_assert(obj != NULL);
    obj->ns = NULL;
    obj->len = 0;
    obj->summ = 0.0;
    obj->diff = 0.0;
    return (GVariant*)obj;
}

static void clean_W2(gpointer p)
{
    WORKER2* obj = (WORKER2*)p;
    free(obj->ns);
    free(obj);
}

static void on_method_call_W2(GDBusConnection* connection, const gchar* sender,
                              const gchar* object_path, const gchar* interface_name,
                              const gchar* method_name, GVariant* parameters,
                              GDBusMethodInvocation* invocation, gpointer user_data)
{
    WORKER2* obj = (WORKER2*)user_data;
    if (g_strcmp0(method_name, "Calc") != 0)
    {
        dbus_error(invocation, "com.exmpl.method_error", "Unknown method name");
        return;
    }
    if (obj->ns == NULL)
    {
        dbus_error(invocation, "com.exmpl.calc_error", "Array is empty");
        return;
    }
    obj->summ = obj->ns[0];
    obj->diff = obj->ns[0];
    int i;
    for (i = 1; i < obj->len; i++)
    {
        obj->summ += obj->ns[i];
        obj->diff -= obj->ns[i];
    }
    g_dbus_method_invocation_return_value(invocation, g_variant_new("(i)", 0));
}

static GVariant* on_get_property_W2(GDBusConnection* connection, const gchar* sender,
                                    const gchar* object_path, const gchar* interface_name,
                                    const gchar* property_name,
                                    GError** error, gpointer user_data)
{
    WORKER2* obj = (WORKER2*)user_data;
    if (g_strcmp0(property_name, "Summ") == 0)
    {
        return g_variant_new("d", obj->summ);
    }
    else if (g_strcmp0(property_name, "Diff") == 0)
    {
        return g_variant_new("d", obj->diff);
    }
    else if (g_strcmp0(property_name, "NS") == 0)
    {
        //this part isn't necessary for our test examples to work, since we'll
        //only be setting the input array, but included for completion's sake.
        //It mirrors function "build_array_dbl" from gclient.c
        int i;
        if (obj->ns == NULL) return NULL;
        GVariantBuilder* bd = g_variant_builder_new(G_VARIANT_TYPE("ad"));
        for (i = 0; i < obj->len; i++) g_variant_builder_add(bd, "d", obj->ns[i]);
        GVariant* val = g_variant_new("ad", bd);
        g_variant_builder_unref(bd);
        return val;
    }
    else
    {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Unknown property %s", property_name);
    }
    return NULL;
}
                                         
static gboolean on_set_property_W2(GDBusConnection* connection, const gchar* sender,
                                   const gchar* object_path, const gchar* interface_name,
                                   const gchar* property_name,
                                   GVariant* value, GError** error, gpointer user_data)
{
    WORKER2* obj = (WORKER2*)user_data;
    if (g_strcmp0(property_name, "NS") == 0)
    {
        g_free(obj->ns);
        GVariantIter* iter;
        g_variant_get(value, "ad", &iter);
        obj->len = g_variant_iter_n_children(iter);
        obj->ns = g_malloc(obj->len * sizeof(double));
        int i = 0; double d;
        while (g_variant_iter_loop(iter, "d", &d))
        {
            obj->ns[i] = d;
            i++;
        }
        g_variant_iter_free(iter);
    }
    else if (g_strcmp0(property_name, "Summ") == 0 || g_strcmp0(property_name, "Diff") == 0)
    {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Read-only property %s", property_name);
    }
    else
    {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Unknown property %s", property_name);
    }
    return *error == NULL;
}

WORKER_DATA data_W2 =
{
    .global_counter = &counter_W2,
    .new_func = new_W2,
    .clean_func = clean_W2,
    .meth_handler = on_method_call_W2,
    .prop_getter = on_get_property_W2,
    .prop_setter = on_set_property_W2,
    .introspection_xml = 
        "<node>"
        "    <interface name='exmpl.worker2'>"
        "        <property name='NS' type='ad' access='readwrite'/>"
        "        <property name='Summ' type='d' access='read'/>"
        "        <property name='Diff' type='d' access='read'/>"
        "        <method name='Calc'>"
        "            <arg direction='out' name='result' type='i'/>"
        "        </method>"
        "    </interface>"
        "</node>"
};
