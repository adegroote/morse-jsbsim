import logging; logger = logging.getLogger("morse." + __name__)
from morse.middleware.hla.message_buffer import MessageBufferReader
from morse.middleware.hla.abstract_hla import AbstractHLAInput

class MagnetometerInput(AbstractHLAInput):
    def initialize(self):
        AbstractHLAInput.initialize(self)

        aircraft_handle = self.amb.object_handle('Aircraft')

        self.handle_mag = self.amb.attribute_handle("magnetometer", aircraft_handle)

        self.suscribe_attributes(aircraft_handle, [self.handle_mag])

    def default(self, ci = 'unused'):
        attributes = self.get_attributes()

        if attributes:
            mag_attr = attributes[self.handle_mag]

            if not mag_attr:
                return False

            m_mag = MessageBufferReader(mag_attr)

            self.data['x'] = m_mag.read_double()
            self.data['y'] = m_mag.read_double()
            self.data['z'] = m_mag.read_double()
            return True

        return False

