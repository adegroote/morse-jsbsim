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
                return False

            m_pos = MessageBufferReader(pos_attr)
            m_or = MessageBufferReader(rot_attr)

            self.data['x'] = m_pos.read_double()
            self.data['y'] = m_pos.read_double()
            self.data['z'] = m_pos.read_double()
            self.data['yaw'] = m_or.read_double()
            self.data['pitch'] = m_or.read_double()
            self.data['roll'] = m_or.read_double()

            return True

        return False

