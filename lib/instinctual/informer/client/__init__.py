#!/usr/bin/env python

import restclient
from xml.dom import minidom, Node

import instinctual
from instinctual import informer
from instinctual.informer.client.appevent import *

LOG = instinctual.getLogger(__name__)

# ------------------------------------------------------------------------------
class Client(object):
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

