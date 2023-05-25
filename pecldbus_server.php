<?php

class Factory
{
    static $wnames = ['Worker1', 'Worker2', 'Worker3', 'Worker4'];
    static $counters = [0, 0, 0, 0];

    # it doesn't seem like pecl-dbus supports exporting instantiated objects, only static members
    # so instead of doing "proper" OOP, we imitate it by dynamically creating a new anonymous class
    # every time - first by giving it an alias (https://stackoverflow.com/a/52861290),
    # then by wrapping it in eval (https://www.php.net/manual/en/function.eval.php#121837)

    static function Create($worker_type)
	{
        if (!in_array($worker_type, Factory::$wnames))
            throw new UnexpectedValueException("Wrong worker type");
        $ci = intval(substr($worker_type, 6)) - 1;
        Factory::$counters[$ci]++;
        $nm = sprintf("/%s/Instance%d", $worker_type, Factory::$counters[$ci]);
        eval($GLOBALS['classW'.($ci+1)]);
        $newClassName = get_class($newClass);
        class_alias($newClassName, $nm);
        $GLOBALS["conn"]->registerObject($nm, 'exmpl.worker'.($ci+1), $nm);
        $GLOBALS["conn"]->registerObject($nm, 'org.freedesktop.DBus.Properties', $nm);
        return $nm;
	}

    static function Destroy($object_name)
    {
        # doesn't seem to be any way to unregisterObject either
    }

    static function StopService()
    {
        $GLOBALS["work"] = false;
    }
}

$props_code = '
    static function Get($interface_name, $prop_name)
    {
        return new DBusVariant(static::$$prop_name);
    }
    static function Set($interface_name, $prop_name, $val)
    {
        static::$$prop_name = $val->getData();
    }
';

$GLOBALS["classW1"] = '
    $newClass = new class {
        static float $N1 = 0, $N2 = 0, $N3 = 0, $N4 = 0, $Summ = 0, $Diff = 0;
        '.$props_code.'
        static function Calc()
        {
            static::$Summ = static::$N1 + static::$N2 + static::$N3 + static::$N4;
            static::$Diff = static::$N1 - static::$N2 - static::$N3 - static::$N4;
            return 0;
        }
    };
';

$GLOBALS["classW2"] = '
    $newClass = new class {
        static $NS = array();
        static float $Summ = 0, $Diff = 0;
        '.$props_code.'
        static function Calc()
        {
            $arr = static::$NS->GetData();
            static::$Summ = array_sum($arr);
            static::$Diff = $arr[0] - array_sum(array_slice($arr, 1));
            return 0;
        }
    };
';

$GLOBALS["classW3"] = '
    $newClass = new class {
        static string $S1, $S2, $S3, $Joined;
        '.$props_code.'
        static function JoinEw()
        {
            $res = array();
            $l1 = strlen(static::$S1);
            $l2 = strlen(static::$S2);
            $l3 = strlen(static::$S3);
            $mx = max($l1, $l2, $l3);
            for ($i = 0; $i < $mx; $i++)
            {
                if ($i < $l1) array_push($res, static::$S1[$i]);
                if ($i < $l2) array_push($res, static::$S2[$i]);
                if ($i < $l3) array_push($res, static::$S3[$i]);
            }
            static::$Joined = implode("", $res);
            return 0;
        }
    };
';

$GLOBALS["classW4"] = '
    $newClass = new class {
        static $SS = array();
        static string $Joined;
        '.$props_code.'
        static function JoinEw()
        {
            $inp = static::$SS->GetData();
            $res = array();
            $lens = array_map("strlen", $inp);
            for ($i = 0; $i < max($lens); $i++)
                foreach ($inp as $s)
                    if ($i < strlen($s))
                        array_push($res, $s[$i]);
            static::$Joined = implode("", $res);
            return 0;
        }
    };
';

if ($argc > 1 and $argv[1] == '--system')
{
    echo "START (SYSTEM BUS)\n";
    $bus = Dbus::BUS_SYSTEM;
}
else
{
    echo "START\n";
    $bus = Dbus::BUS_SESSION;
}

$GLOBALS["conn"] = new Dbus($bus, true);
$GLOBALS["conn"]->requestName('com.exmpl.v0001');
$GLOBALS["work"] = true;
$conn->registerObject('/factory', 'exmpl.factory', 'Factory');
do
{
    $GLOBALS["conn"]->waitLoop();
}
while ($GLOBALS["work"]);
echo "STOP\n";
?>