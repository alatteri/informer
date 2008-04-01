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
from instinctual.informer.frame import Frame, FRAME_UPLOAD, FRAME_DELETE

LOG = instinctual.getLogger('uploader')

class LogInfo(object):
    def write(self, msg):
        return LOG.info(msg)

class LogWarn(object):
    def write(self, msg):
        return LOG.warn(msg)

# ------------------------------------------------------------------------------
# really... someday take the time and clean this up
# ------------------------------------------------------------------------------
def main():
    conf = instinctual.getConf()
    uploadDir = conf.get('informer', 'dir_uploads')
    frameGlob = os.path.join(uploadDir, '*', 'frame.pkl')

    thirdPartyBin = conf.get('informer', 'dir_third_party_bin')
    convert = os.path.join(thirdPartyBin, 'convert')
    identify = os.path.join(thirdPartyBin, 'identify')

    daemon = False
    flamePid = None

    for i in range(len(sys.argv)):
        if sys.argv[i] == '-P':
            flamePid = sys.argv[i+1]
        elif sys.argv[i] == '-D':
            daemon = True

    if daemon == True:
        LOG.debug("running in daemon mode.")
        if os.fork():
            sys.exit(0)

        sys.stdout = LogInfo()
        sys.stderr = LogWarn()

    # ------------------------------------------------------------------------------
    # check for running process
    # ------------------------------------------------------------------------------
    pidfile = os.path.join(uploadDir, 'uploader.pid')

    try:
        fd = open(pidfile, 'r')
        oldpid = fd.readline()[:-1]
        cmdline = open('/proc/%s/cmdline' % oldpid, 'r').readline()

        if cmdline.find('python') != -1 and cmdline.find('uploader') != -1:
            LOG.info("Uploader already running with pid: %s" % (oldpid))
            sys.exit(0)
    except IOError, e:
        if errno.ENOENT != e.errno:
            LOG.fatal("Unable to determine if uploader running: %s" % (e))
            raise e

    # --------------------
    # create the pid
    #
    try:
        os.makedirs(uploadDir)
        LOG.info("ok. made" + uploadDir)
    except OSError, e:
        if errno.EEXIST != e.errno:
            LOG.fatal("Unable to create %s: %s" % (uploadDir, e))
            raise e

    fd = open(pidfile, 'w')
    fd.write("%s\n" % os.getpid())
    fd.close()

    LOG.info("Uploader started with pid: %s" % os.getpid())

    # ------------------------------------------------------------------------------
    # do the work
    # ------------------------------------------------------------------------------
    tries = 0
    maxWidth  = 1024
    maxHeight = 768
    identifyRegExp = re.compile(r'\s+(\d+)x(\d+)\s+')

    try:
        run = True
        tries = 0
        while run:
            count = 0
            matches = glob(frameGlob)
            matches.sort()
            for framePath in matches:
                count += 1
                LOG.debug("%s) Found -> %s" % (count, framePath))
                print("%s) Found -> %s" % (count, framePath))
                frame = Frame.load(framePath)

                if frame.isBusy:
                    LOG.debug("Frame was busy...")
                elif FRAME_DELETE == frame.status:
                    LOG.debug("Deleting frame:" + frame.rgbPath)
                    frame.delete()
                elif FRAME_UPLOAD == frame.status:
                    LOG.debug("I can work on:" + frame.rgbPath)
                    #frame.isBusy = True
                    #frame.save()

                    par = 100 * float("%.5f" % frame.pixelAspectRatio)
                    frame.resizedPath = os.path.join(frame.container, 'frame.jp2')

                    cmd = '%s -quality 90 %s -resize "%s%%x100%%" -resize "%sx%s>" -channel RGB -compress RLE %s'
                    cmd = cmd % (convert, frame.rgbPath, par, maxWidth, maxHeight, frame.resizedPath)
                    print "CMD is:", cmd
                    LOG.debug("CMD:" + cmd)

                    if 0 == os.system(cmd):
                        cmd = '%s %s' % (identify, frame.resizedPath)
                        (result, output) = commands.getstatusoutput(cmd)
                        if 0 == result:
                            match = identifyRegExp.search(output)
                            if match != None:
                                frame.resizedDepth = 8
                                frame.resizedWidth = match.group(1)
                                frame.resizedHeight = match.group(2)
                                LOG.info("MATCHED (%s) X (%s)!" % (frame.resizedWidth, frame.resizedHeight))
                                try:
                                    LOG.info("FRAME (%s) STATUS (%s)" % (framePath, frame.status))
                                    client = Client()
                                    client.createFrame(frame)
                                    LOG.debug("done!")
                                    frame.delete()
                                    pass
                                except ClientConnectionError, e:
                                    LOG.debug("Client error encountered -- ignoring frame")
                                    frame.delete()
                            else:
                                LOG.warn("DID NOT MATCH identify group!")
                        else:
                            LOG.warn("identify did not return 0!")
                    else:
                        LOG.warn("FAILURE running convert")
                        frame.delete()

                time.sleep(0.1)

            if flamePid:
                try:
                    os.stat('/proc/%s' % flamePid)
                    tries = 0
                    # LOG.debug("Flame process (%s) still running..." % flamePid)
                except OSError, e:
                    if e.errno == errno.ENOENT:
                        LOG.debug("Flame process (%s) NOT running." % flamePid)
                        tries += 1
                    else:
                        raise e

            if tries > 200:     # 10 minutes = 200 * 3.0 / 60
                LOG.info("TIMEOUT: Flame appears to have exited. Quitting.")
                run = False
            else:
                # LOG.debug("now sleeping... (flame pid: %s)" % flamePid)
                time.sleep(3.0)

    except e:
        LOG.fatal("uploader died: %s" % (e))
        os.unlink(pidfile)
        sys.exit(1)

    os.unlink(pidfile)

# ------------------------------------------------------------------------------
class Uploader(object):
    def run(self):
        main()
