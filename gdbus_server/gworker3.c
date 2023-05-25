#include "gworker3.h"

static unsigned long counter_W3 = 0;

static GVariant* new_W3()
{
    WORKER3* obj = (WORKER3*)g_malloc(sizeof(WORKER3));
    obj->s1 = NULL;
    obj->s2 = NULL;
    obj->s3 = NULL;
    obj->joined = NULL;
    return (GVariant*)obj;
}

static void clean_W3(gpointer p)
{
    WORKER3* obj = (WORKER3*)p;
    free(obj->s1);
    free(obj->s2);
    free(obj->s3);
    free(obj->joined);
    free(obj);
}

static void on_method_call_W3(GDBusConnection* connection, const gchar* sender,
                              const gchar* object_path, const gchar* interface_name,
                              const gchar* method_name, GVariant* parameters,
                              GDBusMethodInvocation* invocation, gpointer user_data)
{
    WORKER3* obj = (WORKER3*)user_data;
    if (g_strcmp0(method_name, "JoinEw") != 0)
    {
        dbus_error(invocation, "com.exmpl.method_error", "Unknown method name");
        return;
    }
    if (obj->s1 == NULL || obj->s2 == NULL || obj->s3 == NULL)
    {
        dbus_error(invocation, "com.exmpl.join_error", "S1, S2 or S3 are undefined");
        return;
    }
    g_free(obj->joined);
    size_t l1 = strlen(obj->s1);
    size_t l2 = strlen(obj->s2);
    size_t l3 = strlen(obj->s3);
    obj->joined = (char*)g_malloc((l1+l2+l3+1)*sizeof(char));
    int i = 0;
    int k = 0;
    while (i < l1 || i < l2 || i < l3)
    {
        if (i < l1) { obj->joined[k] = obj->s1[i]; k++; }
        if (i < l2) { obj->joined[k] = obj->s2[i]; k++; }
        if (i < l3) { obj->joined[k] = obj->s3[i]; k++; }
        i++;
    }
    obj->joined[k] = '\0';
    g_dbus_method_invocation_return_value(invocation, g_variant_new("(i)", 0));
}

static GVariant* on_get_property_W3(GDBusConnection* connection, const gchar* sender,
                                    const gchar* object_path, const gchar* interface_name,
                                    const gchar* property_name,
                                    GError** error, gpointer user_data)
{
    WORKER3* obj = (WORKER3*)user_data;
    if (g_strcmp0(property_name, "S1") == 0)
    {
        if (obj->s1 == NULL) return NULL;
        return g_variant_new("s", g_strdup(obj->s1));
    }
    if (g_strcmp0(property_name, "S2") == 0)
    {
        if (obj->s2 == NULL) return NULL;
        return g_variant_new("s", g_strdup(obj->s2));
    }
    if (g_strcmp0(property_name, "S3") == 0)
    {
        if (obj->s3 == NULL) return NULL;
        return g_variant_new("s", g_strdup(obj->s3));
    }
    if (g_strcmp0(property_name, "Joined") == 0)
    {
        if (obj->joined == NULL) return NULL;
        return g_variant_new("s", g_strdup(obj->joined));
    }
    g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Unknown property %s", property_name);
    return NULL;
}
                                         
static gboolean on_set_property_W3(GDBusConnection* connection, const gchar* sender,
                                   const gchar* object_path, const gchar* interface_name,
                                   const gchar* property_name,
                                   GVariant* value, GError** error, gpointer user_data)
{
    WORKER3* obj = (WORKER3*)user_data;
    if (g_strcmp0(property_name, "S1") == 0)
    {
        g_free(obj->s1);
        obj->s1 = g_variant_dup_string(value, NULL);
    }
    else if (g_strcmp0(property_name, "S2") == 0)
    {
        g_free(obj->s2);
        obj->s2 = g_variant_dup_string(value, NULL);
    }
    else if (g_strcmp0(property_name, "S3") == 0)
    {
        g_free(obj->s3);
        obj->s3 = g_variant_dup_string(value, NULL);
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

WORKER_DATA data_W3 =
{
    .global_counter = &counter_W3,
    .new_func = new_W3,
    .clean_func = clean_W3,
    .meth_handler = on_method_call_W3,
    .prop_getter = on_get_property_W3,
    .prop_setter = on_set_property_W3,
    .introspection_xml = 
        "<node>"
        "    <interface name='exmpl.worker3'>"
        "        <property name='S1' type='s' access='readwrite'/>"
        "        <property name='S2' type='s' access='readwrite'/>"
        "        <property name='S3' type='s' access='readwrite'/>"
        "        <property name='Joined' type='s' access='read'/>"
        "        <method name='JoinEw'>"
        "            <arg direction='out' name='result' type='i'/>"
        "        </method>"
        "    </interface>"
        "</node>"
};