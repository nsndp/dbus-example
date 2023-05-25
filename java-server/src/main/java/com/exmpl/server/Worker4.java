package com.exmpl.server;

import java.util.Arrays;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import org.freedesktop.dbus.types.Variant;

public class Worker4 implements Worker4Interface {

    private int id;
    public String[] ss;
    public String joined;

    public Worker4(int id) {
        this.id = id;
    }

    @Override
    public <A> A Get(String _interfaceName, String _propertyName) {
        if (_propertyName.equals("SS")) return (A)this.ss;
        else if (_propertyName.equals("Joined")) return (A)this.joined;
        else return null; //wrong property
    }

    @Override
    public <A> void Set(String _interfaceName, String _propertyName, A _value) {
        if (_propertyName.equals("SS")) {
            ArrayList<String> val = (ArrayList<String>)_value;
            this.ss = new String[val.size()];
            this.ss = val.toArray(this.ss);
        }
    }

    @Override
    public Map<String, Variant<?>> GetAll(String _interfaceName) {
        return null; //not implemented
    }

    @Override
    public String getObjectPath() {
        return "/Worker4/Instance" + this.id;
    }

    @Override
    public int JoinEw() {
        StringBuilder sb = new StringBuilder();
        //int mx = this.ss.stream().mapToInt(x -> x.length()).max().getAsInt();
        int mx = Arrays.stream(this.ss).mapToInt(x -> x.length()).max().getAsInt();
        for (int i = 0; i < mx; i++)
            for (String s : this.ss)
                if (i < s.length())
                    sb.append(s.charAt(i));
        this.joined = sb.toString();
        return 0;
    }
}