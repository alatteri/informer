import instinctual
import instinctual.informer
from instinctual.informer.clip import Clip
from instinctual.informer.client import Client, AppEvent

LOG = instinctual.getLogger(__name__)

class Spark(object):
    def __init__(self, name):
        self.name = name
        self.clip = Clip()
        self.clip.save()

    def getNotes(self, setup):
        """
        Returns an array of note objects
        """
        client = Client()

        LOG.info("Running getNotes()")
        notes = client.getNotes(setup)

        LOG.info("Lookup found: %s notes" % (len(notes)))
        LOG.info("Found this: %s", (notes))

        return notes

    def getElements(self, setup):
        """
        Returns an array of element objects
        """
        client = Client()

        LOG.info("Running getElements()")
        elements = client.getElements(setup)

        LOG.info("Lookup found: %s elements" % (len(elements)))
        LOG.info("Found this: %s", (elements))

        return elements

    def createNote(self, setup, isChecked, text, createdBy):
        data = {}
        data['text'] = text
        data['created_by'] = createdBy
        data['is_checked'] = isChecked

        client = Client()

        LOG.info("Running createNote()")
        client.createNote(setup, data)

        return True

    def updateNote(self, setup, id, isChecked, modifiedBy):
        data = {}
        data['id'] = id
        data['is_checked'] = isChecked
        data['modified_by'] = modifiedBy

        client = Client()

        LOG.info("Running updateNote()")
        return client.updateNote(setup, data)

    def updateElement(self, setup, id, isChecked):
        data = {}
        data['id'] = id
        data['is_checked'] = isChecked

        client = Client()

        LOG.info("Running updateElement()")
        return client.updateElement(setup, data)

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
