import os
import sys
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
        # figure out the logfile path
        if 'DL_FLAVOR' in os.environ:
            root = "/usr/discreet/log/" + os.environ['DL_FLAVOR']
            version = commands.getoutput(GET_FLAME_VERSION)
            hostname = commands.getoutput("hostname -s")
            self.logfile = "%s%s_%s_shell.log" % (root, version, hostname)
        else:
            err = "Unable to determine logfile path: env DL_FLAVOR not set"
            raise EnvironmentError(err)

        print "-----------> LOGFILE:", self.logfile

        self.app = None
        self.threads = {}

        # --------------------
        # THREADS
        #
        # self.threads['log'] = LogfileThread(self.logfile)
        # self.threads['events'] = None
        # self.threads['osd'] = None
        # self.threads['gui'] = None

        self.threads['A'] = SampleThread('A', 5)
        #self.threads['B'] = SampleThread('B', 7)
        #self.threads['C'] = SampleThread('C', 11)

    def start(self):
        for thread in self.threads.values():
            print "STARTING THREAD: %s..." % (thread.name)
            thread.start()

    def stop(self):
        for thread in self.threads.values():
            if thread.isAlive():
                print "STOPPING THREAD: %s..." % (thread.name)
                thread.join()
            else:
                print "Thread %s was dead\n" % (thread.name)

class _MyObserver(Observer):
    def process(self, event):
        appEvent = AppEvent(event)

        LOG.warn(''.join(['-'*20, 'App Event', '-'*20]))
        for (key, val) in appEvent.asDict().items():
            LOG.warn("%s: %s" % (key, val))
        LOG.warn(''.join(['-'*20, '---------', '-'*20]))

        client = Client()
        client.newAppEvent(appEvent)

class LogfileThread(threading.Thread):
    def __init__(self, logpath):
        threading.Thread.__init__(self)

        observer = _MyObserver()
        self.app = DiscreetAppSubject(logpath, wait=True)
        self.app.registerObserver(observer)

    def run(self):
        LOG.info("Now going to kick off the logwatcher")
        self.app.operate()

class SampleThread(threading.Thread):
    def __init__(self, name, wait):
        threading.Thread.__init__(self)

        self.name = name
        self.wait = wait

    def run(self):
        import time
        # while True:
        for i in range(5):
            print "Thread %s: hello. [wait: %s]" % (self.name, self.wait)
            time.sleep(self.wait)
        print "Thread %s: goodbye." % (self.name)
