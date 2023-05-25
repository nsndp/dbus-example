package com.exmpl.server;

import java.lang.reflect.Constructor;
import java.util.Arrays;
import java.util.List;
import org.freedesktop.dbus.exceptions.DBusException;
import org.freedesktop.dbus.interfaces.DBusInterface;

public class Factory implements FactoryInterface {

    private List<String> names = Arrays.asList("Worker1", "Worker2", "Worker3", "Worker4");
    private int[] counters = {0, 0, 0, 0};

    @Override
    public String getObjectPath() {
        return "/factory";
    }

    @Override
    public String Create(String worker_type) throws Exception {
        if (!this.names.contains(worker_type)) throw new DBusException("Wrong worker type");
        int ind = Integer.parseInt(worker_type.substring(6));
        int c = ++this.counters[ind - 1];
        String nm = String.format("/%s/Instance%d", worker_type, c);
        Class<?> cls = Class.forName("com.exmpl.server.Worker" + ind);
        Constructor<?> constructor = cls.getConstructor(int.class);
        Object W = constructor.newInstance(c);
        //Server.CONN.exportObject(nm, (DBusInterface)W);
        Server.registerObj(nm, (DBusInterface)W);
        return nm;
    }

    @Override
    public void Destroy(String object_name) {
        Server.removeObj(object_name);
    }

    @Override
    public void StopService() throws DBusException {
        Server.finish();
    }
}