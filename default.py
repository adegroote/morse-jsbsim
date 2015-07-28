#! /usr/bin/env morseexec

""" Basic MORSE simulation scene for <morse-jsbsim> environment

Feel free to edit this template as you like!
"""

from morse.builder import *
from jsbsim.builder.jsbsim import JSBSimExporter
from jsbsim.builder.actuators import DirectControl
from jsbsim.builder.sensors import JsbsimMagnetometer

bpymorse.set_speed(fps = 100, logic_step_max = 5, physics_step_max = 5)

robot = QUAD2012()
robot.translate(x = 5, z = 5)
robot.properties(jsbsim_model = 'simple_quad')

teleport = Teleport()
robot.append(teleport)
#order is important, feet first, then ECEF
#teleport.alter('feet')
teleport.alter('geodetic')
#teleport.alter('BodyNED', 'jsbsim.modifiers.body_ned.BodyENUfromNED')
teleport.add_stream('hla', 'jsbsim.middleware.hla.read_aircraft_input.AircraftPoseInput')

ctrl = DirectControl()
robot.append(ctrl)
ctrl.add_stream('socket', 'morse.middleware.socket_datastream.SocketReader', direction = 'IN')
ctrl.add_stream('hla', 'jsbsim.middleware.hla.write_aircraft_ctrl.AircraftCtrl', direction = 'OUT')

pose = Pose()
robot.append(pose)
pose.add_stream('socket')

morse_mag = Magnetometer()
robot.append(morse_mag)
morse_mag.add_stream('socket')

jsbsim_mag = JsbsimMagnetometer()
robot.append(jsbsim_mag)
jsbsim_mag.add_stream('socket', 'morse.middleware.socket_datastream.SocketPublisher', direction = 'OUT')
jsbsim_mag.add_stream('hla', 'jsbsim.middleware.hla.read_magnetometer_input.MagnetometerInput', direction = 'IN')

# set 'fastmode' to True to switch to wireframe mode
env = Environment('sandbox', fastmode = False)
env.set_camera_location([-18.0, -6.7, 10.8])
env.set_camera_rotation([1.09, 0, -1.14])
env.properties(longitude = 1.26, latitude = 43.26, altitude = 130.0)
env.configure_stream_manager(
        'hla',
         fom = 'aircraft.fed', name = 'Morse', federation = 'morse_fdm',
         sync_point = 'Init', time_sync = True, sync_register = True)


env.set_log_level('morse.middleware.hla_datastream', 'debug')

JSBSimExporter(env).dump()
