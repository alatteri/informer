import httplib2
from httplib2 import *

print "----- httplib2auth loaded ----"

class Http(httplib2.Http):
    def __init__(self, *args, **kwargs):
        print "word up. My __init__ was called"
        httplib2.Http.__init__(self, *args, **kwargs)
        self.add_credentials('spark', 'sparky')