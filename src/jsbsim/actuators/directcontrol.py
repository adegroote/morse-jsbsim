import logging; logger = logging.getLogger("morse." + __name__)

from morse.core.external_object import ExternalActuator
from morse.helpers.components import add_data, add_property

class DirectControl(ExternalActuator):
    """
    The DirectControl actuator takes some control value, such as engine
    power and apply them. 
    """
    _name = "DirectControl"
    _short_desc = ""
    
    add_property('cmd_description', [], 'cmd_description', '[string]',
                 ' A short name to describe  the channel associated \
                  to each value of cmd')
    add_data('cmd', [], '[float]', 'A list of command')

    def __init__(self, obj, parent=None):
        logger.info("%s initialization" % obj.name)
        # Call the constructor of the parent class
        ExternalActuator.__init__(self, obj, parent)

        self.cmd_description = self.cmd_description.split(',')
        logger.info('Component initialized')

    def default_action(self):
        logger.debug(self.local_data['cmd'])
