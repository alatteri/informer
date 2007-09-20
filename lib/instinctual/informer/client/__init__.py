#!/usr/bin/env python

# The following line is needed to eval(data) from python requests
import datetime

import restclient
from xml.dom import minidom, Node

import instinctual
from instinctual import informer
from instinctual.informer.client.appevent import AppEvent

LOG = instinctual.getLogger(__name__)

class ClientConnectionRefused(Exception):
    pass

# ------------------------------------------------------------------------------
class Client(object):
    def postAppEvent(self, appEvent):
        appEventUrl = instinctual.informer.getAppEventUrl()
        data = appEvent.asDict()
        result = self.POST(appEventUrl, data)
        LOG.info(result)

    def getNotes(self, setup):
        parsed = informer.parseSetup(setup)
        project = parsed['project']
        shot = parsed['shot']
        url = informer.getProjectShotNotesUrl(project, shot)
        return self.GET(url)

    def getElements(self, setup):
        parsed = informer.parseSetup(setup)
        project = parsed['project']
        shot = parsed['shot']
        url = informer.getProjectShotElementsUrl(project, shot)
        return self.GET(url)

    def putNote(self, setup, data):
        parsed = informer.parseSetup(setup)
        project = parsed['project']
        shot = parsed['shot']
        pk = data['pk']
        del data['pk']
        url = informer.getProjectShotNoteUrl(project, shot, pk)
        print "Hey the url is: %s" % (url)
        self.POST(url, data)

    def GET(self, url):
        data = restclient.GET(url)
        data = eval(data) # whoa! scary!

        if len(data) == 2:
            if data[0] == 111:
                raise ClientConnectionRefused("%s %s: %s" % (url, data[0], data[1]))
        return data

    def PUT(self, url, data):
        print "yeah!"
        result = restclient.PUT(url, data, async=False)
        print result

    def POST(self, url, data):
        print "now calling [%s] with (%s)" % (url, data)
        result = restclient.POST(url, data, async=False)
        print result
