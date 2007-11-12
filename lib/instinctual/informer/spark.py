import instinctual
import instinctual.informer
from instinctual.informer.clip import Clip
from instinctual.informer.client import Client, AppEvent

LOG = instinctual.getLogger(__name__)

class Spark(object):
    count = 0

    def __init__(self, name):
        print "||||||| spark __init__ got called with [%s]" % (name)
        if not name:
            self.count += 1
            name = "TEMP%d.Informer" % self.count
            print "name was not specified -- made", name

        self.name = name
        self.clip = Clip()
        self.clip.save()

    def processFrame(self, width, height, depth, frameNo):
        f = self.clip.newFrame()
        f.width = width
        f.height = height
        f.depth = depth
        f.frameNo = frameNo

        print "width:", f.width
        print "height:", f.height
        print "depth:", f.depth
        print "frameNo:", f.frameNo

        f.save()
        return f.rgbPath
