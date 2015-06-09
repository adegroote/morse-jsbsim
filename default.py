#! /usr/bin/env morseexec

""" Basic MORSE simulation scene for <morse-jsbsim> environment

Feel free to edit this template as you like!
"""

from morse.builder import *

robot = QUAD2012()

teleport = Teleport()
robot.append(teleport)
#order is important, feet first, then ECEF
teleport.alter('feet')
teleport.alter('ECEF')
teleport.alter('NED', 'morse.modifiers.ned.AnglesFromNED')
teleport.add_stream('hla', 'jsbsim.middleware.hla.read_aircraft_input.AircraftPoseInput')

pose = Pose()
robot.append(pose)
pose.add_stream('socket')

# set 'fastmode' to True to switch to wireframe mode
env = Environment('sandbox', fastmode = False)
env.set_camera_location([-18.0, -6.7, 10.8])
env.set_camera_rotation([1.09, 0, -1.14])
env.properties(longitude = 1.26, latitude = 43.26, altitude = 130)
env.configure_stream_manager(
        'hla',
         fom = 'aircraft.fed', name = 'Morse', federation = 'morse_fdm',
         sync_point = 'Init', time_sync = True)


env.set_log_level('morse.middleware.hla_datastream', 'debug')
