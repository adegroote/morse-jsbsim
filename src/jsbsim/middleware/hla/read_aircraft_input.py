import logging; logger = logging.getLogger("morse." + __name__)
from morse.middleware.hla.message_buffer import MessageBufferReader
from morse.middleware.hla.abstract_hla import AbstractHLAInput

class AircraftPoseInput(AbstractHLAInput):
    def initialize(self):
        AbstractHLAInput.initialize(self)

        aircraft_handle = self.amb.object_handle('Aircraft')

        self.handle_position = self.amb.attribute_handle("position", aircraft_handle)
        self.handle_orientation = self.amb.attribute_handle("orientation", aircraft_handle)

        self.suscribe_attributes(aircraft_handle, [self.handle_position, self.handle_orientation])

    def default(self, ci = 'unused'):
        attributes = self.get_attributes()

        if attributes:
            pos_attr = attributes[self.handle_position]
            rot_attr = attributes[self.handle_orientation]

            if not pos_attr or not rot_attr:
                return 

            self.data['x'] = MessageBufferReader(pos_attr).read_double()
            self.data['y'] = MessageBufferReader(pos_attr).read_double()
            self.data['z'] = MessageBufferReader(pos_attr).read_double()
            self.data['yaw'] = MessageBufferReader(rot_attr).read_double()
            self.data['pitch'] = MessageBufferReader(rot_attr).read_double()
            self.data['roll'] = MessageBufferReader(rot_attr).read_double()

