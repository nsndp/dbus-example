package com.exmpl.server;

import java.util.List;
import org.freedesktop.dbus.annotations.DBusInterfaceName;
import org.freedesktop.dbus.annotations.DBusProperty;
import org.freedesktop.dbus.annotations.DBusProperty.Access;
import org.freedesktop.dbus.interfaces.DBusInterface;
import org.freedesktop.dbus.interfaces.Properties;

@DBusInterfaceName("exmpl.worker4")
@DBusProperty(name = "SS", type = List.class, access = Access.READ_WRITE)
@DBusProperty(name = "Joined", type = String.class, access = Access.READ)
public interface Worker4Interface extends DBusInterface, Properties {

    public int JoinEw();
}