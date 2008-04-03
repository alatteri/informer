import os
import time
import uuid
import errno
import pickle
import random

import instinctual
LOG = instinctual.getLogger(__name__)

class Serializable(object):
    def __init__(self, parent, name):
        self._id = str(uuid.uuid1())
        rand = str(random.randint(0,999999))
        now = str(time.time())
        self._container = os.path.join(parent, now + '-' + rand)
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
        LOG.debug("[[[[[[[[ delete called for %s ]]]]]]]]]]]]" % (self.container))

        for root, dirs, files in os.walk(self.container):
            for f in files:
                os.remove(os.path.join(root, f))
            for d in dirs:
                os.remove(os.path.join(root, d))

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
