import json

class JSBSimExporter:
    def __init__(self):
        pass

    def dump(self):
        dict_ = { 'hla' : { 'federation' : 'morse_fdm',
                           'sync_point' : 'Init' }}

        f = open("/tmp/jsbsim_config.json", "w")
        json.dump(dict_, f, indent = 4)
