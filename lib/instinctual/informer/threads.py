import os
import sys
import time
import Queue
import commands
import threading

import instinctual
from instinctual.parser.subject import DiscreetLogSubject

threading._VERBOSE = True

LOG = instinctual.getLogger(__name__)

THREAD_PROCESS = 'process'
THREAD_RESUME  = 'resume'
THREAD_STOP    = 'stop'
THREAD_SUSPEND = 'suspend'

class InformerThread(threading.Thread):
    def __init__(self, name):
        threading.Thread.__init__(self, name=name)

        self.name = self.getName()
        self.isProcessing = False
        self.isSuspended = False
        self.lastProcess = 0
        self.queue = Queue.Queue(100)

    def getNextCommand(self):
        return self.queue.get()

    def run(self):
        shouldRun = True
        while shouldRun:
            cmd = self.getNextCommand()
            # print "Thread [%s] got: %s" % (self.name, cmd)
            if cmd == THREAD_PROCESS:
                if self.isSuspended:
                    print ("Ignoring 'process' thread [%s] is suspended" % (self.name))
                else:
                    self.isProcessing = True
                    self.threadProcess()
                    self.lastProcess = time.time()
                    self.isProcessing = False
            elif cmd == THREAD_STOP:
                LOG.info("Stopping thread [%s]" % (self.name))
                self.threadStop()
                shouldRun = False
            elif cmd == THREAD_SUSPEND:
                print ("Got call to suspend thread (%s)" % (self.name))
                self.isSuspended = True
            elif cmd == THREAD_RESUME:
                print ("Got call to resume thread (%s)" % (self.name))
                self.isSuspended = False
            else:
                LOG.warn("unknown command passed to thread [%s]: %s" % (self.name, cmd))

        print "Thread %s: goodbye." % (self.name)

    def process(self):
        self.queue.put(THREAD_PROCESS)

    def resume(self):
        self.queue.put(THREAD_RESUME)

    def stop(self):
        self.queue.put(THREAD_STOP)

    def suspend(self):
        self.queue.put(THREAD_SUSPEND)

    def threadProcess(self):
        print "Thread %s: process called." % (self.name)

    def threadStop(self):
        print "Thread %s: stop called." % (self.name)


class SchedulerThread(InformerThread):
    def __init__(self, name, interval):
        InformerThread.__init__(self, name)
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
            return THREAD_PROCESS

    def threadProcess(self):
        now = time.time()

        for thread in self.threads.values():
            if thread.isProcessing:
                # print "Thread [%s] is currently processing" % (thread.name)
                pass
            else:
                wait = self.waits[thread.name]
                delta = now - thread.lastProcess
                if delta > wait:
                    thread.process()

    def threadStop(self):
        for thread in self.threads.values():
            thread.stop()
            if thread.isAlive():
                print "STOPPING THREAD: %s..." % (thread.name)
                thread.join()
            else:
                print "Thread %s was dead\n" % (thread.name)

class LogfileThread(InformerThread):
    # This was mirrored from $FLAME_HOME/bin/startApplication
    GET_FLAME_VERSION = """cat $FLAME_HOME/FLAME_VERSION | cut -d '"' -f2 | sed 's/\.//g'"""

    def __init__(self, name):
        InformerThread.__init__(self, name)

        # figure out the logfile path
        if 'DL_FLAVOR' in os.environ:
            root = "/usr/discreet/log/" + os.environ['DL_FLAVOR']
            version = commands.getoutput(self.GET_FLAME_VERSION)
            hostname = commands.getoutput("hostname -s")
            self.logfile = "%s%s_%s_app.log" % (root, version, hostname)
        else:
            err = "Unable to determine logfile path: env DL_FLAVOR not set"
            raise EnvironmentError(err)

        print "-----------> LOGFILE:", self.logfile

        self.logger = DiscreetLogSubject(self.logfile)

    def threadProcess(self):
        self.logger.operate()

    def registerObserver(self, *args, **kwargs):
        self.logger.registerObserver(*args, **kwargs)