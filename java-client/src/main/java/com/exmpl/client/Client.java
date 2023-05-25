package com.exmpl.client;

import java.lang.Math;
import org.freedesktop.dbus.annotations.DBusInterfaceName;
import org.freedesktop.dbus.connections.impl.DBusConnection;
import org.freedesktop.dbus.connections.impl.DBusConnectionBuilder;
import org.freedesktop.dbus.exceptions.DBusException;
import org.freedesktop.dbus.interfaces.DBusInterface;
import org.freedesktop.dbus.interfaces.Properties;

@DBusInterfaceName("exmpl.factory")
interface Factory extends DBusInterface {
    public String Create(String worker_type);
    public void Destroy(String object_name);
    public void StopService();
}

@DBusInterfaceName("exmpl.worker1")
interface Worker1 extends DBusInterface { public void Calc(); }

@DBusInterfaceName("exmpl.worker2")
interface Worker2 extends DBusInterface { public void Calc(); }

@DBusInterfaceName("exmpl.worker3")
interface Worker3 extends DBusInterface { public void JoinEw(); }

@DBusInterfaceName("exmpl.worker4")
interface Worker4 extends DBusInterface { public void JoinEw(); }

class Client {
    public static void main(String[] args) throws DBusException {
        
        DBusConnection conn = (args.length > 0) && (args[0].equals("--system"))
            ? DBusConnectionBuilder.forSystemBus().build()
            : DBusConnectionBuilder.forSessionBus().build();

        String service = "com.exmpl.v0001";
        Factory fc = conn.getRemoteObject(service, "/factory", Factory.class);

        for (int i = 1; i < 5; i++) {
            String ifc = "exmpl.worker1";
            String obj_name = fc.Create("Worker1");
            Worker1 W1 = conn.getRemoteObject(service, obj_name, Worker1.class);
            Properties props = conn.getRemoteObject(service, obj_name, Properties.class);

            props.Set(ifc, "N1", 0.11 + i * 1000);
            props.Set(ifc, "N2", 0.11 + i * 100);
            props.Set(ifc, "N3", 0.11 + i * 10);
            props.Set(ifc, "N4", 0.11 + i);

            W1.Calc();
            System.out.printf("%s %.2f %.2f\n", obj_name,
                props.Get(ifc, "Summ"), props.Get(ifc, "Diff"));
            fc.Destroy(obj_name);
        }

        for (int i = 1; i < 5; i++)
        {
            String ifc = "exmpl.worker2";
            String obj_name = fc.Create("Worker2");
            Worker2 W2 = conn.getRemoteObject(service, obj_name, Worker2.class);
            Properties props = conn.getRemoteObject(service, obj_name, Properties.class);

            double[] a = {0.11, 0.11, 0.11, 0.11};
            for (int j = 0; j < 4; j++) a[j] += i * Math.pow(10, 3 - j);
            props.Set(ifc, "NS", a);

            W2.Calc();
            System.out.printf("%s %.2f %.2f\n", obj_name,
                props.Get(ifc, "Summ"), props.Get(ifc, "Diff"));
            fc.Destroy(obj_name);
        }

        String[] inp1 = {"eme", "eme", "abcde", "pqrst"};
        String[] inp2 = {"xp1", "xp2", "ABCDE", "PQRST_tail"};
        String[] inp3 = {"al",  "al",  "12345", "67890"};
        for (int i = 0; i < 4; i++)
        {
            String ifc = "exmpl.worker3";
            String obj_name = fc.Create("Worker3");
            Worker3 W3 = conn.getRemoteObject(service, obj_name, Worker3.class);
            Properties props = conn.getRemoteObject(service, obj_name, Properties.class);

            props.Set(ifc, "S1", inp1[i % 4]);
            props.Set(ifc, "S2", inp2[i % 4]);
            props.Set(ifc, "S3", inp3[i % 4]);

            W3.JoinEw();
            System.out.println(obj_name + " " + props.Get(ifc, "Joined"));
            fc.Destroy(obj_name);
        }

        String[][] inp = {{"eme", "xp1", "al"}, {"eme", "xp2", "al"},
                          {"abcde", "ABCDE", "12345"}, {"pqrst", "PQRST_tail", "67890"}};
        for (int i = 0; i < 4; i++)
        {
            String ifc = "exmpl.worker4";
            String obj_name = fc.Create("Worker4");
            Worker4 W4 = conn.getRemoteObject(service, obj_name, Worker4.class);
            Properties props = conn.getRemoteObject(service, obj_name, Properties.class);

            props.Set(ifc, "SS", inp[i % 4]);

            W4.JoinEw();
            System.out.println(obj_name + " " + props.Get(ifc, "Joined"));
            fc.Destroy(obj_name);
        }

        if ((args.length <= 1) || (!args[1].equals("--nostop"))) fc.StopService();
        conn.disconnect();
    }
}