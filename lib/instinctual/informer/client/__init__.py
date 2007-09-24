#!/usr/bin/env python

# The following line is needed to eval(data) from python requests
import datetime

import instinctual
from instinctual import settings

import restclient
from django.core.serializers.xml_serializer import Deserializer, Serializer

from instinctual import informer
from instinctual.informer.client.appevent import AppEvent

LOG = instinctual.getLogger(__name__)

class ClientConnectionError(Exception):
    pass

# ------------------------------------------------------------------------------
class Client(object):
    def newAppEvent(self, appEvent):
        appEventUrl = instinctual.informer.getAppEventUrl()
        data = appEvent.asDict()
        result = self.POST(appEventUrl, data)

    def getElements(self, setup):
        parsed = informer.parseSetup(setup)
        project = parsed['project']
        shot = parsed['shot']
        url = informer.getProjectShotElementsUrl(project, shot)
        return self.GET(url)

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
        self.PUT(url, updates)

    def newNote(self, setup, data):
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

    def _getObjects(self, deserializer):
        objects = []
        try:
            while 1:
                data = deserializer.next()
                objects.append(data.object)
        except StopIteration, e:
            pass

        return objects

        return objects

    def wrapRestClient(method):
        def wrapper(self, *args, **kwargs):
            kwargs['resp'] = True
            kwargs['async'] = False

            resp, data = method(*args, **kwargs)

            LOG.info(data)

            if 200 != resp.status:
                err = "Request failed: status=%s" % (resp.status)
                raise ClientConnectionError(err)

            return data

        return wrapper

    GET  = wrapRestClient(restclient.GET)
    PUT  = wrapRestClient(restclient.PUT)
    POST = wrapRestClient(restclient.POST)
