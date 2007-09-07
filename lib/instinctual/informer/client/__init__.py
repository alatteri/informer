#!/usr/bin/env python

# The following line is needed to eval(data) from python requests
import datetime

import restclient
from xml.dom import minidom, Node

import instinctual
from instinctual import informer
from instinctual.informer import TYPE_XML, TYPE_PYTHON
from instinctual.informer.client.appevent import AppEvent

LOG = instinctual.getLogger(__name__)

# ------------------------------------------------------------------------------
class Client(object):
    def postAppEvent(self, appEvent, async=False):
        appEventUrl = instinctual.informer.getAppEventUrl(TYPE_XML)
        LOG.info(restclient.POST(appEventUrl, appEvent.asDict(), async=async))

    def getNotes(self, setup):
        parsed = informer.parseSetup(setup)
        project = parsed['project']
        shot = parsed['shot']
        url = informer.getProjectShotNotesUrl(TYPE_PYTHON, project, shot)
        data = restclient.GET(url)
        return eval(data)

    def getElements(self, setup):
        parsed = informer.parseSetup(setup)
        project = parsed['project']
        shot = parsed['shot']
        url = informer.getProjectShotElementsUrl(TYPE_PYTHON, project, shot)
        data = restclient.GET(url)
        return eval(data)
