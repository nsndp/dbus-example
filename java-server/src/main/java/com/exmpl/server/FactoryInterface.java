package com.exmpl.server;

import org.freedesktop.dbus.annotations.DBusInterfaceName;
import org.freedesktop.dbus.interfaces.DBusInterface;
import org.freedesktop.dbus.exceptions.DBusException;

@DBusInterfaceName("exmpl.factory")
public interface FactoryInterface extends DBusInterface {

    public String Create(String worker_type) throws Exception;

    public void Destroy(String object_name);
    
    public void StopService() throws DBusException;
}