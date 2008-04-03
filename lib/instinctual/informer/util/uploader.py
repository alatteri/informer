#!/usr/bin/env python

# ------------------------------------------------------------------------------
# START BOOTSTRAP
# ------------------------------------------------------------------------------
try:
    import instinctual
except ImportError, e:
    import os
    import sys
    lib = os.sep.join(os.path.abspath(__file__).split(os.sep)[:-2] + ['lib'])
    sys.path.append(lib)
    import sitecustomize
# ------------------------------------------------------------------------------
# END BOOTSTRAP
# ------------------------------------------------------------------------------

import os
import re
import sys
import time
import errno
import commands
from glob import glob
import instinctual
from instinctual import settings
from instinctual.informer.client import Client, ClientConnectionError
from instinctual.informer.frame import Frame, FRAME_UPLOAD, FRAME_DELETE, FRAME_UNKNOWN

LOG = instinctual.getLogger('uploader')

class LogInfo(object):
    def write(self, msg):
        return LOG.info(msg)

class LogWarn(object):
    def write(self, msg):
        return LOG.warn(msg)

# ------------------------------------------------------------------------------
class Uploader(object):
    def __init__(self):
        conf = instinctual.getConf()
        thirdPartyBin = conf.get('informer', 'dir_third_party_bin')
        uploadDir = conf.get('informer', 'dir_uploads')

        self.pidfile = os.path.join(uploadDir, 'uploader.pid')
        self.frameGlob = os.path.join(uploadDir, '*', 'frame.pkl')
        self.convert = os.path.join(thirdPartyBin, 'convert')
        self.identify = os.path.join(thirdPartyBin, 'identify')

        self.maxWidth  = 1024
        self.maxHeight = 768
        self.identifyRegExp = re.compile(r'\s+(\d+)x(\d+)\s+')

        self.daemon = False
        self.flamePid = None

        for i in range(len(sys.argv)):
            if sys.argv[i] == '-P':
                self.flamePid = sys.argv[i+1]
            elif sys.argv[i] == '-D':
                self.daemon = True

    # ------------------------------------------------------------------------------
    def createPid(self):
        """
        create the pid
        """
        parent = os.path.dirname(self.pidfile)

        try:
            os.makedirs(parent)
            LOG.info("ok. made" + parent)
        except OSError, e:
            if errno.EEXIST != e.errno:
                LOG.fatal("Unable to create %s: %s" % (parent, e))
                raise e

        fd = open(self.pidfile, 'w')
        fd.write("%s\n" % os.getpid())
        fd.close()

        LOG.info("Uploader started with pid: %s" % os.getpid())

    # ------------------------------------------------------------------------------
    def removePid(self):
        os.unlink(self.pidfile)

    # ------------------------------------------------------------------------------
    def isPidfileRunning(self):
        """
        check for running process
        """
        try:
            fd = open(self.pidfile, 'r')
            oldpid = fd.readline()[:-1]
            cmdline = open('/proc/%s/cmdline' % oldpid, 'r').readline()

            if cmdline.find('python') != -1 and cmdline.find('uploader') != -1:
                LOG.info("Uploader already running with pid: %s" % (oldpid))
                return True
        except IOError, e:
            if errno.ENOENT != e.errno:
                LOG.fatal("Unable to determine if uploader running: %s" % (e))
                raise e

        return False

    # ------------------------------------------------------------------------------
    def isPidRunning(self, pid):
        try:
            os.stat('/proc/%s' % self.flamePid)
            return True
            # LOG.debug("Flame process (%s) still running..." % self.lamePid)
        except OSError, e:
            if e.errno == errno.ENOENT:
                # LOG.debug("Flame process (%s) NOT running." % self.flamePid)
                return False
            else:
                raise e

    # ------------------------------------------------------------------------------
    def run(self):
        if self.isPidfileRunning():
            sys.exit(0)

        if self.daemon == True:
            LOG.debug("running in daemon mode.")
            if os.fork():
                sys.exit(0)

            sys.stdout = LogInfo()
            sys.stderr = LogWarn()

        self.createPid()

        try:
            try:
                self.runLoop()
            except Exception, e:
                LOG.fatal("uploader exiting: %s" % (e))
        finally:
            self.removePid()

    # ------------------------------------------------------------------------------
    def runLoop(self):
        run = True
        tries = 0

        while run:
            count = self.uploadFrames()
            time.sleep(3.0)

            if not count:
                if self.flamePid and not self.isPidRunning(self.flamePid):
                    # LOG.debug("Flame appears to have exited...")
                    tries += 1
            else:
                tries = 0

            # Stop 5 mintues after work is done and flame has quit
            if tries > 100:
                # LOG.debug("No work to do and flame no longer running.")
                run = False

    # ------------------------------------------------------------------------------
    # do the work
    # ------------------------------------------------------------------------------
    def uploadFrames(self):
        count = 0
        matches = glob(self.frameGlob)
        matches.sort()

        for framePath in matches:
            count += 1

            LOG.debug("%s) Found -> %s" % (count, framePath))
            print("%s) Found -> %s" % (count, framePath))

            try:
                frame = Frame.load(framePath)
                self.uploadFrame(frame)
            except ClientConnectionError, e:
                LOG.debug("Client error encountered -- ignoring frame")
                frame.delete()
            except Exception, e:
                LOG.error("Error encountered trying to upload frame: %s" % e)
                if frame:
                    frame.delete()
            time.sleep(0.1)

        return count

    # ------------------------------------------------------------------------------
    def uploadFrame(self, frame):
        if frame.isBusy:
            LOG.debug("Frame was busy...")
        elif FRAME_DELETE == frame.status:
            frame.delete()
        elif FRAME_UPLOAD == frame.status:
            LOG.debug("I can work on: %s" % frame)
            #frame.isBusy = True
            #frame.save()

            par = 100 * float("%.5f" % frame.pixelAspectRatio)
            frame.resizedPath = os.path.join(frame.container, 'frame.jp2')

            cmd = '%s -quality 90 %s -resize "%s%%x100%%" -resize "%sx%s>" %s'
            cmd = cmd % (self.convert, frame.rgbPath, par, self.maxWidth, self.maxHeight, frame.resizedPath)
            LOG.debug("CMD:" + cmd)

            if 0 == os.system(cmd):
                cmd = '%s %s' % (self.identify, frame.resizedPath)
                (result, output) = commands.getstatusoutput(cmd)
                if 0 == result:
                    match = self.identifyRegExp.search(output)
                    if match != None:
                        frame.resizedDepth = 8
                        frame.resizedWidth = match.group(1)
                        frame.resizedHeight = match.group(2)
                        LOG.info("MATCHED (%s) X (%s)!" % (frame.resizedWidth, frame.resizedHeight))

                        LOG.info("FRAME (%s) STATUS (%s)" % (frame, frame.status))
                        client = Client()
                        client.createFrame(frame)
                        LOG.debug("done!")
                        frame.delete()
                    else:
                        raise ValueError("DID NOT MATCH identify group!")
                else:
                    raise ValueError("identify did not return 0!")
            else:
                raise ValueError("FAILURE running convert")
        elif FRAME_UNKNOWN == frame.status:
            # unknown frame -- check if the pid that generated it
            # is still running. If not, it can be deleted
            if not self.isPidRunning(frame.pid):
                LOG.info("The pid (%s) that generated (%s) is no longer running" % (frame.pid, frame))
                frame.delete()
