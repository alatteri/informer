import re
import instinctual.informer

LOG = instinctual.informer.getLogger(__name__)

class Event(object):
    def __init__(self, event):
        self.event = event

class DiscreetEvent(Event):
    _reLog = re.compile(r'^\[(\w+)\]\s+(\d+)\s+([\w\.]+):(\d+)\s+([^\s]+)\s+(.*)$')
    _reExtra = re.compile(r'(\w+)\s*:\s*(.*)$')

    def __init__(self, event):
        super(DiscreetEvent, self).__init__(event)
        # print "new event created for [%s]" % (event)

        self.level = ''
        self.pid = ''
        self.file = ''
        self.line = ''
        self.date = ''
        self.category = ''
        self.message = ''

        self._parse()
        LOG.debug(self)

    def _parse(self):
        match = self._reLog.search(self.event)
        if match != None:
            # print "PARSED!"
            self.level  = match.group(1)
            self.pid    = match.group(2)
            self.file   = match.group(3)
            self.line   = match.group(4)
            self.date   = match.group(5)
            # (self.level, self.pid, self.file, self.line) = match.group(1,2,3,4)

            extra = match.group(6)
            match = self._reExtra.search(extra)
            if match != None:
                self.category = match.group(1)
                self.message = match.group(2)
            else:
                self.message = extra
        elif '# ' == self.event[0:2]:
            LOG.debug("Found comment [%s]" % (self.event))
            self.message = self.event
            self.category = 'COMMENT'
        else:
            LOG.debug("Could not parse[%s]" % (self.event))
            self.message = self.event

    def __repr__(self):
        return " ".join(("LEVEL:", self.level,
                         "PID:", self.pid,
                         "FILE:", self.file,
                         "LINE:", self.line,
                         "DATE:", self.date,
                         "CATEGORY:", self.category,
                         "MESSAGE:", self.message))

