import os
import errno
import instinctual
from instinctual.serializable import Serializable

class Frame(Serializable):
    def __init__(self, parent):
        Serializable.__init__(self, parent, 'frame.pkl')
        self._rgbPath = os.path.join(self.container, 'frame.rgb')

        self.width = None
        self.height = None
        self.depth = None
        self.frameNo = None

    def delete(self):
        try:
            os.unlink(self.rgbPath)
        except OSError, e:
            if errno.ENOENT != e.errno:
                raise e

        Serializable.delete(self)

    def _getRgbPath(self):
        return self._rgbPath

    rgbPath = property(_getRgbPath)
