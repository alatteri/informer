import os
import time

import instinctual.informer
from instinctual.informer.event import DiscreetEvent

LOG = instinctual.informer.getLogger(__name__)

class Subject(object):
    def __init__(self):
        LOG.debug("__init__ called for Subject()")
        self._observers = list()

    def registerObserver(self, observer):
        LOG.debug("registerObserver()")
        self._observers.append(observer)

    def notifyObservers(self, event):
        for observer in self._observers:
            observer.notify(event)

class LogSubject(Subject):
    def __init__(self, path):
        self._path = path
        self._handle = None
        super(LogSubject, self).__init__()

    def operate(self):
        LOG.debug("Now watching %s for input" % (self._path))
        previous = None
        self._handle = self._open()

        while (1):
            lines = list()
            now = os.stat(self._path)
            if previous and now.st_ino != previous.st_ino:
                # make sure there is nothing else to read
                lines = self._read()

                LOG.debug("---- RESET ----")
                self._handle.close()
                self._handle = self._open()
                previous = now
            elif not previous or now.st_mtime > previous.st_mtime:
                # process lines
                lines = self._read()
                previous = now
            else:
                time.sleep(1)

            for line in lines:
                event = self.event(line)
                self.notifyObservers(event)

    def _open(self):
        return open(self._path, 'r')

    def _read(self):
        LOG.debug("---- one readline ---")
        lines = self._handle.readlines(100*1024)
        return lines


class DiscreetLogSubject(LogSubject):
    def event(self, line):
        return DiscreetEvent(line)

