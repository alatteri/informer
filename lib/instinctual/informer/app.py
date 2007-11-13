import instinctual
import instinctual.informer
from instinctual.parser.subject import Subject
from instinctual.parser.event import *
from instinctual.parser.observer import *

from instinctual.informer.spark import Spark
from instinctual.informer.client import Client, AppEvent
from instinctual.informer.threads import LogfileThread, SchedulerThread, InformerThread

LOG = instinctual.getLogger(__name__)

class _appinfo(InformerThread):
    def __init__(self, name, app):
        InformerThread.__init__(self, name)
        self.app = app

    def threadProcess(self):
        print "\n[[[[[ APP ]]]]]"
        print "* user:", self.app.user
        print "* setup:", self.app.setup
        print "* project:", self.app.project
        print "---- sparks ----"
        for (name, spark) in self.app.sparks.items():
            print "\t* [%s]: %s" % (name, spark)

class App(Subject):
    def __init__(self):
        Subject.__init__(self)

        # --------------------
        # APP STATE
        #
        self.resetAppState()
        self.resetBatchState()

        # --------------------
        # SPARKS
        #
        self.sparks = {}

        # --------------------
        # THREADS
        #
        self._appinfo = _appinfo('appinfo', self)
        self.logfile = LogfileThread('logfile')
        self.scheduler = SchedulerThread('scheduler', interval=0.1)

        # app state events
        self.logfile.registerObserver(DiscreetSpecifyHostname(self.cbSpecifyHostname))
        self.logfile.registerObserver(DiscreetSpecifyProject(self.cbSpecifyProject))
        self.logfile.registerObserver(DiscreetSpecifyVolume(self.cbSpecifyVolume))
        self.logfile.registerObserver(DiscreetSpecifyUser(self.cbSpecifyUser))

        # setup events
        self.logfile.registerObserver(DiscreetLoadSetup(self.cbLoadSetup))
        self.logfile.registerObserver(DiscreetSaveSetup(self.cbSaveSetup))

        # batch processing events
        self.logfile.registerObserver(DiscreetBatchProcess(self.cbBatchProcess))
        self.logfile.registerObserver(DiscreetBatchProcessOutput(self.cbBatchProcessOutput))

        # flushing events
        self.logfile.registerObserver(DiscreetBatchPlay(self.cbBatchPlay))
        self.logfile.registerObserver(DiscreetBatchEnd(self.cbBatchEnd))
        self.logfile.registerObserver(DiscreetAppExit(self.cbAppExit))

    def resetAppState(self):
        self.user = None
        self.volume = None
        self.project = None
        self.hostname = None

    def resetBatchState(self):
        self.shot = None
        self.setup = None
        self.queue = []
        self.outputs = {}

    # ----------------------------------------------------------------------
    # App Control
    # ----------------------------------------------------------------------
    def start(self):
        self.resetAppState()
        self.resetBatchState()

        self.scheduler.register(self._appinfo, 3)
        self.scheduler.register(self.logfile, 0.1)
        self.scheduler.process()
        self.scheduler.start()

    def stop(self):
        self.scheduler.stop()
        if self.scheduler.isAlive():
            print "STOPPING INTERPRETER..."
            self.scheduler.join()
        else:
            print "scheduler was dead."

        for t in self.scheduler.threads.values():
            if t.isAlive():
                t.stop()
            print "Thread [%s] is alive %s" % (t.name, t.isAlive())

        print "the scheduler is alive %s" % (self.scheduler.isAlive())

    def _suspend(self):
        # suspend threads that alter the app state
        self.logfile.suspend()

    def _resume(self):
        # resume threads that alter the app state
        self.logfile.resume()


    # ----------------------------------------------------------------------
    # ----------------------------------------------------------------------
    def _sparkCleanName(self, name):
        print "clean IN[%s]" % (name)
        if name and name[-1] == '\n':
            name = name[0:-1]
        print "clean OUT[%s]" % (name)
        return name

    def sparkRegister(self, name):
        name = self._sparkCleanName(name)
        print "Hey this is cool. registering spark", name
        spark = Spark(name)
        self.sparks[spark.name] = spark
        print "Now returning", spark.name
        return spark.name

    def sparkGetByName(self, name):
        name = self._sparkCleanName(name)
        print "In the app trying to get the spark named", name
        if name in self.sparks:
            print "ok found it"
            return self.sparks[name]
        else:
            print "nope."
            return None

    def sparkRename(self, oldName, newName):
        print "rename called with [%s] and [%s]" % (oldName, newName)
        oldName = self._sparkCleanName(oldName)
        newName = self._sparkCleanName(newName)
        if oldName == newName:
            print "old and new were the same:", newName
        elif oldName in self.sparks:
            print "Ok. renamed %s to %s" % (oldName, newName)
            self.sparks[newName] = self.sparks[oldName]
            del self.sparks[oldName]
        else:
            print "Could not rename! [%s] was not found" % (oldName)

    def flushBatchQueue(self):
        LOG.info("(((( flushing batch queue ))))")
        self._isBatchProcessing = False

        while self.queue:
            appEvent = self.queue.pop(0)
            if isinstance(appEvent, DiscreetAppBatchProcessEvent):
                # wait until we flush the queue to determine the batch outputs
                appEvent.outputs = self.outputs.keys()

            client = Client()
            client.newAppEvent(appEvent)

    def _setAppEvent(self, appEvent, logEvent):
        appEvent.date = logEvent.date
        appEvent.shot = self.shot
        appEvent.user = self.user
        appEvent.setup = self.setup
        appEvent.volume = self.volume
        appEvent.project = self.project
        appEvent.hostname = self.hostname
        return appEvent


    # ----------------------------------------------------------------------
    # App/Server interaction
    # ----------------------------------------------------------------------
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


    # ----------------------------------------------------------------------
    # Callbacks for modifying the App state
    # ----------------------------------------------------------------------
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
        parsed = instinctual.informer.parseSetup(setup)
        self.shot = parsed['shot']

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

        self._isBatchProcessing = True
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

