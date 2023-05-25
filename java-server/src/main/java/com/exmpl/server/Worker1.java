package com.exmpl.server;

import java.util.Map;
import org.freedesktop.dbus.types.Variant;

public class Worker1 implements Worker1Interface {

    private int id;
    public Double n1, n2, n3, n4, summ, diff;

    public Worker1(int id) {
        this.id = id;
        this.n1 = this.n2 = this.n3 = this.n4 = this.summ = this.diff = 0.0;
    }

    @Override
    public <A> A Get(String _interfaceName, String _propertyName) {
        if (_propertyName.equals("N1")) return (A)this.n1;
        else if (_propertyName.equals("N2")) return (A)this.n2;
        else if (_propertyName.equals("N3")) return (A)this.n3;
        else if (_propertyName.equals("N4")) return (A)this.n4;
        else if (_propertyName.equals("Summ")) return (A)this.summ;
        else if (_propertyName.equals("Diff")) return (A)this.diff;
        else return null; //wrong property
    }

    @Override
    public <A> void Set(String _interfaceName, String _propertyName, A _value) {
        if (_propertyName.equals("N1")) this.n1 = (Double)_value;
        else if (_propertyName.equals("N2")) this.n2 = (Double)_value;
        else if (_propertyName.equals("N3")) this.n3 = (Double)_value;
        else if (_propertyName.equals("N4")) this.n4 = (Double)_value;
    }

    @Override
    public Map<String, Variant<?>> GetAll(String _interfaceName) {
        return null; //not implemented
    }

    @Override
    public String getObjectPath() {
        return "/Worker1/Instance" + this.id;
    }

    @Override
    public int Calc() {
        this.summ = this.n1 + this.n2 + this.n3 + this.n4;
        this.diff = this.n1 - this.n2 - this.n3 - this.n4;
        return 0;
    }
}