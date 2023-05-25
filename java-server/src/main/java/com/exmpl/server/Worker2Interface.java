package com.exmpl.server;

import java.util.List;
import org.freedesktop.dbus.annotations.DBusInterfaceName;
import org.freedesktop.dbus.annotations.DBusProperty;
import org.freedesktop.dbus.annotations.DBusProperty.Access;
import org.freedesktop.dbus.interfaces.DBusInterface;
import org.freedesktop.dbus.interfaces.Properties;

@DBusInterfaceName("exmpl.worker2")
@DBusProperty(name = "NS", type = List.class, access = Access.READ_WRITE)
@DBusProperty(name = "Summ", type = Double.class, access = Access.READ)
@DBusProperty(name = "Diff", type = Double.class, access = Access.READ)
public interface Worker2Interface extends DBusInterface, Properties {

    public int Calc();
}