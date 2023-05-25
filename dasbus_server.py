import sys

from dasbus.connection import SessionMessageBus, SystemMessageBus
from dasbus.error import DBusError
from dasbus.loop import EventLoop
from dasbus.server.interface import dbus_interface
from dasbus.typing import Str

from dasbus_common import XML0, XML1, XML2, XML3, XML4


BUS = None
LOOP = EventLoop()


@dbus_interface("exmpl.factory")
class Factory(object):

    def Create(self, worker_type: Str) -> Str:
        if not worker_type.startswith('Worker') or int(worker_type[6:]) not in [1, 2, 3, 4]:
            raise DBusError("Unknown worker")
        cn = 'counter' + worker_type[6:]
        c = getattr(self, cn) if hasattr(self, cn) else 0
        setattr(self, cn, c + 1)
        nm = "/%s/Instance%d" % (worker_type, c + 1)
        constructor = globals()[worker_type]
        BUS.publish_object(nm, constructor())
        return nm

    def Destroy(self, object_name: Str):
        BUS.unpublish_object(object_name)

    def StopService(self):
        LOOP.quit()


class Worker1(object):

    __dbus_xml__ = XML1

    N1, N2, N3, N4 = 0, 0, 0, 0
    Summ, Diff = 0, 0

    def Calc(self):
        self.Summ = self.N1 + self.N2 + self.N3 + self.N4
        self.Diff = self.N1 - self.N2 - self.N3 - self.N4
        return 0


class Worker2(object):

    __dbus_xml__ = XML2

    NS = []
    Summ, Diff = 0, 0

    def Calc(self):
        self.Summ = sum(self.NS)
        self.Diff = self.NS[0] - sum(self.NS[1:])
        return 0


class Worker3(object):

    __dbus_xml__ = XML3

    S1, S2, S3, Joined = "", "", "", ""

    def JoinEw(self):
        res = []
        for i in range(max([len(s) for s in [self.S1, self.S2, self.S3]])):
            for s in [self.S1, self.S2, self.S3]:
                if i < len(s):
                    res += s[i]
        self.Joined = ''.join(res)
        return 0


class Worker4(object):

    __dbus_xml__ = XML4

    SS = []
    Joined = ""
    
    def JoinEw(self):
        res = []
        for i in range(max([len(s) for s in self.SS])):
            for s in self.SS:
                if i < len(s):
                    res += s[i]
        self.Joined = ''.join(res)
        return 0


if __name__ == "__main__":
    try:
        BUS = SystemMessageBus() if '--system' in sys.argv else SessionMessageBus()
        print("START" + (" (SYSTEM BUS)" if '--system' in sys.argv else ""))
        BUS.publish_object("/factory", Factory())
        BUS.register_service("com.exmpl.v0001")
        LOOP.run()
    finally:
        BUS.disconnect()
        print('STOP')