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
import sys
import errno
import tempfile
import commands
from pprint import pprint
from datetime import datetime

import instinctual
from instinctual import settings
from instinctual.informer.models import *

LOG = instinctual.getLogger(__file__)

class LogInfo(object):
    def write(self, msg):
        return LOG.info(msg)

class LogWarn(object):
    def write(self, msg):
        return LOG.warn(msg)

sys.stdout = LogInfo()
sys.stderr = LogWarn()

ROOT = os.sep.join(os.path.abspath(__file__).split(os.sep)[:-5])
TMPDIR  = ROOT + '/tmp'
PENDING = ROOT + '/media/pending.tiff'
FFMPEG  = ROOT + '/third_party/bin/ffmpeg'
QT_FAST = ROOT + '/third_party/bin/qt-faststart'
CONVERT = ROOT + '/third_party/bin/convert'

def main():
    # get the lock
    lockfile = TMPDIR + '/lockfile'

    try:
        MKDIR(TMPDIR)
        flag = os.O_CREAT | os.O_EXCL | os.O_WRONLY
        fd = os.open(lockfile, flag, 0600)
        os.write(fd, str(os.getpid()))
        os.close(fd)
    except OSError, e:
        LOG.info("Unable to acquire lock.")
        sys.exit(1)
    except Exception, e:
        LOG.fatal("Unable to acquire lock: unknown error: %s" % e)
        sys.exit(1)

    try:
        try:
            # get the renders that need updating
            render_ids = Frame.objects.filter(in_render=False).values('render').distinct()
            LOG.info("Lock acquired: %s renders to process..." % len(render_ids))

            for render_id in render_ids:
                try:
                    makerender(render_id)
                except Exception, e:
                    LOG.error(str(e))

        except Exception, e:
            LOG.fatal(str(e))
            sys.exit(1)
    finally:
        LOG.info("removing lockfile")
        os.unlink(lockfile)

def makerender(render_id):
    render = Render.objects.get(id=render_id['render'])
    LOG.info("+ working on render id %s" % render.id)

    # grab info about the frames that need processing
    max = 0
    height = None
    width = None
    frames = {}
    for frame in Frame.objects.filter(render=render):
        frame.in_render = True
        frames[frame.number] = frame
        height = frame.resized_height
        width  = frame.resized_width
        if not max or frame.number > max:
            max = frame.number

    # create the temp workspace
    MKDIR(TMPDIR)

    prefix = "%s-%s-%s-" % ('moviemaker', datetime.now().strftime("%Y.%m.%d-%H.%M.%S"), render.id)
    workspace = tempfile.mkdtemp('', prefix, TMPDIR)
    os.chmod(workspace, 0750)
    os.chdir(workspace)

    # create a pending frame to match the size of the resized frames
    pending = workspace + '/' + 'pending.tiff'
    cmd = "%s %s -resize '%sx%s!' -channel RGB -depth 8 -compress RLE %s" % (CONVERT, PENDING, width, height, pending)
    SYSTEM(cmd)

    pattern = workspace + '/' + '%06d.tiff'
    for number in range(1, max + 1):
        if number in frames:
            source = frames[number].get_image_filename()
            if not os.path.exists(source):
                print "WARN: frame %s is in database but not on disk" % (number)
                source = pending
        else:
            print "WARN: frame %s is missing" % (number)
            source = pending

        if not source.endswith('.tiff'):
            # convert the image to tiff
            outfile = source[:source.rfind('.')] + '.tiff'
            cmd = "%s -channel RGB -depth 8 -compress RLE %s %s" % (CONVERT, source, outfile)
            SYSTEM(cmd)

            # update the database
            image = frames[number].image
            frames[number].image = image[:image.rfind('.')] + '.tiff'
            frames[number].save()

            # remove orginal source, use the tiff from here on out
            UNLINK(source)
            source = outfile

            print "Frame [%s] %s" % (number, frames[number].get_image_filename())

        dest = pattern % (number)
        os.symlink(source, dest)

    # --------------------------------------------------
    # create the hi rez movie
    slow_hi = workspace + '/' + 'slow-hi.mov'
    print "pass hi 1"
    cmd = "%s -r %s -f image2 -vcodec tiff -i %s -threads auto -vcodec libx264 -subq 10 -refs 6 -me umh -me_range 20 -partitions +parti4x4+parti8x8+partp8x8+partp4x4+partb8x8 -qcomp .7 -g 75 -keyint_min 25 -bf 3 -flags +loop+umv -psnr -flags2 +bpyramid+mixed_refs+dct8x8 -coder 1 -cmp +chroma -sc_threshold 40 -i_qfactor .7 -b 2600k -bt 3500k -b_strategy 1 -trellis 1 -rc_eq 'blurCplx^(1-qComp)' -qmin 5 -qmax 35 -qdiff 2 -qscale 1 -pass 1 -y %s" % (FFMPEG, render.rate, pattern, slow_hi)
    SYSTEM(cmd)

    print "pass hi 2"
    cmd = "%s -r %s -f image2 -vcodec tiff -i %s -threads auto -vcodec libx264 -subq 10 -refs 6 -me umh -me_range 20 -partitions +parti4x4+parti8x8+partp8x8+partp4x4+partb8x8 -qcomp .7 -g 75 -keyint_min 25 -bf 3 -flags +loop+umv -psnr -flags2 +bpyramid+mixed_refs+dct8x8 -coder 1 -cmp +chroma -sc_threshold 40 -i_qfactor .7 -b 2600k -bt 3500k -b_strategy 1 -trellis 1 -rc_eq 'blurCplx^(1-qComp)' -qmin 5 -qmax 35 -qdiff 2 -qscale 1 -pass 2 -y %s" % (FFMPEG, render.rate, pattern, slow_hi)
    SYSTEM(cmd)

    # qt-faststart it
    fast_hi = workspace + '/' + 'fast-hi.mov'
    cmd = "%s %s %s" % (QT_FAST, slow_hi, fast_hi)
    SYSTEM(cmd)

    # --------------------------------------------------
    # create the lo rez movie
    slow_lo = workspace + '/' + 'slow-lo.mov'
    print "pass lo 1"
    cmd = "%s -r %s -f image2 -vcodec tiff -i %s -threads auto -vcodec libx264  -subq 10 -refs 6 -me umh -me_range 20 -partitions +parti4x4+parti8x8+partp8x8+partp4x4+partb8x8 -qcomp 1 -g 2 -keyint_min 25 -bf 3 -flags +loop+umv -psnr -flags2 +bpyramid+mixed_refs+dct8x8 -coder 1 -cmp +chroma -sc_threshold 40 -i_qfactor 1 -b 800k -bt 1150k -b_strategy 1 -trellis 1 -rc_eq 'blurCplx^(1-qComp)' -qmin 10 -qmax 35 -qdiff 2 -qscale 1 -s 544x304 -pass 1 -y %s" % (FFMPEG, render.rate, pattern, slow_lo)
    SYSTEM(cmd)

    print "pass lo 2"
    cmd = "%s -r %s -f image2 -vcodec tiff -i %s -threads auto -vcodec libx264  -subq 10 -refs 6 -me umh -me_range 20 -partitions +parti4x4+parti8x8+partp8x8+partp4x4+partb8x8 -qcomp 1 -g 2 -keyint_min 25 -bf 3 -flags +loop+umv -psnr -flags2 +bpyramid+mixed_refs+dct8x8 -coder 1 -cmp +chroma -sc_threshold 40 -i_qfactor 1 -b 800k -bt 1150k -b_strategy 1 -trellis 1 -rc_eq 'blurCplx^(1-qComp)' -qmin 10 -qmax 35 -qdiff 2 -qscale 1 -s 544x304 -pass 2 -y %s" % (FFMPEG, render.rate, pattern, slow_lo)
    SYSTEM(cmd)

    # qt-faststart it
    fast_lo = workspace + '/' + 'fast-lo.mov'
    cmd = "%s %s %s" % (QT_FAST, slow_lo, fast_lo)
    SYSTEM(cmd)

    # --------------------------------------------------
    # copy the movies...
    etime = render.created_on
    setup = os.path.basename(render.setup)
    basename = "%s_%s" % (setup, etime.strftime("%H%M"))

    if not render.is_pending:
        UNLINK(render.get_movie_hi_filename())
    f = open(fast_hi, 'rb')
    render.save_movie_hi_file('%s.mov' % basename, f.read(), True)

    if not render.is_pending:
        UNLINK(render.get_movie_lo_filename())
    f = open(fast_lo, 'rb')
    render.save_movie_lo_file('%s_lo.mov' % basename, f.read(), True)

    render.is_pending = False
    render.save()

    # save it
    for frame in frames.values():
        frame.in_render = True
        frame.save()
        pass

    # cleanup
    RMDIR(workspace)

# ------------------------------------------------------------------------------
# utility functions
# ------------------------------------------------------------------------------
def UNLINK(path):
    try:
        os.unlink(path)
    except OSError, e:
        if errno.ENOENT != e.errno:
            raise e

def RMDIR(path):
    for entry in os.listdir(path):
        child = os.path.join(path, entry)
        if os.path.isdir(child):
            RMDIR(child)
        else:
            UNLINK(child)
    try:
        os.rmdir(path)
    except OSError, e:
        if errno.ENOENT != e.errno:
            raise e

def SYSTEM(cmd):
    print cmd
    (result, output) = commands.getstatusoutput(cmd)
    print output

    if 0 != result:
        msg = "Non-Zero (%s) status returned from %s:" % (result, cmd)
        print msg
        raise ValueError(msg + "\n---- BEGIN ----\n%s\n---- END ----" % output)

def MKDIR(dir):
    try:
        os.mkdir(dir)
    except OSError, e:
        if errno.EEXIST != e.errno:
            raise e

# ------------------------------------------------------------------------------
class MovieMaker(object):
    def run(self):
        main()
