#!/usr/bin/env python

# The following line is needed to eval(data) from python requests
import time
from datetime import datetime

import instinctual
from instinctual import settings

import restclient

from instinctual import informer
from instinctual.informer.client.xml_serializer import Deserializer
from instinctual.informer.client import httplib2auth

LOG = instinctual.getLogger(__name__)

# Force the restclient to use the authenticating httplib...
restclient.__dict__['httplib2'] = httplib2auth

# ------------------------------------------------------------------------------
class ClientConnectionError(Exception):
    pass

# ------------------------------------------------------------------------------
class Client(object):
    def createEvent(self, event):
        parsed = informer.parseSetup(event.setup)
        project = parsed['project']
        shot = parsed['shot']
        eventsUrl = informer.getProjectShotEventsUrl(project, shot, 'xml')

        data = {}
        data['created_on'] = event.date
        data['created_by'] = event.user
        data['type'] = event.event
        data['setup'] = event.setup
        data['volume'] = event.volume
        data['host'] = event.hostname

        # if event.__class__.__name__ == 'DiscreetAppBatchProcessEvent':
        #    data['outputs'] = ','.join(event.outputs)

        LOG.warn(''.join(['-'*20, 'Event', '-'*20]))
        for (key, val) in data.items():
            LOG.warn("%s: %s" % (key, val))
        LOG.warn(''.join(['-'*20, '---------', '-'*20]))

        result = self.POST(eventsUrl, data)

    def createRender(self, render):
        parsed = informer.parseSetup(render.setup)
        project = parsed['project']
        shot = parsed['shot']
        url = informer.getProjectShotRendersUrl(project, shot, 'xml')

        data = {}
        data['job'] = render.job
        data['rate'] = render.rate
        data['created_by'] = render.user
        data['setup'] = render.setup
        data['host'] = render.hostname

        LOG.warn(''.join(['-'*20, 'Event', '-'*20]))
        for (key, val) in data.items():
            LOG.warn("%s: %s" % (key, val))
        LOG.warn(''.join(['-'*20, '---------', '-'*20]))

        result = self.POST(url, data)


    def createFrame(self, frame):
        framesUrl = informer.getFramesUrl('xml')

        data = {}
        data['job'] = frame.job
        data['width'] = frame.width
        data['height'] = frame.height
        data['depth'] = frame.depth

        data['number'] = frame.number
        data['spark'] = frame.spark
        data['host'] = frame.host

        data['created_by'] = frame.createdBy
        data['created_on'] = frame.createdOn

        data['resized_width'] = frame.resizedWidth
        data['resized_height'] = frame.resizedHeight
        data['resized_depth'] = frame.resizedDepth

        image = open(frame.resizedPath).read()
        filename = "%s.png" % (frame.uuid)
        files = {'image': {'file': image, 'filename': filename}}
        result = self.POST(framesUrl, data, files=files)

    def createNote(self, setup, data):
        if 'id' in data:
            del data['id']

        parsed = informer.parseSetup(setup)
        project = parsed['project']
        shot = parsed['shot']
        url = informer.getProjectShotNotesUrl(project, shot, 'xml')
        self.POST(url, data)

    def getElements(self, setup):
        parsed = informer.parseSetup(setup)
        project = parsed['project']
        shot = parsed['shot']
        url = informer.getProjectShotElementsUrl(project, shot, 'xml')

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

        url = informer.getProjectShotElementUrl(project, shot, id, 'xml')
        result = self.PUT(url, updates)

        deserializer = Deserializer(result)
        elements = self._getObjects(deserializer)
        return elements

    def getNote(self, setup, id):
        parsed = informer.parseSetup(setup)
        project = parsed['project']
        shot = parsed['shot']
        url = informer.getProjectShotNoteUrl(project, shot, id, 'xml')

        data = self.GET(url)
        deserializer = Deserializer(data)
        notes = self._getObjects(deserializer)
        return notes[0]

    def getNotes(self, setup):
        parsed = informer.parseSetup(setup)
        project = parsed['project']
        shot = parsed['shot']
        url = informer.getProjectShotNotesUrl(project, shot, 'xml')

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

        url = informer.getProjectShotNoteUrl(project, shot, id, 'xml')
        result = self.PUT(url, updates)

        deserializer = Deserializer(result)
        notes = self._getObjects(deserializer)
        return notes

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
                rest = open("/usr/discreet/sparks/instinctual/informer/logs/rest.%s.html" % (time.time()), 'w')
                rest.write(data)
                rest.close()
                err = "Request failed: status=%s" % (resp.status)
                e = ClientConnectionError(err)
                e.resp = resp
                raise e

            return data

        return wrapper

    GET  = wrapRestClient(restclient.GET,  [200])
    PUT  = wrapRestClient(restclient.PUT,  [200])
    POST = wrapRestClient(restclient.POST, [201])
