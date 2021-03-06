import os
import errno
import instinctual
from instinctual.serializable import Serializable

LOG = instinctual.getLogger(__name__)

FRAME_DELETE='delete'
FRAME_UPLOAD='upload'
FRAME_UNKNOWN='unknown'

class Frame(Serializable):
    def __init__(self, parent):
        Serializable.__init__(self, parent, 'frame.pkl')
        self.rgbPath = os.path.join(self.container, 'frame.rgb')

        self.number = None
        self.status = FRAME_UNKNOWN
        self.isBusy = False

        self.width  = None
        self.height = None
        self.depth  = None
        self.pixelAspectRatio = None

        self.host = None
        self.createdBy = None
        self.createdOn = None

        self.pid = os.getpid()
        self.job = None
        self.spark = None

        self.quality = None
        self.resizedDepth  = None
        self.resizedWidth  = None
        self.resizedHeight = None
        self.resizedPath   = None

    def __str__(self):
        return self.rgbPath

    def delete(self):
        LOG.debug("Deleting frame %s" % self)

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
