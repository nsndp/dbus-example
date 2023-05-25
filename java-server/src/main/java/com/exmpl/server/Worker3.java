package com.exmpl.server;

import java.lang.Math;
import java.lang.StringBuilder;
import java.util.Map;
import org.freedesktop.dbus.types.Variant;

public class Worker3 implements Worker3Interface {

    private int id;
    public String s1, s2, s3, joined;

    public Worker3(int id) {
        this.id = id;
    }

    @Override
    public <A> A Get(String _interfaceName, String _propertyName) {
        if (_propertyName.equals("S1")) return (A)this.s1;
        else if (_propertyName.equals("S2")) return (A)this.s2;
        else if (_propertyName.equals("S3")) return (A)this.s3;
        else if (_propertyName.equals("Joined")) return (A)this.joined;
        else return null; //wrong property
    }

    @Override
    public <A> void Set(String _interfaceName, String _propertyName, A _value) {
        if (_propertyName.equals("S1")) this.s1 = (String)_value;
        else if (_propertyName.equals("S2")) this.s2 = (String)_value;
        else if (_propertyName.equals("S3")) this.s3 = (String)_value;
    }

    @Override
    public Map<String, Variant<?>> GetAll(String _interfaceName) {
        return null; //not implemented
    }

    @Override
    public String getObjectPath() {
        return "/Worker3/Instance" + this.id;
    }

    @Override
    public int JoinEw() {
        StringBuilder sb = new StringBuilder();
        int l1 = s1.length();
        int l2 = s2.length();
        int l3 = s3.length();
        int mx = Math.max(Math.max(l1, l2), l3);
        for (int i = 0; i < mx; i++) {
            if (i < l1) sb.append(s1.charAt(i));
            if (i < l2) sb.append(s2.charAt(i));
            if (i < l3) sb.append(s3.charAt(i));
        }
        this.joined = sb.toString();
        return 0;
    }
}