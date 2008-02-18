import httplib2
from httplib2 import *

import instinctual

print "----- httplib2auth loaded ----"

_auths = []

_conf   = instinctual.getConf()
_proto  = _conf.get('informer', 'proto').lower()
_server = _conf.get('informer', 'hostname').lower()

class Http(httplib2.Http):
    def __init__(self, *args, **kwargs):
        print "word up. My __init__ was called"
        httplib2.Http.__init__(self, *args, **kwargs)
        self.add_credentials('spark', 'sparky')

        # used the cached auths
        global _auths
        self.authorizations = _auths

    def request(self, uri, *args, **kwargs):
        if 'https' == _proto and uri.lower().startswith("http://" + _server):
            uri = 'https' + uri[4:]
        return httplib2.Http.request(self, uri, *args, **kwargs)
