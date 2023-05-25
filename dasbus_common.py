XML0 = """
<node>
    <interface name="exmpl.factory">
        <method name="StopService" />
        <method name="Create">
            <arg direction="in" name="worker_type" type="s" />
            <arg direction="out" name="object_name" type="s" />
        </method>
        <method name="Destroy">
            <arg direction="in" name="object_name" type="s" />
        </method>
    </interface>
</node>
"""

XML1 = """
<node>
    <interface name="exmpl.worker1">
        <property name="N1" type="d" access="readwrite" />
        <property name="N2" type="d" access="readwrite" />
        <property name="N3" type="d" access="readwrite" />
        <property name="N4" type="d" access="readwrite" />
        <property name="Summ" type="d" access="read" />
        <property name="Diff" type="d" access="read" />
        <method name="Calc">
            <arg direction="out" name="result" type="i" />
        </method>
    </interface>
</node>
"""

XML2 = """
<node>
    <interface name="exmpl.worker2">
        <property name="NS" type="ad" access="readwrite" />
        <property name="Summ" type="d" access="read" />
        <property name="Diff" type="d" access="read" />
        <method name="Calc">
            <arg direction="out" name="result" type="i" />
        </method>
    </interface>
</node>
"""

XML3 = """
<node>
    <interface name="exmpl.worker3">
        <property name="S1" type="s" access="readwrite" />
        <property name="S2" type="s" access="readwrite" />
        <property name="S3" type="s" access="readwrite" />
        <property name="Joined" type="s" access="read" />
        <method name="JoinEw">
            <arg direction="out" name="result" type="i" />
        </method>
    </interface>
</node>
"""

XML4 = """
<node>
    <interface name="exmpl.worker4">
        <property name="SS" type="as" access="readwrite" />
        <property name="Joined" type="s" access="read" />
        <method name="JoinEw">
            <arg direction="out" name="result" type="i" />
        </method>
    </interface>
</node>
"""