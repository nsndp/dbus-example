<?php
$bus = ($argc > 1 and $argv[1] == '--system') ? Dbus::BUS_SYSTEM : Dbus::BUS_SESSION;
$conn = new Dbus($bus, true);
$service = 'com.exmpl.v0001';
$factory = $conn->createProxy($service, '/factory', 'exmpl.factory');

for ($i = 1; $i < 5; $i++)
{
    $ifc = 'exmpl.worker1';
    $obj_name = $factory->Create('Worker1');
    $obj = $conn->createProxy($service, $obj_name, $ifc);
    $props = $conn->createProxy($service, $obj_name, 'org.freedesktop.DBus.Properties');
    $props->Set($ifc, 'N1', new DBusVariant(0.11 + $i * 1000));
    $props->Set($ifc, 'N2', new DBusVariant(0.11 + $i * 100));
    $props->Set($ifc, 'N3', new DBusVariant(0.11 + $i * 10));
    $props->Set($ifc, 'N4', new DBusVariant(0.11 + $i));
    
    $obj->Calc();
    echo sprintf("%s %.2f %.2f\n", $obj_name,
        $props->Get($ifc, 'Summ')->getData(),
        $props->Get($ifc, 'Diff')->getData());
    $factory->Destroy($obj_name);
}

for ($i = 1; $i < 5; $i++)
{
    $ifc = 'exmpl.worker2';
    $obj_name = $factory->Create('Worker2');
    $obj = $conn->createProxy($service, $obj_name, $ifc);
    $props = $conn->createProxy($service, $obj_name, 'org.freedesktop.DBus.Properties');
    
    $arr = [0.11, 0.11, 0.11, 0.11];
    for ($j = 0; $j < 4; $j++) $arr[$j] += $i * pow(10, 3 - $j);
    $arr = new DBusArray(DBUS::DOUBLE, $arr);
    $props->Set($ifc, 'NS', new DBusVariant($arr, "ad"));
    
    $obj->Calc();
    echo sprintf("%s %.2f %.2f\n", $obj_name,
        $props->Get($ifc, 'Summ')->getData(),
        $props->Get($ifc, 'Diff')->getData());
    $factory->Destroy($obj_name);
}

$inp1 = ['eme', 'eme', 'abcde', 'pqrst'];
$inp2 = ['xp1', 'xp2', 'ABCDE', 'PQRST_tail'];
$inp3 = ['al',  'al',  '12345', '67890'];

for ($i = 0; $i < 4; $i++)
{
    $ifc = 'exmpl.worker3';
    $obj_name = $factory->Create('Worker3');
    $obj = $conn->createProxy($service, $obj_name, $ifc);
    $props = $conn->createProxy($service, $obj_name, 'org.freedesktop.DBus.Properties');
    
    $props->Set($ifc, 'S1', new DBusVariant($inp1[$i % 4]));
    $props->Set($ifc, 'S2', new DBusVariant($inp2[$i % 4]));
    $props->Set($ifc, 'S3', new DBusVariant($inp3[$i % 4]));
    
    $obj->JoinEw();
    echo $obj_name." ".$props->Get($ifc, 'Joined')->getData()."\n";
    $factory->Destroy($obj_name);
}

$inp = [['eme', 'xp1', 'al'], ['eme', 'xp2', 'al'],
        ['abcde', 'ABCDE', '12345'], ['pqrst', 'PQRST_tail', '67890']];

for ($i = 0; $i < 4; $i++)
{
    $ifc = 'exmpl.worker4';
    $obj_name = $factory->Create('Worker4');
    $obj = $conn->createProxy($service, $obj_name, $ifc);
    $props = $conn->createProxy($service, $obj_name, 'org.freedesktop.DBus.Properties');
    
    $arr = new DBusArray(DBUS::STRING, $inp[$i % 4]);
    $props->Set($ifc, 'SS', new DBusVariant($arr, "as"));
    
    $obj->JoinEw();
    echo $obj_name." ".$props->Get($ifc, 'Joined')->getData()."\n";
    $factory->Destroy($obj_name);
}

if ($argc <= 2 or $argv[2] != '--nostop') $factory->StopService();
?>