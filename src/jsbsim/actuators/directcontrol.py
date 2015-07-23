import logging; logger = logging.getLogger("morse." + __name__)

from morse.core.external_object import ExternalActuator

from morse.core.services import service, async_service, interruptible
from morse.core import status
from morse.helpers.components import add_data, add_property

class DirectControl(ExternalActuator):
    """Write here the general documentation of your actuator.
    It will appear in the generated online documentation.
    """
    _name = "DirectControl"
    _short_desc = ""

    add_data('cmd', [], '[float]', 'A list of command')

    def __init__(self, obj, parent=None):
        logger.info("%s initialization" % obj.name)
        # Call the constructor of the parent class
        ExternalActuator.__init__(self, obj, parent)

        logger.info('Component initialized')


    def default_action(self):
        logger.info(self.local_data['cmd'])
