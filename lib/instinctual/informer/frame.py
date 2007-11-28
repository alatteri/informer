import os
import errno
import instinctual
from instinctual.serializable import Serializable

class Frame(Serializable):
    def __init__(self, parent):
        Serializable.__init__(self, parent, 'frame.pkl')
        self.rgbPath = os.path.join(self.container, 'frame.rgb')

        self.isBusy = False

        self.width  = None
        self.height = None
        self.depth  = None

        self.start  = None
        self.number = None
        self.end    = None
        self.rate   = None

        self.host = None
        self.createdBy = None
        self.createdOn = None

        self.setup = None
        self.spark = None

        self.resizedDepth  = None
        self.resizedWidth  = None
        self.resizedHeight = None
        self.resizedPath   = None

    def delete(self):
        try:
            if self.rgbPath:
                os.unlink(self.rgbPath)
        except OSError, e:
            if errno.ENOENT != e.errno:
                raise e

        try:
            if self.resizedPath:
                os.unlink(self.resizedPath)
        except OSError, e:
            if errno.ENOENT != e.errno:
                raise e

        Serializable.delete(self)
