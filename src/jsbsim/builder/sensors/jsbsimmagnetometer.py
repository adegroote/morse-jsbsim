from morse.builder.creator import SensorCreator

class JsbsimMagnetometer(SensorCreator):
    _classpath = "jsbsim.sensors.jsbsim_magnetometer.JsbsimMagnetometer"
    _blendname = "jsbsimmagnetometer"

    def __init__(self, name=None):
        SensorCreator.__init__(self, name)

