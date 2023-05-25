import sys

from dasbus.connection import SessionMessageBus, SystemMessageBus
from dasbus.specification import DBusSpecification

from dasbus_common import XML0, XML1, XML2, XML3, XML4


def test1(bus, factory, reps=1):
    for i in range(1, 5 * reps):
        obj_name = factory.Create('Worker1')
        W = bus.get_proxy(service, obj_name)
        W._handler._specification = DBusSpecification.from_xml(XML1)
        W.N1 = 0.11 + i * 1000
        W.N2 = 0.11 + i * 100
        W.N3 = 0.11 + i * 10
        W.N4 = 0.11 + i
        W.Calc()
        print("%s %.2f %.2f" % (obj_name, W.Summ, W.Diff))
        factory.Destroy(obj_name)


def test2(bus, factory, reps=1):
    for i in range(1, 5 * reps):
        obj_name = factory.Create('Worker2')
        W = bus.get_proxy(service, obj_name)
        W._handler._specification = DBusSpecification.from_xml(XML2)
        W.NS = [0.11 + i * 1000, 0.11 + i * 100, 0.11 + i * 10, 0.11 + i]
        W.Calc()
        print("%s %.2f %.2f" % (obj_name, W.Summ, W.Diff))
        factory.Destroy(obj_name)


def test3(bus, factory, reps=1):
    inp1 = ["eme", "eme", "abcde", "pqrst"]
    inp2 = ["xp1", "xp2", "ABCDE", "PQRST_tail"]
    inp3 = ["al",  "al",  "12345", "67890"]
    for i in range(0, 4 * reps):
        obj_name = factory.Create('Worker3')
        W = bus.get_proxy(service, obj_name)
        W._handler._specification = DBusSpecification.from_xml(XML3)
        W.S1 = inp1[i % 4]
        W.S2 = inp2[i % 4]
        W.S3 = inp3[i % 4]
        W.JoinEw()
        print(obj_name, W.Joined)
        factory.Destroy(obj_name)


def test4(bus, factory, reps=1):
    a = [["eme", "xp1", "al"], ["eme", "xp2", "al"],
         ["abcde", "ABCDE", "12345"], ["pqrst", "PQRST_tail", "67890"]]
    for i in range(0, 4):
        obj_name = factory.Create('Worker4')
        W = bus.get_proxy(service, obj_name)
        W._handler._specification = DBusSpecification.from_xml(XML4)
        W.SS = a[i % 4]
        W.JoinEw()
        print(obj_name, W.Joined)
        factory.Destroy(obj_name)


if __name__ == "__main__":
    service = 'com.exmpl.v0001'
    bus = SystemMessageBus() if '--system' in sys.argv else SessionMessageBus()
    fc = bus.get_proxy(service, '/factory')
    fc._handler._specification = DBusSpecification.from_xml(XML0)
    test1(bus, fc)
    test2(bus, fc)
    test3(bus, fc)
    test4(bus, fc)
    if '--nostop' not in sys.argv:
        fc.StopService()