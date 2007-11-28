import os
import uuid
import errno
import pickle

class Serializable(object):
    def __init__(self, parent, name):
        self._id = str(uuid.uuid1())
        self._container = os.path.join(parent, self._id)
        os.mkdir(self._container)
        self._path = os.path.join(self._container, name)

    def load(cls, path):
        input = open(path, 'rb')
        obj = pickle.load(input)
        input.close()
        return obj
    load = classmethod(load)

    def save(self):
        output = open(self.path, 'wb')
        pickle.dump(self, output)
        output.close()

    def delete(self):
        try:
            os.unlink(self.path)
        except OSError, e:
            if errno.ENOENT != e.errno:
                raise e

        print "[[[[[[[[ delete called for %s ]]]]]]]]]]]]" % (self.container)
        os.rmdir(self.container)

    def _getId(self):
        return self._id

    def _getPath(self):
        return self._path

    def _getContainer(self):
        return self._container

    uuid = property(_getId)
    path = property(_getPath)
    container = property(_getContainer)
