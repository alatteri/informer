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
from datetime import datetime, timedelta

import instinctual
from instinctual import settings
from instinctual.informer.models import *

LOG = instinctual.getLogger(__file__)

# ------------------------------------------------------------------------------
class _LogInfo(object):
    def write(self, msg):
        return LOG.info(msg)

class _LogWarn(object):
    def write(self, msg):
        return LOG.warn(msg)

def LogOutput(fn):
    def _wrap_LogOutput(*args, **kwargs):
        sys.stdout = _LogInfo()
        sys.stderr = _LogWarn()
        return fn(*args, **kwargs)
    return _wrap_LogOutput

# ------------------------------------------------------------------------------
class UtilApp(object):
    def main(self):
        pass

    def run(self):
        try:
            self.main()
        except Exception, e:
            LOG.fatal(str(e))
            sys.exit(1)

# ------------------------------------------------------------------------------
class FrameCleaner(UtilApp):
    def main(self):
        # get the old frames
        too_old = datetime.now() - timedelta(days=1)
        frames = Frame.objects.filter(created_on__lt=too_old)
        try:
            for f in frames:
                print "Removing frame[%s]: %s" % (f.id, f.get_image_filename())
                f.delete()
        except Exception, e:
            LOG.error(str(e))
    main = LogOutput(main)
