from morse.builder.abstractcomponent import Configuration
from morse.builder import bpymorse
from morse.builder.data import MORSE_DATASTREAM_MODULE 
from morse.helpers.coordinates import CoordinateConverter
from math import degrees, pi

import json
import numpy

class JSBSimExporter:
    def __init__(self, env):
        self.env = env
        self.properties = {}
        self.coord_conv = None

    def _generate_hla_properties(self):
        hla_stream_manager = MORSE_DATASTREAM_MODULE['hla']
        hla_config = Configuration.stream_manager[hla_stream_manager]
        hla_federation = hla_config['federation']
        hla_sync_point = hla_config.get('sync_point', None)
        if hla_sync_point:
            self.properties['hla'] = { 'federation' : hla_federation,
                                        'sync_point': hla_sync_point}
        else:
            self.properties['hla'] = { 'federation' : hla_federation}

    def _generate_simu_properties(self):
        self.properties['simu'] = { 'fps' : bpymorse.get_fps() }

    def _generate_env_properties(self):
        env_props = bpymorse.get_properties(self.env._bpy_object)
        longitude = env_props.get('longitude')
        geod_latitude = env_props.get('latitude')
        altitude = env_props.get('altitude')
        if 'angle_against_north' in env_props:
            angle = env_props.get('angle_against_north')  - pi/2
        else:
            angle = 0.0

        try:
            self.coord_conv = CoordinateConverter(geod_latitude, longitude, altitude, angle)
        except:
            self.coord_conv = CoordinateConverter(geod_latitude, longitude, altitude)

        geoc = self.coord_conv.ecef_to_geocentric(
                    self.coord_conv.ltp_to_ecef(
                    self.coord_conv.blender_to_ltp(
                        numpy.matrix([0.0, 0.0, 0.0]))))
        geoc[0, 0] = degrees(geoc[0, 0])
        geoc[0, 1] = degrees(geoc[0, 1])
        geoc[0, 2] -= self.coord_conv.A

        self.properties['env'] = {'longitude': geoc[0, 0],
                                  'latitude':   geoc[0, 1],
                                  'altitude':   geoc[0, 2] }

    def _generate_robot_properties(self):
        robots = {}
        for obj in bpymorse.get_objects():
            p = obj.game.properties
            if 'Robot_Tag' in p and 'jsbsim_model' in p:
                loc = obj.location
                rot = obj.rotation_euler
                model = p['jsbsim_model'].value

                geoc = self.coord_conv.ecef_to_geocentric(
                        self.coord_conv.ltp_to_ecef(
                            self.coord_conv.blender_to_ltp(numpy.matrix(loc))))
                geoc[0, 0] = degrees(geoc[0, 0])
                geoc[0, 1] = degrees(geoc[0, 1])
                geoc[0, 2] -= self.coord_conv.A

                # XXX check angles orientation
                robots[obj.name] = {'longitude' : geoc[0, 0],
                                    'latitude' :  geoc[0, 1],
                                    'altitude' :  geoc[0, 2],
                                    'yaw' : rot.z,
                                    'pitch': rot.y,
                                    'roll' : rot.x,
                                    'model': model }

        self.properties['robots'] = robots

    def _generate_properties(self):
        self._generate_simu_properties()
        self._generate_hla_properties()
        self._generate_env_properties()
        self._generate_robot_properties()

    def dump(self):
        self._generate_properties()
        f = open("/tmp/jsbsim_config.json", "w")
        json.dump(self.properties, f, indent = 4)
