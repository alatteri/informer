from pprint import pprint
from datetime import datetime
import instinctual
import instinctual.informer
from instinctual.informer.frame import Frame, FRAME_UPLOAD
from instinctual.informer.client import Client

LOG = instinctual.getLogger(__name__)

class SparkDuplicateFrame(Exception):
    pass

class Spark(object):
    count = 0

    def __init__(self, name):
        print "||||||| spark __init__ got called with [%s]" % (name)
        if not name:
            self.count += 1
            name = "TEMP%d.Informer" % self.count
            print "name was not specified -- made", name

        self.name = name
        self.frames = []

    def registerFrame(self, frame):
        """
        Associates a frame object with the spark
        """
        # if frame.number in self.frames:
        #    raise SparkDuplicateFrame("Frame %s already seen" % frame.number)
        # self.frames[frame.number] = frame
        self.frames.append(frame)

    def getLastFrame(self):
        """
        Returns the most recent frame
        """
        return self.frames[len(self.frames)-1]

    def deleteFramesOlderThan(self, seconds):
        print "delete: checking frames older than", seconds
        if self.frames:
            self.new = []
            for i in range(len(self.frames)):
                print "frame", i, "time", self.frames[i].createdOnInSeconds
                if seconds > self.frames[i].createdOnInSeconds:
                    print "deleting!"
                    self.frames[i].delete()
                else:
                    print "keeping!"
                    self.new.append(self.frames[i])
            self.frames = self.new

    def uploadFramesOlderThan(self, seconds):
        print "UPLOAD: checking frames older than", seconds
        if self.frames:
            self.new = []
            for i in range(len(self.frames)):
                print "frame", i, "time", self.frames[i].createdOnInSeconds
                if seconds > self.frames[i].createdOnInSeconds:
                    print "uploading!"
                    self.frames[i].status = FRAME_UPLOAD
                    self.frames[i].save()
                else:
                    print "keeping!"
                    self.new.append(self.frames[i])
            self.frames = self.new
