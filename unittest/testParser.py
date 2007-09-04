import unittest

from instinctual.parser.subject import DiscreetAppSubject
from instinctual.parser.observer import Observer

# ------------------------------------------------------------------------------
class DiscreetAppEvent(object):
    def __init__(self,
                 project,
                 shot,
                 setup,
                 hostname,
                 volume,
                 user,
                 date,
                 event,
                 outputs=[]):

        self.project    = project
        self.shot       = shot
        self.setup      = setup
        self.hostname   = hostname
        self.volume     = volume
        self.user       = user
        self.date       = date
        self.event      = event
        self.outputs    = outputs

# ------------------------------------------------------------------------------
class MyObserver(Observer):
    def process(self, event):
        testcase = self._testcase
        expected = self._getNextEvent()

        attrs = ['project', 'shot', 'setup', 'hostname', 'volume', 'user',
                 'date', 'event', 'outputs',]
        for attr in attrs:
            testcase.assertEqual(getattr(expected, attr), getattr(event, attr))

    def _setEvents(self, events):
        self._events = events

    def _setTestCase(self, testcase):
        self._testcase = testcase

    def _getNextEvent(self):
        return self._events.pop(0)

    def _getEventCount(self):
        return len(self._events)

# ------------------------------------------------------------------------------
class DiscreetLogTestCase(unittest.TestCase):
    def assertLogEvents(self, logfile, events=[]):
        observer = MyObserver()
        observer._setEvents(events)
        observer._setTestCase(self)

        app = DiscreetAppSubject(logfile, wait=False)
        app.registerObserver(observer)
        app.operate()

        # make sure there aren't any left over events
        self.assertEqual(0, observer._getEventCount())

    def testLogSample1(self):
        """
        The hello world of Discreet log files
        consists of 3 events: LOAD SETUP, BATCH PROCESS and a SAVE SETUP
        """
        events = [
            DiscreetAppEvent(
                project     = 'Informer',
                shot        = 'jfr',
                setup       = '/usr/discreet/project/Informer/batch/jfr',
                hostname    = 'discreet',
                volume      = 'stonefs',
                user        = 'Alan',
                date        = '08/01/07:21:07:45.077',
                event       = 'LOAD SETUP',
            ),
            DiscreetAppEvent(
                project     = 'Informer',
                shot        = 'jfr',
                outputs     = ['output4'],
                setup       = '/usr/discreet/project/Informer/batch/jfr',
                hostname    = 'discreet',
                volume      = 'stonefs',
                user        = 'Alan',
                date        = '08/01/07:21:07:50.196',
                event       = 'BATCH PROCESS',
            ),
            DiscreetAppEvent(
                project     = 'Informer',
                shot        = 'jfr',
                setup       = '/usr/discreet/project/Informer/batch/jfr',
                hostname    = 'discreet',
                volume      = 'stonefs',
                user        = 'Alan',
                date        = '08/01/07:21:08:05.361',
                event       = 'SAVE SETUP',
            ),
        ]

        self.assertLogEvents('sample1.log', events)

    def testLogSample2(self):
        pass

if __name__ == '__main__':
    unittest.main()
