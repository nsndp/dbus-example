package com.exmpl.server;

import java.util.Arrays;
import java.util.stream.DoubleStream;
import java.util.Map;
import org.freedesktop.dbus.types.Variant;

public class Worker2 implements Worker1Interface {

    private int id;
    public double[] ns;
    public Double summ, diff;

    public Worker2(int id) {
        this.id = id;
        this.ns = null;
        this.summ = this.diff = 0.0;
    }

    @Override
    public <A> A Get(String _interfaceName, String _propertyName) {
        if (_propertyName.equals("NS")) return (A)this.ns;
        else if (_propertyName.equals("Summ")) return (A)this.summ;
        else if (_propertyName.equals("Diff")) return (A)this.diff;
        else return null; //wrong property
    }

    @Override
    public <A> void Set(String _interfaceName, String _propertyName, A _value) {
        if (_propertyName.equals("NS")) this.ns = (double[])_value;
    }

    @Override
    public Map<String, Variant<?>> GetAll(String _interfaceName) {
        return null; //not implemented
    }

    @Override
    public String getObjectPath() {
        return "/Worker2/Instance" + this.id;
    }

    @Override
    public int Calc() {
        this.summ = Arrays.stream(this.ns).sum();
        this.diff = this.ns[0] - Arrays.stream(this.ns, 1, this.ns.length).sum();
        return 0;
    }
}