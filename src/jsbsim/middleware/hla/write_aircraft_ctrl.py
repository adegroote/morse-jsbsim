import logging; logger = logging.getLogger("morse." + __name__)
from morse.middleware.hla.message_buffer import MessageBufferWriter
from morse.middleware.hla.abstract_hla import AbstractHLAOutput


class AircraftCtrl(AbstractHLAOutput):
    def initialize(self):
        AbstractHLAOutput.initialize(self)

        aircraft_handle = self.amb.object_handle('Aircraft')

        self.control_handle = self.amb.attribute_handle("control", aircraft_handle)

        self.publish_attributes(aircraft_handle, [self.control_handle])

    def default(self, ci = 'unused'):
        cmd = self.data['cmd']
        if cmd:
            m = MessageBufferWriter()
            for i in range(0, len(cmd)):
                m.add_float(cmd[i])
            self.update_attribute({self.control_handle : m.write()})
