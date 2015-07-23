from morse.builder.creator import ActuatorCreator

class DirectControl(ActuatorCreator):
    _classpath = "jsbsim.actuators.directcontrol.DirectControl"
    _blendname = "directcontrol"

    def __init__(self, name=None):
        ActuatorCreator.__init__(self, name)

