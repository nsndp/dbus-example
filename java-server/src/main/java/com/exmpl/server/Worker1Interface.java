package com.exmpl.server;

import org.freedesktop.dbus.annotations.DBusInterfaceName;
import org.freedesktop.dbus.annotations.DBusProperty;
import org.freedesktop.dbus.annotations.DBusProperty.Access;
import org.freedesktop.dbus.interfaces.DBusInterface;
import org.freedesktop.dbus.interfaces.Properties;

@DBusInterfaceName("exmpl.worker1")
@DBusProperty(name = "N1", type = Double.class, access = Access.READ_WRITE)
@DBusProperty(name = "N2", type = Double.class, access = Access.READ_WRITE)
@DBusProperty(name = "N3", type = Double.class, access = Access.READ_WRITE)
@DBusProperty(name = "N4", type = Double.class, access = Access.READ_WRITE)
@DBusProperty(name = "Summ", type = Double.class, access = Access.READ)
@DBusProperty(name = "Diff", type = Double.class, access = Access.READ)
public interface Worker1Interface extends DBusInterface, Properties {

    public int Calc();
}