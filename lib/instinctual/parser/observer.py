import re
import new
import instinctual

LOG = instinctual.getLogger(__name__)

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

class DiscreetSaveSetup(DiscreetObserver):
    _re = re.compile(r'Saving\s+setup\s+(.+)\.batch\.')
    def process(self, event):
        if event.category == 'BATCH':
            match = self._re.search(event.message)
            if match != None:
                return {'setup': match.group(1)}

class DiscreetBatchProcess(DiscreetObserver):
    def process(self, event):
        if event.category == 'BUTTON':
            if event.message == '[Process] BatchProcess':
                return {}

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
