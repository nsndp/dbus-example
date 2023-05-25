#include "gworker4.h"

static unsigned long counter_W4 = 0;

static GVariant* new_W4()
{
    WORKER4* obj = (WORKER4*)g_malloc(sizeof(WORKER4));
    obj->strings = NULL;
    obj->strings_count = 0;
    obj->joined = NULL;
    return (GVariant*)obj;
}

static void clean_W4(gpointer p)
{
    int i;
    WORKER4* obj = (WORKER4*)p;
    for (i = 0; i < obj->strings_count; i++) free(obj->strings[i]);
    free(obj->strings);
    free(obj->joined);
    free(obj);
}

static void on_method_call_W4(GDBusConnection* connection, const gchar* sender,
                              const gchar* object_path, const gchar* interface_name,
                              const gchar* method_name, GVariant* parameters,
                              GDBusMethodInvocation* invocation, gpointer user_data)
{
    WORKER4* obj = (WORKER4*)user_data;
    if (g_strcmp0(method_name, "JoinEw") != 0)
    {
        dbus_error(invocation, "com.exmpl.method_error", "Unknown method name");
        return;
    }
    if (obj->strings == NULL)
    {
        dbus_error(invocation, "com.exmpl.join_error", "SS is undefined");
        return;
    }
    g_free(obj->joined);
    int n = obj->strings_count;
    int maxln = 0, total = 0, i = 0, j = 0, k = 0;
    int* lens = (int*)g_malloc((n)*sizeof(int));
    for (i = 0; i < n; i++)
    {
        size_t l = strlen(obj->strings[i]);
        total += l;
        if (maxln < l) maxln = l;
        lens[i] = l;
    }
    obj->joined = (char*)g_malloc((total+1)*sizeof(char));
    while (j < maxln)
    {
        for (i = 0; i < n; i++) if (j < lens[i]) { obj->joined[k] = obj->strings[i][j]; k++; }
        j++;
    }
    obj->joined[k] = '\0';
    g_free(lens);
    g_dbus_method_invocation_return_value(invocation, g_variant_new("(i)", 0));
}

static GVariant* on_get_property_W4(GDBusConnection* connection, const gchar* sender,
                                    const gchar* object_path, const gchar* interface_name,
                                    const gchar* property_name,
                                    GError** error, gpointer user_data)
{
    WORKER4* obj = (WORKER4*)user_data;
    if (g_strcmp0(property_name, "Joined") == 0)
    {
        if (obj->joined == NULL) return NULL;
        return g_variant_new("s", g_strdup(obj->joined));
    }
    if (g_strcmp0(property_name, "SS") == 0)
    {
        //this part isn't necessary for our test examples to work, since we'll
        //only be setting the input array, but included for completion's sake.
        //It mirrors function "build_array_str" from gclient.c
        int i;
        if (obj->strings == NULL) return NULL;
        GVariantBuilder* bd = g_variant_builder_new(G_VARIANT_TYPE("as"));
        for (i = 0; i < obj->strings_count; i++) g_variant_builder_add(bd, "s", obj->strings[i]);
        GVariant* val = g_variant_new("as", bd);
        g_variant_builder_unref(bd);
        return val;
    }
    g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Unknown property %s", property_name);
    return NULL;
}
                                         
static gboolean on_set_property_W4(GDBusConnection* connection, const gchar* sender,
                                   const gchar* object_path, const gchar* interface_name,
                                   const gchar* property_name,
                                   GVariant* value, GError** error, gpointer user_data)
{
    WORKER4* obj = (WORKER4*)user_data;
    if (g_strcmp0(property_name, "SS") == 0)
    {
        int i;
        for (i = 0; i < obj->strings_count; i++) g_free(obj->strings[i]);
        g_free(obj->strings);
        //following example from https://docs.gtk.org/glib/gvariant-format-strings.html#examples_3
        GVariantIter* iter;
        g_variant_get(value, "as", &iter);
        obj->strings_count = g_variant_iter_n_children(iter);
        obj->strings = g_malloc(obj->strings_count * sizeof(char*));
        i = 0; gchar* s;
        while (g_variant_iter_loop(iter, "s", &s))
        {
            obj->strings[i] = g_strdup(s);
            i++;
        }
        g_variant_iter_free(iter);
    }
    else if (g_strcmp0(property_name, "Joined") == 0)
    {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Read-only property %s", property_name);
    }
    else
    {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Unknown property %s", property_name);
    }
    return *error == NULL;
}

WORKER_DATA data_W4 =
{
    .global_counter = &counter_W4,
    .new_func = new_W4,
    .clean_func = clean_W4,
    .meth_handler = on_method_call_W4,
    .prop_getter = on_get_property_W4,
    .prop_setter = on_set_property_W4,
    .introspection_xml = 
        "<node>"
        "    <interface name='exmpl.worker4'>"
        "        <property name='SS' type='as' access='readwrite'/>"
        "        <property name='Joined' type='s' access='read'/>"
        "        <method name='JoinEw'>"
        "            <arg direction='out' name='result' type='i'/>"
        "        </method>"
        "    </interface>"
        "</node>"
};
