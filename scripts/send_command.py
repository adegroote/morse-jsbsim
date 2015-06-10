import hla.rti
import hla.omt as fom
from morse.middleware.hla.message_buffer import MessageBufferWriter


class MyAmbassador(hla.rti.FederateAmbassador):
    def initialize(self, name):
        self.aircraft_handle = rtia.getObjectClassHandle("Aircraft")

        self.control_handle = rtia.getAttributeHandle("control", self.aircraft_handle)
        self.position_handle = rtia.getAttributeHandle("position", self.aircraft_handle)
        rtia.publishObjectClass(self.aircraft_handle, [self.control_handle])
        rtia.subscribeObjectClassAttributes(self.aircraft_handle, [self.control_handle])
        self._name = name
        self.obj = None
        self.owned = False

    def discoverObjectInstance(self, obj, objectclass, name):
        print("DISCOVER %s %s %s" % (name, obj, objectclass))
        if name == self._name:
            self.obj = obj
            rtia.attributeOwnershipAcquisition(self.obj, [self.control_handle], "bar")

    def attributeOwnershipAcquisitionNotification(self, obj, attr):
        print("OWNED %s %s" % (self._name, attr))
        self.owned = True


    def send_command(self, cmd):
        if not self.obj:
            print("%s not yet discovered" % self._name)
            return
        if not self.owned:
            print("%s not yet owned" %  self._name)
            return
        m = MessageBufferWriter()
        for i in range(0, 3):
            m.add_double(cmd[i])
        rtia.updateAttributeValues(self.obj, {self.control_handle: m.write() }, "foo")


rtia = hla.rti.RTIAmbassador()
mya = MyAmbassador()
rtia.joinFederationExecution("send_command", "morse_fdm", mya)

mya.initialize("robot")

try:
    while True:
        while (not mya.owned):
            rtia.tick()
        x = input("New command > ")
        cmd = [float(y) for y in x.split()]
        mya.send_command(cmd)
        rtia.tick()
except KeyboardInterrupt:
    pass

mya.terminate()

rtia.resignFederationExecution(hla.rti.ResignAction.DeleteObjectsAndReleaseAttributes)

print("Done.")
