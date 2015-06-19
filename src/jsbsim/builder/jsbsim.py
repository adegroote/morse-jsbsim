from morse.builder.abstractcomponent import Configuration
from morse.builder import bpymorse
from morse.builder.data import MORSE_DATASTREAM_MODULE 
from morse.helpers.coordinates import CoordinateConverter
from math import degrees

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

    def _generate_env_properties(self):
        env_props = bpymorse.get_properties(self.env._bpy_object)
        longitude = env_props.get('longitude')
        geod_latitude = env_props.get('latitude')
        altitude = env_props.get('altitude')

        self.coord_conv = CoordinateConverter(geod_latitude, longitude, altitude)
        geoc_latitude = self.coord_conv.geodetic_to_geocentric(geod_latitude, altitude)

        self.properties['env'] = {'longitude' : longitude,
                                  'latitude': geoc_latitude,
                                  'altitude': altitude}

    def _generate_robot_properties(self):
        robots = {}
        for obj in bpymorse.get_objects():
            p = obj.game.properties
            if 'Robot_Tag' in p and 'jsbsim_model' in p:
                loc = obj.location
                rot = obj.rotation_euler
                model = p['jsbsim_model'].value

                coord = self.coord_conv.ltp_to_geodetic(numpy.matrix(loc))
                longitude = degrees(coord[0, 0])
                geod_altitude = degrees(coord[0, 1])
                altitude = coord[0, 2]
                geoc_latitude = self.coord_conv.geodetic_to_geocentric(geod_altitude, altitude)

                # XXX check angles orientation
                robots[obj.name] = {'longitude' : longitude,
                                    'latitude' : geoc_latitude,
                                    'altitude' : altitude,
                                    'yaw' : rot.z,
                                    'pitch': rot.y,
                                    'roll' : rot.x,
                                    'model': model }

        self.properties['robots'] = robots

    def _generate_properties(self):
        self._generate_hla_properties()
        self._generate_env_properties()
        self._generate_robot_properties()

    def dump(self):
        self._generate_properties()
        f = open("/tmp/jsbsim_config.json", "w")
        json.dump(self.properties, f, indent = 4)
