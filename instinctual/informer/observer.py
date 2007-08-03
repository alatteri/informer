class Observer(object):
    def __init__(self, name):
        self._name = name

    def notify(self, event):
        print "Observer %s received event [%s]" % (self._name, event)

