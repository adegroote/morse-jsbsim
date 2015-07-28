import logging; logger = logging.getLogger("morse." + __name__)

from morse.helpers.components import add_data
from morse.core.external_object import ExternalSensor

class JsbsimMagnetometer(ExternalSensor):
    """Write here the general documentation of your sensor.
    It will appear in the generated online documentation.
    """
    _name = "JsbsimMagnetometer"
    _short_desc = "Magnemoter as computed by JSBSim"

    add_data('x', 0.0, "float",
            'Northern component of the magnetic field vector, in nT')
    add_data('y', 0.0, "float",
            'Eastern component of the magnetic field vector, in nT')
    add_data('z', 0.0, "float",
            'Downward component of the magnetic field vector, in nT')


    def __init__(self, obj, parent=None):
        logger.info("%s initialization" % obj.name)
        # Call the constructor of the parent class
        ExternalSensor.__init__(self, obj, parent)

        logger.info('Component initialized')

    def default_action(self):
        """ Main loop of the sensor.

        Implements the component behaviour
        """
        pass
