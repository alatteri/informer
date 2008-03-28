import re
import new
import instinctual

LOG = instinctual.getLogger(__name__)

# [notice] 2554867904 menu.C:3103 03/26/08:21:54:35.698 BUTTON : [Burn] BatchBurnProcessMenu
# [notice] 2554867904 messageAccumulator.C:268 03/26/08:21:54:35.700 BATCH : WARNING: Processing in proxy mode. Render this setup remotely?
# [notice] 2554867904 messageAccumulator.C:268 03/26/08:21:55:11.975 BATCH : Burn job 'Burn_flame2_080326_21.55.10' sent.
# [notice] 2554867904 BatchBurnIFFFS.C:797 03/26/08:21:55:11.975 BURN_SUBMIT_JOB: JOB_ID=Burn_flame2_080326_21.55.10 BATCH_SETUP_NAME=test-01
# {'GROUP': 'root', 'HOSTTYPE': 'x86_64-linux', 'DL_VOLUME_LOCK_RETRY': '600', 'LOGNAME': 'root', 'USER': 'root', 'PATH': '/sbin:/usr/sbin:/bin:/usr/bin:/usr/X11R6/bin', 'VENDOR': 'unknown', 'BURN_HOME': '/usr/discreet/burn_2008.SP1', '__GL_SYNC_TO_VBLANK': '1', 'DL_PROJECT_LOCK_RETRY': '600', 'previous': 'N', 'LANG': 'en_US.UTF-8', 'TERM': 'linux', 'BURN_SYSFILE': '/usr/local/flexlm/licenses/burn', 'DL_MASTER_BYPASS': '', 'SW_NUM_DISK_IO_PROCS': '2', 'SHLVL': '3', 'PREVLEVEL': 'N', 'DISPLAY': ':0.0', 'LIBQUICKTIME_PLUGIN_DIR': '/usr/discreet/burn_2008.SP1/lib64', 'DL_ARCH_LOCK_RETRY': '2000', 'DL_VOLUME_MGR_CACHE_NB_TRIES': '600', 'runlevel': '5', 'OSTYPE': 'linux', 'DL_FSQUEUE_IO_TIMEOUT': '0', 'HOME': '/', 'acpi': 'off', 'DL_DEFAULT_FRAMESTORE': 'stonefs/flame2', 'SELINUX_INIT': 'YES', 'CONSOLE': '/dev/console', 'SW_DISK_IO_PRIO_LEVEL': '90', 'DL_AUDIO_METERS_OFF': '1', 'HOST': 'flame2', 'BURN_CONFIG': '/usr/discreet/burn_2008.SP1/cfg', '_': '/usr/discreet/backburner/backburnerServer', 'DL_FILE_LOCK_DELAY': '50', 'DL_DB_LOCK_RETRY': '600', 'PWD': '/', 'DJANGO_SETTINGS_MODULE': 'instinctual.settings', 'DL_NO_IN_MODULE_READ_AHEAD': '', 'RUNLEVEL': '5', 'MACHTYPE': 'x86_64', 'INIT_VERSION': 'sysvinit-2.85'}

class Observer(object):
    def __init__(self, callback=None):
        self._callback = callback

    def process(self, event):
        return None

    def notify(self, event):
        name = self.__class__.__name__
        # print "MESSAGE [%s] CATEGORY [%s]" % (event.message, event.category)
        data = self.process(event)
        if data != None:
            LOG.debug("[[[ %s event matched: %s ]]]" % (name, data))
            if self._callback:
                LOG.debug('=' * 80)
                data['event'] = event
                self._callback(**data)
                LOG.debug('=' * 80)
            else:
                LOG.warn("No callback defined for Observer %s" % (name))

class DiscreetObserver(Observer):
    def process(self, event):
        return {'event': event}

class DiscreetTimedMessage(DiscreetObserver):
    def process(self, event):
        if event.date:
            return {'date': str(event.date)}

class DiscreetSpecifyHostname(DiscreetObserver):
    _re = re.compile(r'#\s+Hostname\s+\-\s+(.+)')
    def process(self, event):
        if event.category == 'COMMENT':
            match = self._re.search(event.message)
            if match != None:
                return {'hostname': match.group(1)}

class DiscreetSpecifyProject(DiscreetObserver):
    _re = re.compile(r'^Project\s+\((.+)\)')
    def process(self, event):
        if event.category == 'PRJMGT':
            match = self._re.search(event.message)
            if match != None:
                return {'project': match.group(1)}

class DiscreetSpecifyVolume(DiscreetObserver):
    _re = re.compile(r'^Volume\s+\((.+)\)')
    def process(self, event):
        if event.category == 'PRJMGT':
            match = self._re.search(event.message)
            if match != None:
                return {'volume': match.group(1)}

class DiscreetSpecifyUser(DiscreetObserver):
    _re = re.compile(r'^User\s+\((.+)\)')
    def process(self, event):
        if event.category == 'PRJMGT':
            match = self._re.search(event.message)
            if match != None:
                return {'user': match.group(1)}

class DiscreetLoadSetup(DiscreetObserver):
    _re = re.compile(r'Loading\s+setup\s+(.+)\.batch\.')
    def process(self, event):
        if event.category == 'BATCH':
            match = self._re.search(event.message)
            if match != None:
                return {'setup': match.group(1)}

class DiscreetLoadInformerSetup(DiscreetObserver):
    _re = re.compile(r'Loading\s+setup\s+(.+)\.Informer\.')
    def process(self, event):
        if event.category == 'BATCH':
            match = self._re.search(event.message)
            if match != None:
                return {'setup': match.group(1)}

class DiscreetSaveSetup(DiscreetObserver):
    _re = re.compile(r'Saving\s+setup\s+(.+)\.batch\.')
    def process(self, event):
        if event.category == 'BATCH':
            match = self._re.search(event.message)
            if match != None:
                return {'setup': match.group(1)}

class DiscreetBatchProcess(DiscreetObserver):
    # support 2009 style 
    # [notice] 2600011520 menu.C:3121 03/14/08:08:30:53.727 BUTTON : [Process] BatchProcessButton
    def process(self, event):
        if event.category == 'BUTTON':
            if event.message.startswith('[Process] BatchProcess'):
                return {}

class DiscreetBurnProcess(DiscreetObserver):
    _re = re.compile(r'JOB_ID=(.+)\s+')
    def process(self, event):
        if event.category == 'BURN_SUBMIT_JOB':
            match = self._re.search(event.message)
            if match != None:
                return {'job': match.group(1)}

class DiscreetEnterSparkModule(DiscreetObserver):
    def process(self, event):
        if event.category == 'PUSH MENU':
            if event.message == 'Batch to SparkModule':
                return {}

class DiscreetBatchProcessOutput(DiscreetObserver):
    _re = re.compile(r'Processing\s(.+)\.\s+\d+\s+frames\.')
    def process(self, event):
        if event.category == 'BATCH':
            match = self._re.search(event.message)
            if match != None:
                return {'output': match.group(1)}

class DiscreetBatchPlay(DiscreetObserver):
    def process(self, event):
        if event.category == 'BUTTON':
            if event.message == '[Player] BatchPlay':
                return {}

class DiscreetBatchEnd(DiscreetObserver):
    def process(self, event):
        if event.category == 'BUTTON':
            if event.message == '[EXIT Batch] BatchEnd':
                return {}

class DiscreetAppExit(DiscreetObserver):
    def process(self, event):
        if event.category == 'VOLUMEMGT':
            if event.message == 'Uninitialising.':
                return {}
