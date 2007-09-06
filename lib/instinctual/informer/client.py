#!/usr/bin/python

import restclient
from datetime import datetime
from xml.dom import minidom, Node

import instinctual
from instinctual import informer

LOG = instinctual.getLogger(__name__)

# ------------------------------------------------------------------------------
class AppEvent(object):
    def __init__(self, event=None):
        self.date = None
        self.shot = None
        self.user = None
        self.event = None
        self.setup = None
        self.volume = None
        self.outputs = None
        self.project = None
        self.hostname = None

        self.now = datetime.now().strftime("%m/%d/%y:%H:%M:%S.000")

        if event is not None:
            self._initFromEvent(event)

    def _initFromEvent(self, event):
        self.date = event.date
        self.shot = event.shot
        self.user = event.user
        self.event = event.event
        self.setup = event.setup
        self.volume = event.volume
        self.project = event.project
        self.hostname = event.hostname

        if event.__class__.__name__ == 'DiscreetAppBatchProcessEvent':
            self.outputs = event.outputs

    def asDict(self):
        data = {}
        data['now'] = self.now
        data['date'] = self.date
        data['shot'] = self.shot
        data['user'] = self.user
        data['event'] = self.event
        data['setup'] = self.setup
        data['volume'] = self.volume
        if self.outputs: data['outputs'] = '|'.join(self.outputs)
        data['project'] = self.project
        data['hostname'] = self.hostname
        return data

# ------------------------------------------------------------------------------
class Client(object):
    def __init__(self):
        pass

    def postAppEvent(self, appEvent, async=False):
        appEventUrl = instinctual.informer.getAppEventUrl()
        LOG.info(restclient.POST(appEventUrl, appEvent.asDict(), async=async))

    def getNotes(self, setup):
        parsed = informer.parseSetup(setup)
        project = parsed['project']
        shot = parsed['shot']
        url = informer.getProjectShotNotesUrl(project, shot)
        xml = restclient.GET(url)
        print "Got xml:", xml
        pass

    def getElements(self, setup):
        parsed = informer.parseSetup(setup)
        project = parsed['project']
        shot = parsed['shot']
        url = informer.getProjectShotElementsUrl(project, shot)
        xml = restclient.GET(url)
        print "Got xml:", xml
        pass
