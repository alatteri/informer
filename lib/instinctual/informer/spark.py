import os
import sys
import time
import Queue
import commands
import threading


import instinctual
import instinctual.informer
from instinctual.informer.client import Client, AppEvent
from instinctual.parser.observer import Observer
from instinctual.parser.subject import DiscreetAppSubject


# This was mirrored from $FLAME_HOME/bin/startApplication
GET_FLAME_VERSION = """cat $FLAME_HOME/FLAME_VERSION | cut -d '"' -f2 | sed 's/\.//g'"""


LOG = instinctual.getLogger(__name__)

class Spark(object):
    def __init__(self):

        self.scheduler = SchedulerThread('scheduler', interval=4.0)

        # --------------------
        # THREADS
        #
        # self.threads['events'] = None
        # self.threads['osd'] = None
        # self.threads['gui'] = None

        self.scheduler.register(SparkThread('A'), 5)
        self.scheduler.register(SparkThread('B'), 13)
        self.scheduler.register(SparkThread('C'), 17)
        self.scheduler.register(LogfileThread('logfile'), 1)

    def start(self):
        self.scheduler.queue.put('process')
        self.scheduler.start()

    def stop(self):
        self.scheduler.queue.put('stop')

class SparkThread(threading.Thread):
    def __init__(self, name):
        threading.Thread.__init__(self, name=name)

        self.name = self.getName()
        self.isProcessing = False
        self.lastProcess = 0
        self.queue = Queue.Queue(100)

    def getNextCommand(self):
        return self.queue.get()

    def run(self):
        shouldRun = True
        while shouldRun:
            cmd = self.getNextCommand()
            print "Thread [%s] got: %s" % (self.name, cmd)
            if cmd == 'process':
                self.isProcessing = True
                self.process()
                self.lastProcess = time.time()
                self.isProcessing = False
            elif cmd == 'stop':
                self.stop()
                shouldRun = False
            else:
                LOG.warn("unknown command passed to thread [%s]: %s" % (self.name, cmd))

        print "Thread %s: goodbye." % (self.name)

    def process(self):
        print "Thread %s: process called." % (self.name)

    def stop(self):
        print "Thread %s: stop called." % (self.name)


class SchedulerThread(SparkThread):
    def __init__(self, name, interval):
        SparkThread.__init__(self, name)
        self.waits = {}
        self.threads = {}
        self.interval = interval

    def register(self, thread, wait):
        self.threads[thread.name] = thread
        self.waits[thread.name] = wait

        print "STARTING THREAD: %s..." % (thread.name)
        thread.start()

    def getNextCommand(self):
        try:
            return self.queue.get(timeout=self.interval)
        except Queue.Empty, e:
            return 'process'

    def process(self):
        now = time.time()

        for thread in self.threads.values():
            if thread.isProcessing:
                print "Thread [%s] is currently processing" % (thread.name)
            else:
                wait = self.waits[thread.name]
                delta = now - thread.lastProcess
                if delta > wait:
                    thread.queue.put('process')
                else:
                    print("Thread [%s] needs [%s] more seconds time" %
                          (thread.name, wait - delta))

    def stop(self):
        for thread in self.threads.values():
            thread.queue.put('stop')
            if thread.isAlive():
                print "STOPPING THREAD: %s..." % (thread.name)
                thread.join()
            else:
                print "Thread %s was dead\n" % (thread.name)

class LogfileThread(SparkThread):
    def __init__(self, name):
        SparkThread.__init__(self, name)

        # figure out the logfile path
        if 'DL_FLAVOR' in os.environ:
            root = "/usr/discreet/log/" + os.environ['DL_FLAVOR']
            version = commands.getoutput(GET_FLAME_VERSION)
            hostname = commands.getoutput("hostname -s")
            self.logfile = "%s%s_%s_app.log" % (root, version, hostname)
        else:
            err = "Unable to determine logfile path: env DL_FLAVOR not set"
            raise EnvironmentError(err)

        print "-----------> LOGFILE:", self.logfile

        observer = _MyObserver()
        self.app = DiscreetAppSubject(self.logfile)
        self.app.registerObserver(observer)

    def process(self):
        LOG.info("Now going to kick off the logwatcher")
        self.app.operate()

class _MyObserver(Observer):
    def process(self, event):
        appEvent = AppEvent(event)

        LOG.warn(''.join(['-'*20, 'App Event', '-'*20]))
        for (key, val) in appEvent.asDict().items():
            LOG.warn("%s: %s" % (key, val))
        LOG.warn(''.join(['-'*20, '---------', '-'*20]))

        client = Client()
        client.newAppEvent(appEvent)

