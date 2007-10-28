#!/usr/bin/env python

# The following line is needed to eval(data) from python requests
import time
import datetime

import instinctual
from instinctual import settings

import restclient

from instinctual import informer
from instinctual.informer.client.appevent import AppEvent
from instinctual.informer.client.xml_serializer import Deserializer

LOG = instinctual.getLogger(__name__)

class ClientConnectionError(Exception):
    pass

# ------------------------------------------------------------------------------
class Client(object):
    def newAppEvent(self, appEvent):
        appEventUrl = instinctual.informer.getAppEventsUrl()
        data = appEvent.asDict()
        result = self.POST(appEventUrl, data)

    def getElements(self, setup):
        parsed = informer.parseSetup(setup)
        project = parsed['project']
        shot = parsed['shot']
        url = informer.getProjectShotElementsUrl(project, shot)

        data = self.GET(url)
        deserializer = Deserializer(data)
        elements = self._getObjects(deserializer)
        return elements

    def updateElement(self, setup, updates):
        parsed = informer.parseSetup(setup)
        project = parsed['project']
        shot = parsed['shot']
        id = updates['id']
        del updates['id']

        url = informer.getProjectShotElementUrl(project, shot, id)
        result = self.PUT(url, updates)

        deserializer = Deserializer(result)
        elements = self._getObjects(deserializer)
        return elements

    def getNote(self, setup, id):
        parsed = informer.parseSetup(setup)
        project = parsed['project']
        shot = parsed['shot']
        url = informer.getProjectShotNoteUrl(project, shot, id)

        data = self.GET(url)
        deserializer = Deserializer(data)
        notes = self._getObjects(deserializer)
        return notes[0]

    def getNotes(self, setup):
        parsed = informer.parseSetup(setup)
        project = parsed['project']
        shot = parsed['shot']
        url = informer.getProjectShotNotesUrl(project, shot)

        data = self.GET(url)
        deserializer = Deserializer(data)
        notes = self._getObjects(deserializer)
        return notes

    def updateNote(self, setup, updates):
        parsed = informer.parseSetup(setup)
        project = parsed['project']
        shot = parsed['shot']
        id = updates['id']
        del updates['id']

        url = informer.getProjectShotNoteUrl(project, shot, id)
        result = self.PUT(url, updates)

        deserializer = Deserializer(result)
        notes = self._getObjects(deserializer)
        return notes

    def createNote(self, setup, data):
        if 'id' in data:
            del data['id']

        parsed = informer.parseSetup(setup)
        project = parsed['project']
        shot = parsed['shot']
        url = informer.getProjectShotNotesUrl(project, shot)
        self.POST(url, data)

    def _getObjects(self, deserializer):
        objects = []
        try:
            while 1:
                data = deserializer.next()
                objects.append(data.object)
        except StopIteration, e:
            pass

        return objects

    def wrapRestClient(method, expected):
        def wrapper(self, *args, **kwargs):
            kwargs['resp'] = True
            kwargs['async'] = False

            resp, data = method(*args, **kwargs)

            LOG.info(data)

            if resp.status not in expected:
                err = "Request failed: status=%s" % (resp.status)
                raise ClientConnectionError(err)

            return data

        return wrapper

    GET  = wrapRestClient(restclient.GET,  [200])
    PUT  = wrapRestClient(restclient.PUT,  [200])
    # TODO: remove 200 this is just for AppEvents...
    POST = wrapRestClient(restclient.POST, [200, 201])
