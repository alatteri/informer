import re
import instinctual.informer

LOG = instinctual.informer.getLogger(__name__)

class Observer(object):
    def __init__(self, callback):
        self._callback = callback

    def process(self, event):
        return None

    def callback(*args, **kwargs):
        self._callback(*args, **kwargs)

    def notify(self, event):
        name = self.__class__.__name__
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
    pass

class DiscreetLoadProject(DiscreetObserver):
    def process(self, event):
        if event.category == 'BUTTON':
            if event.message == "[Start] PrjLoadFirstSelection":
                return {}

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

#class DiscreetProcess(DiscreetObserver):
#
