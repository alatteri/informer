from pprint import pprint
from datetime import datetime
import instinctual
import instinctual.informer
from instinctual.informer.frame import Frame, FRAME_UPLOAD, FRAME_DELETE
from instinctual.informer.client import Client

LOG = instinctual.getLogger(__name__)

class SparkDuplicateFrame(Exception):
    pass

class Spark(object):
    count = 0

    def __init__(self, name):
        #print "||||||| spark __init__ got called with [%s]" % (name)
        if not name:
            self.count += 1
            name = "TEMP%d.Informer" % self.count
            #print "name was not specified -- made", name

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
        LOG.debug("delete: checking frames older than %s" % seconds)
        if self.frames:
            self.new = []
            for i in range(len(self.frames)):
                #print "frame", i, "time", self.frames[i].createdOnInSeconds
                if seconds > self.frames[i].createdOnInSeconds:
                    LOG.debug("deleting!")
                    self.frames[i].status = FRAME_DELETE
                    self.frames[i].save()
                else:
                    self.new.append(self.frames[i])
            self.frames = self.new

    def uploadFramesOlderThan(self, seconds, job):
        LOG.debug("UPLOAD: checking frames older than %s" % seconds)
        if self.frames:
            self.new = []
            for i in range(len(self.frames)):
                #print "frame", i, "time", self.frames[i].createdOnInSeconds
                if seconds > self.frames[i].createdOnInSeconds:
                    LOG.debug("uploading!")
                    self.frames[i].job = job
                    self.frames[i].status = FRAME_UPLOAD
                    self.frames[i].save()
                else:
                    self.new.append(self.frames[i])
            self.frames = self.new
