import logging; logger = logging.getLogger("morse." + __name__)

from morse.modifiers.abstract_modifier import AbstractModifier

class BodyENUfromNED(AbstractModifier):
    """ Convert the angles from NED to ENU. """
    def modify(self):
        self.data['pitch'] = -self.data['pitch']
        self.data['roll'] = self.data['roll']
        self.data['yaw'] = -self.data['yaw']
