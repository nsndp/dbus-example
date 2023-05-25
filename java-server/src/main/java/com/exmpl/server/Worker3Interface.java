package com.exmpl.server;

import org.freedesktop.dbus.annotations.DBusInterfaceName;
import org.freedesktop.dbus.annotations.DBusProperty;
import org.freedesktop.dbus.annotations.DBusProperty.Access;
import org.freedesktop.dbus.interfaces.DBusInterface;
import org.freedesktop.dbus.interfaces.Properties;

@DBusInterfaceName("exmpl.worker3")
@DBusProperty(name = "S1", type = String.class, access = Access.READ_WRITE)
@DBusProperty(name = "S2", type = String.class, access = Access.READ_WRITE)
@DBusProperty(name = "S3", type = String.class, access = Access.READ_WRITE)
@DBusProperty(name = "Joined", type = String.class, access = Access.READ)
public interface Worker3Interface extends DBusInterface, Properties {

    public int JoinEw();
}