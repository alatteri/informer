import os
import time


from instinctual.informer.event import DiscreetEvent

class Subject(object):
    def __init__(self):
        print "__init__ called for Subject()"
        self._observers = list()

    def registerObserver(self, observer):
        print "registerObserver()"
        self._observers.append(observer)

    def notifyObservers(self, event):
        # print "notifyObserver()"
        for observer in self._observers:
            observer.notify(event)

class LogSubject(Subject):
    def __init__(self, path):
        self._path = path
        self._handle = None
        super(LogSubject, self).__init__()

    def operate(self):
        print "Now watching %s for input" % (self._path)
        previous = None
        self._handle = self._open()

        while (1):
            lines = list()
            now = os.stat(self._path)
            if previous and now.st_ino != previous.st_ino:
                # make sure there is nothing else to read
                lines = self._read()

                print "---- RESET ----"
                self._handle.close()
                self._handle = self._open()
                previous = now
            elif not previous or now.st_mtime > previous.st_mtime:
                # process lines
                lines = self._read()
                previous = now
            else:
                time.sleep(1)

            for line in lines:
                event = self.event(line)
                self.notifyObservers(event)

    def _open(self):
        return open(self._path, 'r')

    def _read(self):
        print "---- one readline ---"
        lines = self._handle.readlines(100*1024)
        return lines


class DiscreetLogSubject(LogSubject):
    def event(self, line):
        return DiscreetEvent(line)

