package com.exmpl.server;

import org.freedesktop.dbus.annotations.DBusInterfaceName;
import org.freedesktop.dbus.connections.impl.DBusConnection;
import org.freedesktop.dbus.connections.impl.DBusConnectionBuilder;
import org.freedesktop.dbus.exceptions.DBusException;
import org.freedesktop.dbus.interfaces.DBusInterface;
import org.freedesktop.dbus.interfaces.Properties;

class Server {

    private static String service_name = "com.exmpl.v0001";
    private static DBusConnection conn;
    private static boolean work = true;

    public static void registerObj(String nm, DBusInterface obj) throws DBusException {
        conn.exportObject(nm, obj);
    }

    public static void removeObj(String nm) {
        conn.unExportObject(nm);
    }

    public static void finish() throws DBusException {
        work = false;
    }

    public static void main(String[] args) throws DBusException, InterruptedException {

        boolean sysBus = (args.length > 0) && (args[0].equals("--system"));

        System.out.println("START" + (sysBus ? " (SYSTEM BUS)" : ""));

        conn = sysBus
            ? DBusConnectionBuilder.forSystemBus().build()
            : DBusConnectionBuilder.forSessionBus().build();

        conn.requestBusName(service_name);
        Factory fc = new Factory();
        conn.exportObject("/factory", fc);

        while (work) Thread.sleep(100L);
        
        System.out.println("STOP");
        conn.releaseBusName(service_name);
        conn.disconnect();
    }
}