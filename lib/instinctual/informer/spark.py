from pprint import pprint
from datetime import datetime
import instinctual
import instinctual.informer
from instinctual.informer.frame import Frame
from instinctual.informer.client import Client, AppEvent

LOG = instinctual.getLogger(__name__)

class Spark(object):
    count = 0
    conf = instinctual.getConf()
    frameDir = conf.get('informer', 'dir_frames')

    def __init__(self, name):
        print "||||||| spark __init__ got called with [%s]" % (name)
        if not name:
            self.count += 1
            name = "TEMP%d.Informer" % self.count
            print "name was not specified -- made", name

        self.name = name

    def processFrameStart(self, setup, user, host, width, height, depth, start, number, end, rate):
        print "+" * 80
        print "process frame start called"
        print "+" * 80

        self.f = Frame(self.frameDir)
        f = self.f

        f.isBusy = True
        f.spark  = self.name
        f.setup  = setup

        f.host = host
        f.createdBy = user
        f.createdOn = datetime.now()

        f.width  = width
        f.height = height
        f.depth  = depth

        f.start  = start
        f.number = number
        f.end    = end
        f.rate   = str(rate)

        print "Created frame"
        pprint(f.__dict__)

        f.save()
        return f.rgbPath

    def processFrameEnd(self):
        print "+" * 80
        print "process frame end called"
        print "+" * 80

        self.f.isBusy = False
        self.f.save()
        self.f = None
