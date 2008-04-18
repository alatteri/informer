import os
import time
import uuid
import datetime
import commands
from pprint import pprint

import instinctual
import instinctual.informer
from instinctual.parser.subject import Subject
from instinctual.parser.event import *
from instinctual.parser.observer import *

from instinctual.informer.frame import Frame, FRAME_UPLOAD
from instinctual.informer.spark import Spark, SparkDuplicateFrame
from instinctual.informer.client import Client, ClientConnectionError
from instinctual.informer.threads import LogfileThread, SchedulerThread, InformerThread

LOG = instinctual.getLogger(__name__)
CONF = instinctual.getConf()

uploadsDir = CONF.get('informer', 'dir_uploads')

def datetimeToSeconds(dt):
    inSeconds = time.mktime(dt.timetuple())
    return inSeconds + float(dt.microsecond) / (1 * 10**6)

# ------------------------------------------------------------------------------
class App(Subject):
    count = 0
    def __init__(self, program):
        Subject.__init__(self)

        self.program = program
        LOG.debug("The App program is: %s" % program)

        # --------------------
        # APP STATE
        #
        self.resetAppState()
        self.resetBatchState()

        # --------------------
        # SPARKS
        #
        self.id = str(self.count)
        self.count += 1
        self.spark = Spark(self.id)

        # --------------------
        # THREADS
        #
        self.logfile = LogfileThread('logfile', self._getAppLog())
        self.scheduler = SchedulerThread('scheduler', interval=0.1)

        # app state events
        self.logfile.registerObserver(DiscreetSpecifyHostname(self.cbSpecifyHostname))
        self.logfile.registerObserver(DiscreetSpecifyProject(self.cbSpecifyProject))
        self.logfile.registerObserver(DiscreetSpecifyVolume(self.cbSpecifyVolume))
        self.logfile.registerObserver(DiscreetSpecifyUser(self.cbSpecifyUser))

        self.logfile.registerObserver(DiscreetTimedMessage(self.cbTimedMessage))

        # setup events
        self.logfile.registerObserver(DiscreetLoadSetup(self.cbLoadSetup))
        self.logfile.registerObserver(DiscreetSaveSetup(self.cbSaveSetup))
        self.logfile.registerObserver(DiscreetSaveCurrent(self.cbSaveCurrent))
        self.logfile.registerObserver(DiscreetLoadInformerSetup(self.cbLoadInformerSetup))

        # batch processing events
        self.logfile.registerObserver(DiscreetBatchProcess(self.cbBatchProcess))
        self.logfile.registerObserver(DiscreetBurnProcess(self.cbBurnProcess))

    def _getAppLog(self):
        # This was mirrored from $FLAME_HOME/bin/startApplication
        cmd = """cat $%s_HOME/%s_VERSION | cut -d '"' -f2 | sed 's/\.//g'"""
        cmd = cmd % (self.program.upper(), self.program.upper())
        version = commands.getoutput(cmd)
        root = "/usr/discreet/log/" + self.program
        hostname = commands.getoutput("hostname -s")

        appLog = "%s%s_%s_app.log" % (root, version, hostname)
        return appLog

    def isBurn(self):
        return self.program == 'burn'

    def resetAppState(self):
        self.user = None
        self.volume = None
        self.project = None
        self.hostname = None
        self.frameRate = None
        self.pixelAspectRatio = None

    def resetBatchState(self):
        self.shot = None
        self.setup = None
        self.events = []
        self.outputs = {}
        self.lastJob = None
        self.lastProcess = None
        self.ignoreFrames = False

    # ----------------------------------------------------------------------
    # App Control
    # ----------------------------------------------------------------------
    def start(self):
        self.resetAppState()
        self.resetBatchState()

        self.scheduler.register(self.logfile, 0.1)
        self.scheduler.process()
        self.scheduler.start()

    def stop(self):
        self.scheduler.stop()
        if self.scheduler.isAlive():
            LOG.debug("STOPPING INTERPRETER...")
            self.scheduler.join()
        else:
            LOG.debug("scheduler was dead.")

        for t in self.scheduler.threads.values():
            if t.isAlive():
                t.stop()
            LOG.debug("Thread [%s] is alive %s" % (t.name, t.isAlive()))

        LOG.debug("the scheduler is alive %s" % (self.scheduler.isAlive()))

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
        if name and name[-1] == '\n':
            name = name[0:-1]
        return name

    def sparkRegister(self, name):
        self.spark.name = self._sparkCleanName(name)
        return self.spark.name

    def sparkGetByName(self, name):
        name = self._sparkCleanName(name)
        return self.spark

    def sparkRename(self, oldName, newName):
        self.spark.name = newName

    def sparkProcessStart(self, name):
        """
        SparkProcessStart is called from the spark when the user enters
        the spark. The sequence of events is usually:
            * SparkProcessStart
            * FrameProcessStart
            * FrameProcessEnd
            * SparkProcessEnd

        However, the first time the spark is entered the sequence can be:
            * enter spark
            * FrameProcessStart
            * FrameProcessEnd
            * SparkProcessStart
            * SparkProcessEnd

        This is unfortunate since we process a frame when it should have
        been ignored.

        Since the app is not interested in calls to FrameProcess
        during edit SparkProcessStart sets a flag to ignore the
        next Frame to be processed.
        """
        self.ignoreFrames = True

    def sparkProcessEnd(self, name):
        """
        Always called after a SparkProcessEnd it marks the point where the
        app can once again save frames.
        """
        self.ignoreFrames = False

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

            lastEvent = None
            if key in os.environ:
                lastEvent = float(os.environ[key])

            eventSeconds = float(datetimeToSeconds(appEvent.date))

            if lastEvent is None or eventSeconds > lastEvent:
                LOG.debug("SENDING EVENT... LOOKS GOOD")
                try:
                    client = Client()
                    if appEvent.job:
                        client.createRender(appEvent)
                    else:
                        client.createEvent(appEvent)
                except Exception, e:
                    LOG.warn("Error sending event: %s [%s]" % (e, type(e)))

                # set the last event time, even if it failed
                # an event is either a success or failure
                # only setting this on success would allow for retries
                # if a person exited batch and re-entered -- don't think
                # that is what we want
                #
                # explictly state the precision of the float
                os.environ[key] = "%.6f" % eventSeconds
            else:
                LOG.debug("SKIPPING EVENT! LAST EVENT WAS MORE RECENT...")

    def _setAppEvent(self, appEvent, logEvent):
        appEvent.date = logEvent.date
        appEvent.shot = self.shot
        appEvent.user = self.user
        appEvent.setup = self.setup
        appEvent.volume = self.volume
        appEvent.project = self.project
        appEvent.hostname = self.hostname
        return appEvent

    def setFrameRate(self, frameRate):
        LOG.debug("setFrameRate: %s" % frameRate)
        self.frameRate = frameRate

    def setPixelAspectRatio(self, pixelAspectRatio):
        LOG.debug("setPixelAspectRatio: %s" % pixelAspectRatio)
        self.pixelAspectRatio = pixelAspectRatio

    # ----------------------------------------------------------------------
    # Frame event processing
    # ----------------------------------------------------------------------
    def frameProcessStart(self, sparkName, width, height, depth, start, number, end):
        """
        Called by the spark when a frame is being processed. If the app is
        not ignoring frames then it will create a new Frame object to represent
        the process.

        Returns: None if frame should be ignored, or path for rgb file
        """
        LOG.debug("APP: frameProcessStart called for (%s)" % (sparkName))

        if self.ignoreFrames:
            LOG.debug("----- IGNORING FRAME! -------")
            return None

        f = Frame(uploadsDir)

        f.isBusy = True
        f.spark  = sparkName

        f.host = self.hostname
        f.createdBy = self.user
        f.createdOn = datetime.now()
        f.createdOnInSeconds = datetimeToSeconds(f.createdOn)

        f.number = number
        f.width  = width
        f.height = height
        f.depth  = depth
        f.pixelAspectRatio = self.pixelAspectRatio

        if self.isBurn():
            f.job = self.lastJob
            f.status = FRAME_UPLOAD

        try:
            # associate the frame with the spark
            self.spark.registerFrame(f)
            LOG.debug("Created frame")
            LOG.debug("about to save")
            f.save()
            LOG.debug("ok. save is done")
        except SparkDuplicateFrame, e:
            f.delete()
            return None

        return f.rgbPath

    def frameProcessEnd(self, sparkName):
        """
        Called after the spark has written the frame to disk.
        """
        LOG.debug("APP: frameProcessEnd called")

        f = self.spark.getLastFrame()
        f.isBusy = False
        f.save()

    # ----------------------------------------------------------------------
    # App/Server interaction
    # ----------------------------------------------------------------------
    def getNotes(self):
        """
        Returns an array of note objects
        """
        client = Client()

        try:
            LOG.info("Running getNotes()")
            notes = client.getNotes(self.setup)

            LOG.info("Lookup found: %s notes" % (len(notes)))
            LOG.info("Found this: %s", (notes))
            return notes
        except ClientConnectionError, e:
            if 404 == e.resp.status:
                return []

    def getElements(self):
        """
        Returns an array of element objects
        """
        client = Client()

        try:
            LOG.info("Running getElements()")
            elements = client.getElements(self.setup)

            LOG.info("Lookup found: %s elements" % (len(elements)))
            LOG.info("Found this: %s", (elements))
            return elements
        except ClientConnectionError, e:
            if 404 == e.resp.status:
                return []

    def createNote(self, isChecked, text):
        data = {}
        data['text'] = text
        data['created_by'] = self.user
        data['modified_by'] = self.user
        data['is_checked'] = isChecked

        client = Client()

        LOG.info("Running createNote()")
        client.createNote(self.setup, data)

        return True

    def updateNote(self, id, isChecked):
        data = {}
        data['id'] = id
        data['is_checked'] = isChecked
        data['modified_by'] = self.user

        client = Client()

        LOG.info("Running updateNote()")
        return client.updateNote(self.setup, data)

    def updateElement(self, id, isChecked):
        data = {}
        data['id'] = id
        data['is_checked'] = isChecked

        client = Client()

        LOG.info("Running updateElement()")
        return client.updateElement(self.setup, data)

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

    def cbLoadInformerSetup(self, event, setup, job, **kwargs):
        """
        Called when an informer setup file was loaded.
        """
        if self.isBurn() and not self.lastJob:
            self.lastJob = job
            LOG.debug("set job to: %s" % self.lastJob)


    def cbSaveSetup(self, event, setup, **kwargs):
        """
        Called when a setup has been saved
        - flushes the batch queue
        """
        LOG.info("--- SAVE SETUP %s" % (setup))

        self.setup = setup
        parsed = instinctual.informer.parseSetup(setup)
        self.shot = parsed['shot']

        appEvent = self._setAppEvent(DiscreetAppSaveEvent(), event)
        self.events.append(appEvent)
        self.flushEventQueue()

    def cbSaveCurrent(self, event, **kwargs):
        """
        Called when the current setup has been saved
        - flushes the batch queue
        """
        LOG.info("--- SAVE CURRENT SETUP")

        appEvent = self._setAppEvent(DiscreetAppSaveEvent(), event)
        self.events.append(appEvent)
        self.flushEventQueue()

    def cbBatchProcess(self, event, **kwargs):
        """
        Called when the user processes a batch setup
        """
        LOG.info("BATCH PROCESS EVENT")

        appEvent = self._setAppEvent(DiscreetAppBatchProcessEvent(), event)
        LOG.debug("1")
        appEvent.job = str(uuid.uuid1())
        LOG.debug("2")
        appEvent.rate = self.frameRate
        LOG.debug("3")

        self.lastJob = appEvent.job
        LOG.debug("4")
        self.lastProcess = appEvent
        LOG.debug("5")
        self.events.append(appEvent)
        LOG.debug("6")
        self.flushEventQueue()
        LOG.debug("7")

    def cbBurnProcess(self, event, job, **kwargs):
        """
        Called when the user does a burn process
        """
        LOG.info("BURN PROCESS EVENT (job: %s)" % job)

        appEvent = self._setAppEvent(DiscreetAppBatchProcessEvent(), event)
        appEvent.job = job
        appEvent.rate = self.frameRate

        self.lastJob = appEvent.job
        self.lastProcess = appEvent
        self.events.append(appEvent)
        self.flushEventQueue()


    def cbTimedMessage(self, event, **kwargs):
        """
        Called when any log message with a time is seen. Due to the buggy bevahior of
        the limitied API calls available from the spark the log file is used to drive
        frame uploading.

        This method receives Log file events and uses them to mark pending flags for
        upload or deletion.
        """
        if self.isBurn():
            # Nothing to do. All frames processed under burn are automatically
            # marked for upload.
            pass
        elif self.lastProcess:
            LOG.debug("last process was set: uploading...")
            self.spark.uploadFramesOlderThan(datetimeToSeconds(event.date), self.lastJob)
        else:
            LOG.debug("last process was none -- calling spark delete")
            self.spark.deleteFramesOlderThan(datetimeToSeconds(event.date))

        # Now, check to see if we need to clear the lastProcess state...
        # This was determined by looking at various log files and trying
        # to see what might be expected after a BatchProcess...

        if str.lower(event.level) != 'notice':
            # ignore errors and others
            # [error] 65548 swxAudioBlockCache.C:251 07/19/07:17:31:00.960 Unable to load info for frame 0x7: Frame id not found
            # [error] 65536 wireDevice.C:2464 07/19/07:17:31:00.961 Could not get audio info: Frame id not found.
            # [error] 65548 swxAudioBlockCache.C:251 07/19/07:17:31:00.962 Unable to load info for frame 0x7: Frame id not found
            pass
        elif event.category == 'HOTKEY' and event.message in ('POP', 'PUSH Timeline'):
            # ignore whatever these HOTKEY events are... I'm not sure
            # [notice] 65536 hotkeyControl.C:598 08/15/07:17:58:31.915 HOTKEY : POP
            # [notice] 65536 hotkeyControl.C:588 08/15/07:17:58:31.994 HOTKEY : PUSH Timeline
            pass
        elif event.category == 'BATCH':
            # ignore batch generated events
            # [notice] 65536 messageAccumulator.C:268 08/15/07:17:58:33.000 BATCH : No output for process.
            # [notice] 65536 messageAccumulator.C:268 08/15/07:15:35:53.401 BATCH : Processing output4. 1500 frames. 1 processed
            # [notice] 2564825280 messageAccumulator.C:268 03/03/08:21:38:10.144 BATCH : WARNING: Processing in proxy mode.
            pass
        elif event.message.startswith('Last entry repeated '):
            # repeated messges (even of errors or other ignored events)
            # [notice] 1109219680 CoMsgLogger.cpp:199 03/03/08:21:08:25.408 Last entry repeated 3 more times.
            # [notice] 1109219680 CoMsgLogger.cpp:199 03/03/08:21:22:56.000 Last entry repeated 1 more times.
            pass
        elif event.category == 'BUTTON' and event.message == '[Confirm] Confirm(ON)':
            # Allow the user to click OK to render in proxy mode...
            # [notice] 2564825280 menu.C:3140 03/03/08:21:38:12.505 BUTTON : [Confirm] Confirm(ON)
            pass
        elif event.message.startswith('Created partition') or event.category.startswith('Creating partition'):
            # [notice] 3086871040 volumeMgr.C:1717 03/13/08:03:33:10.530 Creating partition: width=360 height=244 name=CUSTOM_RES_360x244_24
            # [notice] 3086871040 volumeMgr.C:1856 03/13/08:03:33:10.544 Created partition "CUSTOM_RES_360x244_24" (0xa000016067975e5) of type video
            pass
        elif event.message.startswith('AUTOSAVE'):
            # [notice] 2588263744 clipMgt.C:476 03/23/08:22:45:46.455 AUTOSAVE ( begin )
            # [notice] 2588263744 clipMgt.C:568 03/23/08:22:45:46.509 AUTOSAVE ( completed in 0.1 sec )
            pass
        elif self.lastProcess is not None:
            LOG.debug("Setting lastProcess to None!")
            self.lastJob = None
            self.lastProcess = None
