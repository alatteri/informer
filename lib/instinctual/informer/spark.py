from pprint import pprint
from datetime import datetime
import instinctual
import instinctual.informer
from instinctual.informer.frame import Frame
from instinctual.informer.client import Client

LOG = instinctual.getLogger(__name__)

class Spark(object):
    count = 0
    conf = instinctual.getConf()
    frameDir = conf.get('informer', 'dir_uploads')

    def __init__(self, name):
        print "||||||| spark __init__ got called with [%s]" % (name)
        if not name:
            self.count += 1
            name = "TEMP%d.Informer" % self.count
            print "name was not specified -- made", name

        self.name = name
