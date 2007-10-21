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
            print "GOT ", line
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

# ------------------------------------------------------------------------------
class DiscreetAppSubject(Subject):
    """
    A high-level subject, generates DiscreetAppEvents based on accumulation
    of lower-level DiscreetLogEvents

    Arguments:
        logpath: the path of the logfile
    """

    def __init__(self, logpath):
        Subject.__init__(self)

        self._logpath = logpath
        self.log = DiscreetLogSubject(self._logpath)

        # app state events
        self.log.registerObserver(DiscreetSpecifyHostname(self.cbSpecifyHostname))
        self.log.registerObserver(DiscreetSpecifyProject(self.cbSpecifyProject))
        self.log.registerObserver(DiscreetSpecifyVolume(self.cbSpecifyVolume))
        self.log.registerObserver(DiscreetSpecifyUser(self.cbSpecifyUser))

        # setup events
        self.log.registerObserver(DiscreetLoadSetup(self.cbLoadSetup))
        self.log.registerObserver(DiscreetSaveSetup(self.cbSaveSetup))

        # batch processing events
        self.log.registerObserver(DiscreetBatchProcess(self.cbBatchProcess))
        self.log.registerObserver(DiscreetBatchProcessOutput(self.cbBatchProcessOutput))

        # flushing events
        self.log.registerObserver(DiscreetBatchPlay(self.cbBatchPlay))
        self.log.registerObserver(DiscreetBatchEnd(self.cbBatchEnd))
        self.log.registerObserver(DiscreetAppExit(self.cbAppExit))

        self.resetAppState()
        self.resetBatchState()

    def _setAppEvent(self, appEvent, logEvent):
        appEvent.date = logEvent.date
        appEvent.shot = self.shot
        appEvent.user = self.user
        appEvent.setup = self.setup
        appEvent.volume = self.volume
        appEvent.project = self.project
        appEvent.hostname = self.hostname
        return appEvent

    def resetAppState(self):
        self.user = None
        self.volume = None
        self.project = None
        self.hostname = None

    def resetBatchState(self):
        self.queue = []
        self.outputs = {}
        self.shot = None
        self.setup = None

    def flushBatchQueue(self):
        LOG.info("(((( flushing batch queue ))))")
        while self.queue:
            appEvent = self.queue.pop(0)
            if isinstance(appEvent, DiscreetAppBatchProcessEvent):
                # wait until we flush the queue to determine the batch outputs
                appEvent.outputs = self.outputs.keys()
            self.notifyObservers(appEvent)

    def parseShot(self, setup):
        # TODO: Add shot parsing of the setup file path
        return os.path.basename(setup)

    def operate(self):
        self.log.operate()

    def cbSpecifyHostname(self, hostname, **kwargs):
        """
        Usually specified once in an app's run. Identifies the
        hostname of the computer running the app.
        """
        LOG.info("--- HOSTNAME: %s" % (hostname))

        self.hostname = hostname

    def cbSpecifyProject(self, project, **kwargs):
        """
        Called to identify the current project being worked on.
        - flushes the batch queue
        - resets the batch state
        """
        LOG.info("--- PROJECT: %s" % (project))

        self.flushBatchQueue()
        self.resetBatchState()

        self.project = project

    def cbSpecifyVolume(self, volume, **kwargs):
        """
        Called to identify the current volume being worked with
        """
        LOG.info("--- VOLUME: %s" % (volume))

        self.volume = volume

    def cbSpecifyUser(self, user, **kwargs):
        """
        Called to identify the current user.
        - Changing the user flushes the batch queue
        """
        LOG.info("--- USER: %s" % (user))

        self.flushBatchQueue()
        self.user = user

    def cbLoadSetup(self, event, setup, **kwargs):
        """
        Called when a new setup has been loaded
        - flushes the batch queue
        - resets the batch state
        """
        # --------------------
        # Make sure to flush the event queue for any previous events
        # before reassigning the setup and shot
        #
        self.flushBatchQueue()
        self.resetBatchState()

        LOG.info("--- LOAD SETUP %s", (setup))

        self.setup = setup
        self.shot = self.parseShot(setup)

        appEvent = self._setAppEvent(DiscreetAppLoadEvent(), event)
        self.queue.append(appEvent)
        self.flushBatchQueue()

    def cbSaveSetup(self, event, setup, **kwargs):
        """
        Called when a setup has been saved
        - flushes the batch queue
        """
        LOG.info("--- SAVE SETUP %s", (setup))

        appEvent = self._setAppEvent(DiscreetAppSaveEvent(), event)
        self.queue.append(appEvent)
        self.flushBatchQueue()

    def cbBatchProcess(self, event, **kwargs):
        """
        Called when the user processes a batch setup
        """
        LOG.info("BATCH PROCESS EVENT")

        appEvent = self._setAppEvent(DiscreetAppBatchProcessEvent(), event)
        self.queue.append(appEvent)

    def cbBatchProcessOutput(self, output, **kwargs):
        """
        Called once per batch process output
        - adds a new output entry to the batch state
        """
        LOG.info("--- BATCH OUTPUT %s" % (output))
        self.outputs[output] = 1

    def cbBatchPlay(self, **kwargs):
        """
        Called when the user plays the result of a batch process
        Used to flush the batch event queue
        """
        LOG.info("--- BATCH PLAY")
        self.flushBatchQueue()

    def cbBatchEnd(self, **kwargs):
        """
        Called when the user exits batch mode. This actually does
        not reset the batch state.
        - flushes the batch queue
        """
        LOG.info("--- EXIT BATCH MODE")
        self.flushBatchQueue()

    def cbAppExit(self, **kwargs):
        """
        Called when the user exits the app.
        - flushes the batch queue
        - resets the batch state
        """
        LOG.info("--- APP EXIT")
        self.flushBatchQueue()
        self.resetBatchState()
        self.resetAppState()

