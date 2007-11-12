import os
import time

import instinctual
from instinctual.parser.event import *
from instinctual.parser.observer import *

LOG = instinctual.getLogger(__name__)

# ------------------------------------------------------------------------------
class Subject(object):
    """
    Subject base class. Supports registering and notifying
    a list of observers.
    """

    def __init__(self):
        LOG.debug("__init__ called for Subject()")
        self._observers = []

    def registerObserver(self, observer):
        LOG.debug("registerObserver()")
        self._observers.append(observer)

    def notifyObservers(self, event):
        LOG.debug("now going to notify with %s" % event.event)
        for observer in self._observers:
            observer.notify(event)

# ------------------------------------------------------------------------------
class LogSubject(Subject):
    """
    A Subject that generates events for each line appended
    """

    def __init__(self, path):
        self._path = path
        self._handle = None
        Subject.__init__(self)

        self._previous = None
        self._handle = self._open()
        LOG.info("__init__ called for LogSubject")

    def operate(self):
        LOG.debug("Now watching %s for input (previous: %s)" % (self._path, self._previous))

        lines = []
        now = os.stat(self._path)

        if self._previous is not None and now.st_ino != self._previous.st_ino:
            # make sure there is nothing else to read
            lines = self._read()

            LOG.debug("---- RESET ----")
            self._handle.close()
            self._handle = self._open()
            self._previous = now
        elif self._previous is None or now.st_mtime > self._previous.st_mtime:
            if not self._previous:
                LOG.debug("self._previous is False")
            else:
                LOG.debug("now mtime[%s] _previous mtime[%s]" % (now.st_mtime, self._previous.st_mtime))

            # process lines
            lines = self._read()
            self._previous = now
            LOG.debug("OK done with the read, previous: %s" % (self._previous))
        else:
            LOG.info("end of file reached")

        for line in lines:
            print "LOG --> ", line
            event = self.event(line)
            self.notifyObservers(event)

    def _open(self):
        LOG.info("_open called for %s" % (self._path))
        return open(self._path, 'r')

    def _read(self):
        LOG.debug("---- one readline ---")
        all_lines = []
        while 1:
            # TODO: should this be xreadlines?
            lines = self._handle.readlines(100*1024)
            if not lines: break
            all_lines.extend(lines)
        return all_lines

# ------------------------------------------------------------------------------
class DiscreetLogSubject(LogSubject):
    """
    A LogSubject class that returns DiscreetLogEvent events
    """

    def event(self, line):
        return DiscreetLogEvent(line)
