#include <gio/gio.h>
#include <stdio.h>

static GDBusProxy* get_proxy(const gchar* service_name, const gchar* object_name, const gchar* interface_name);
static gchar* create_object(GDBusProxy* factory, const gchar* worker_type);
static void destroy_object(GDBusProxy* factory, gchar* object_name);
static void call_method(GDBusProxy* proxy, const gchar* method_name);
static void set_property(GDBusProxy* proxy, const gchar* property_name, GVariant* value, const gchar* interface_name);
static GVariant* get_property(GDBusProxy* proxy, const gchar* property_name, const gchar* interface_name);
static double unpack_double(GVariant* val);
static GVariant* build_array_dbl(double a[], int len);
static GVariant* build_array_str(char* s[], int len);

static GBusType bus_type = G_BUS_TYPE_SESSION;

int main(int argc, char** argv)
{
    #if !GLIB_CHECK_VERSION(2,35,0)
	g_type_init();
	#endif

    if (argc > 1 && g_strcmp0(argv[1], "--system") == 0) bus_type = G_BUS_TYPE_SYSTEM;
    gchar* worker_type;
    gchar* interface_name;
    const gchar* service = "com.exmpl.v0001";
    GDBusProxy* factory = get_proxy(service, "/factory", "exmpl.factory");
    
    int i;   
    worker_type = "Worker1";
    interface_name = "exmpl.worker1";
    for (i = 1; i < 5; i++)
    {
        gchar* obj = create_object(factory, worker_type);
        GDBusProxy* M = get_proxy(service, obj, interface_name);
        GDBusProxy* MP = get_proxy(service, obj, "org.freedesktop.DBus.Properties");
        
        set_property(MP, "N1", g_variant_new("d", 0.11 + (double)i * 1000), interface_name);
        set_property(MP, "N2", g_variant_new("d", 0.11 + (double)i * 100), interface_name);
        set_property(MP, "N3", g_variant_new("d", 0.11 + (double)i * 10), interface_name);
        set_property(MP, "N4", g_variant_new("d", 0.11 + (double)i), interface_name);
        call_method(M, "Calc");
        double res1 = unpack_double(get_property(MP, "Summ", interface_name));
        double res2 = unpack_double(get_property(MP, "Diff", interface_name));
        printf("%s %.2f %.2f\n", obj, res1, res2);
        
        destroy_object(factory, obj);
        g_object_unref(M);
        g_object_unref(MP);
    }

    worker_type = "Worker2";
    interface_name = "exmpl.worker2";
    for (i = 1; i < 5; i++)
    {
        gchar* obj = create_object(factory, worker_type);
        GDBusProxy* M = get_proxy(service, obj, interface_name);
        GDBusProxy* MP = get_proxy(service, obj, "org.freedesktop.DBus.Properties");
        
        double a[4];
        a[0] = 0.11 + (double)i * 1000;
        a[1] = 0.11 + (double)i * 100;
        a[2] = 0.11 + (double)i * 10;
        a[3] = 0.11 + (double)i;
        set_property(MP, "NS", build_array_dbl(a, 4), interface_name);
        call_method(M, "Calc");
        double res1 = unpack_double(get_property(MP, "Summ", interface_name));
        double res2 = unpack_double(get_property(MP, "Diff", interface_name));
        printf("%s %.2f %.2f\n", obj, res1, res2);
        
        destroy_object(factory, obj);
        g_object_unref(M);
        g_object_unref(MP);
    }

    worker_type = "Worker3";
    interface_name = "exmpl.worker3";
    char* inp1[4] = { "eme", "eme", "abcde", "pqrst" };
    char* inp2[4] = { "xp1", "xp2", "ABCDE", "PQRST_tail" };
    char* inp3[4] = { "al",  "al",  "12345", "67890" };
    for (i = 0; i < 4; i++)
    {
        gchar* obj = create_object(factory, worker_type);
        GDBusProxy* M = get_proxy(service, obj, interface_name);
        GDBusProxy* MP = get_proxy(service, obj, "org.freedesktop.DBus.Properties");
        
        set_property(MP, "S1", g_variant_new("s", inp1[i % 4]), interface_name);
        set_property(MP, "S2", g_variant_new("s", inp2[i % 4]), interface_name);
        set_property(MP, "S3", g_variant_new("s", inp3[i % 4]), interface_name);
        call_method(M, "JoinEw");
        GVariant* res = get_property(MP, "Joined", interface_name);
        printf("%s %s\n", obj, g_variant_get_string(res, NULL));
        g_variant_unref(res);

        destroy_object(factory, obj);
        g_object_unref(M);
        g_object_unref(MP);
    }

    worker_type = "Worker4";
    interface_name = "exmpl.worker4";
    char* a[][3] = { {"eme", "xp1", "al"}, {"eme", "xp2", "al"},
                     {"abcde", "ABCDE", "12345"}, {"pqrst", "PQRST_tail", "67890"} };
    for (i = 0; i < 4; i++)
    {
        gchar* obj = create_object(factory, worker_type);
        GDBusProxy* M = get_proxy(service, obj, interface_name);
        GDBusProxy* MP = get_proxy(service, obj, "org.freedesktop.DBus.Properties");
        
        set_property(MP, "SS", build_array_str(a[i % 4], 3), interface_name);
        call_method(M, "JoinEw");
        GVariant* res = get_property(MP, "Joined", interface_name);
        printf("%s %s\n", obj, g_variant_get_string(res, NULL));
        g_variant_unref(res);

        destroy_object(factory, obj);
        g_object_unref(M);
        g_object_unref(MP);
    }

    if (argc <= 2 || g_strcmp0(argv[2], "--nostop") != 0) call_method(factory, "StopService");
    g_object_unref(factory);
}

static GDBusProxy* get_proxy(const gchar* service_name, const gchar* object_name, const gchar* interface_name)
{
    GError* error = NULL;
    GDBusProxy* proxy = NULL;
    proxy = g_dbus_proxy_new_for_bus_sync(bus_type, G_DBUS_PROXY_FLAGS_NONE, NULL,
                                          service_name, object_name, interface_name, NULL, &error);
    g_assert_no_error(error);
    return proxy;
}

static gchar* create_object(GDBusProxy* factory, const gchar* worker_type)
{
    GError* error = NULL;
    GVariant* ret = NULL;
    ret = g_dbus_proxy_call_sync(factory, "Create", g_variant_new("(s)", worker_type),
                                 G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
    g_assert_no_error(error);
    gchar* object_name = NULL;
    g_variant_get(ret, "(s)", &object_name);
    g_variant_unref(ret);
    return object_name;
}

static void destroy_object(GDBusProxy* factory, gchar* object_name)
{
    GError* error = NULL;
    g_dbus_proxy_call_sync(factory, "Destroy", g_variant_new("(s)", object_name),
                           G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
    g_assert_no_error(error);
    g_free(object_name);
}

static void call_method(GDBusProxy* proxy, const gchar* method_name)
{
    GError* error = NULL;
    g_dbus_proxy_call_sync(proxy, method_name, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
    g_assert_no_error(error);
}

static void set_property(GDBusProxy* proxy, const gchar* property_name, GVariant* value, const gchar* interface_name)
{
    GError* error = NULL;
    GVariant* ret = NULL;
    ret = g_dbus_proxy_call_sync(proxy, "Set",
                                 g_variant_new("(ssv)", interface_name, property_name, value),
                                 G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
    g_assert_no_error(error);
    g_variant_unref(ret);
}

static GVariant* get_property(GDBusProxy* proxy, const gchar* property_name, const gchar* interface_name)
{
    GError* error = NULL;
    GVariant* ret = NULL;
    GVariant* val = NULL;
    ret = g_dbus_proxy_call_sync(proxy, "Get",
                                 g_variant_new("(ss)", interface_name, property_name),
                                 G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
    g_assert_no_error(error);
    g_variant_get(ret, "(v)", &val);
    g_variant_unref(ret);
    return val;
}

static double unpack_double(GVariant* val)
{
    double d = g_variant_get_double(val);
    g_variant_unref(val);
    return d;
}

static GVariant* build_array_dbl(double a[], int len)
{
    // https://docs.gtk.org/glib/gvariant-format-strings.html#arrays
    int i;
    GVariantBuilder *bd;
    GVariant *val;
    bd = g_variant_builder_new(G_VARIANT_TYPE("ad"));
    for (i = 0; i < len; i++) g_variant_builder_add(bd, "d", a[i]);
    val = g_variant_new("ad", bd);
    g_variant_builder_unref(bd);
    return val;
}

static GVariant* build_array_str(char* s[], int len)
{
    int i;
    GVariantBuilder *bd;
    GVariant *val;
    bd = g_variant_builder_new(G_VARIANT_TYPE("as"));
    for (i = 0; i < len; i++) g_variant_builder_add(bd, "s", s[i]);
    val = g_variant_new("as", bd);
    g_variant_builder_unref(bd);
    return val;
}
