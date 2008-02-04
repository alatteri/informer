import os
import time
import datetime
from pprint import pprint

import instinctual
import instinctual.informer
from instinctual.parser.subject import Subject
from instinctual.parser.event import *
from instinctual.parser.observer import *

from instinctual.informer.frame import Frame
from instinctual.informer.spark import Spark
from instinctual.informer.client import Client
from instinctual.informer.threads import LogfileThread, SchedulerThread, InformerThread

LOG = instinctual.getLogger(__name__)
CONF = instinctual.getConf()

uploadsDir = CONF.get('informer', 'dir_uploads')

def datetimeToSeconds(dt):
    inSeconds = time.mktime(dt.timetuple())
    return inSeconds + float(dt.microsecond) / (1 * 10**6)


class UploadFlushThread(InformerThread):
    def __init__(self, name, app):
        InformerThread.__init__(self, name)
        self.app = app

    def threadProcess(self):
        """
        Called to flush pending frames
        """
        if self.app.frames:
            now = datetimeToSeconds(datetime.now())

            # flush any pending frames
            for (sparkName, frame) in self.app.frames.items():
                diff = now - frame.createdOnInSeconds
                print "((((((((((( DIFF: %s ))))))))))))" % (diff)

                if diff > 1.0:
                    print "diff was greater than 1.0 -- marked for UPLOADING"
                    print "sparkProcessStart: uploading waiting frame..."
                    frame.status = 'upload'
                    frame.save()

                    del self.app.frames[sparkName]

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
        # FRAMES
        #
        self.frames = {}

        # --------------------
        # THREADS
        #
        self.logfile = LogfileThread('logfile')
        self.scheduler = SchedulerThread('scheduler', interval=0.1)
        self.uploadFlush = UploadFlushThread('uploadFlush', self)

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

    def _getShouldProcess(self):
        print "_getShouldProcess returning: %s" % (self._shouldProcess)
        return self._shouldProcess

    def _setShouldProcess(self, val):
        print "_setShouldProcess setting to: %s" % (val)
        self._shouldProcess = val

    shouldProcess = property(_getShouldProcess, _setShouldProcess)

    def resetAppState(self):
        self.user = None
        self.volume = None
        self.project = None
        self.hostname = None
        self.frameRate = None

    def resetBatchState(self):
        self.shot = None
        self.setup = None
        self.events = []
        self.outputs = {}

    # ----------------------------------------------------------------------
    # App Control
    # ----------------------------------------------------------------------
    def start(self):
        self.resetAppState()
        self.resetBatchState()

        self.scheduler.register(self.logfile, 0.1)
        self.scheduler.register(self.uploadFlush, 1.0)
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
    # Sparks
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
            self.sparks[newName].name = newName
            del self.sparks[oldName]
        else:
            print "Could not rename! [%s] was not found" % (oldName)

    def sparkProcessStart(self, name):
        print "hello" * 20
        print "w00t: sparkProcessStart called with: %s" % (name)
        if name in self.frames:
            frame = self.frames[name]
            now = datetimeToSeconds(datetime.now())
            diff = now - frame.createdOnInSeconds
            print "((((((((((( DIFF: %s ))))))))))))" % (diff)

            if diff < 1.0:
                print "diff was less than 1.0 -- DELETING"
                print "sparkProcessStart: deleting waiting frame..."
                self.frames[name].delete()
            else:
                print "diff was greater than 1.0 -- UPLOADING"
                print "sparkProcessStart: uploading waiting frame..."
                frame.status = 'upload'
                frame.save()

        self.frames[name] = 'IGNORE'
        print "hello" * 20

    def sparkProcessEnd(self, name):
        print "goodbye" * 20
        print "w00t: sparkProcessEnd called with: %s" % (name)
        del self.frames[name]
        print "goodbye" * 20

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------
    def flushEventQueue(self):
        LOG.info("(((( flushing event queue ))))")
        key = '_LAST_EVENT_'

        while self.events:
            appEvent = self.events.pop(0)
            if isinstance(appEvent, DiscreetAppBatchProcessEvent):
                # wait until we flush the queue to determine the batch outputs
                appEvent.outputs = self.outputs.keys()

            if key in os.environ:
                print "<<<<<<<<< _LAST_EVENT_ %s >>>>>>>>>>" % (os.environ[key])

            eSeconds = datetimeToSeconds(appEvent.date)
            print "<<<<<<<<< eSeconds     %s >>>>>>>>>>" % (eSeconds)

            if key not in os.environ or float(os.environ[key]) < eSeconds:
                LOG.debug("SENDING EVENT... LOOKS GOOD")
                print ("SENDING EVENT... LOOKS GOOD")
                client = Client()
                client.newEvent(appEvent)
                os.environ[key] = str(eSeconds)
            else:
                print ("SKIPPING EVENT! LAST EVENT WAS MORE RECENT...")

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
    # Frames
    # ----------------------------------------------------------------------
    def setFrameRate(self, frameRate):
        print "setFrameRate:", frameRate
        self.frameRate = frameRate

    def frameProcessStart(self, sparkName, width, height, depth, start, number, end):
        print "+" * 80
        print "APP: frameProcessStart called for (%s)" % (sparkName)
        print "+" * 80

        if sparkName in self.frames:
            frame = self.frames[sparkName]
            if 'IGNORE' == frame:
                print "----- IGNORING FRAME! -------"
                return None
            else:
                print "frameProcessStart flushing frame..."
                frame.status = 'upload'
                frame.save()
                del self.frames[sparkName]

        f = Frame(uploadsDir)
        self.frames[sparkName] = f

        print "1"
        f.isBusy = True
        f.spark  = sparkName
        f.setup  = self.setup

        print "2"
        f.host = self.hostname
        print "2.1"
        f.createdBy = self.user
        print "2.2"
        f.createdOn = datetime.now()
        print "2.3"
        f.createdOnInSeconds = datetimeToSeconds(f.createdOn)

        print "3"
        f.width  = width
        f.height = height
        f.depth  = depth

        print "4"
        f.start  = start
        f.number = number
        f.end    = end
        f.rate   = self.frameRate

        print "Created frame"
        pprint(f.__dict__)

        print "about to save"
        f.save()
        print "ok. save is done"

        return f.rgbPath

    def frameProcessEnd(self, sparkName):
        print "+" * 80
        print "APP: frameProcessEnd called"
        print "+" * 80

        f = self.frames[sparkName]
        f.isBusy = False
        f.save()

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
        data['modified_by'] = createdBy
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

        self.flushEventQueue()
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

        self.flushEventQueue()
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
        self.flushEventQueue()
        self.resetBatchState()

        LOG.info("--- LOAD SETUP %s", (setup))

        self.setup = setup
        parsed = instinctual.informer.parseSetup(setup)
        self.shot = parsed['shot']

    def cbSaveSetup(self, event, setup, **kwargs):
        """
        Called when a setup has been saved
        - flushes the batch queue
        """
        LOG.info("--- SAVE SETUP %s", (setup))

        appEvent = self._setAppEvent(DiscreetAppSaveEvent(), event)
        self.events.append(appEvent)
        self.flushEventQueue()

    def cbBatchProcess(self, event, **kwargs):
        """
        Called when the user processes a batch setup
        """
        LOG.info("BATCH PROCESS EVENT")

        print "=" * 80
        print "                     BATCH PROCESS EVENT"
        print "=" * 80
        appEvent = self._setAppEvent(DiscreetAppBatchProcessEvent(), event)
        self.events.append(appEvent)
        self.flushEventQueue()
