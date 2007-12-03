from datetime import datetime
from time import strptime, mktime

from django.http import HttpResponse
from django.contrib.auth.models import User

from django_restapi.resource import Resource
from django_restapi.receiver import FormReceiver, XMLReceiver
from django_restapi.model_resource import Collection, Entry, reverse, InvalidModelData

from instinctual.informer.models import Project, Shot, Note, Element, Event, Frame

class ProjectShots(Collection):
    def read(self, request):
        project_name = Project.getNameFromRequest(request)
        project = Project.getProject(project_name)

        filtered_set = self.queryset._clone()
        filtered_set = filtered_set.filter(project=project)
        return self.responder.list(request, filtered_set)

class ProjectShotCollection(Collection):
    def read(self, request):
        project_name = Project.getNameFromRequest(request)
        project = Project.getProject(project_name)

        shot_name = Shot.getNameFromRequest(request)
        shot = Shot.getShot(shot_name, project)

        filtered_set = self.queryset._clone()
        filtered_set = filtered_set.filter(shot=shot)

        return self.responder.list(request, filtered_set)

    def create(self, request):
        """
        Creates a resource with attributes given by POST, then
        redirects to the resource URI.

        Unlike the base Collection create() it does not require every
        since field to be specified.
        """
        data = self.receiver.get_post_data(request)
        print "ok here's the data: %s" % (data)

        new_model = self.queryset.model()
        print "ok just made new_model %s" % (new_model.__class__.__name__)

        # calculate the time difference between client and server
        if 'now' in data:
            clientNow = new_model.getDateTime(data['now'])
            new_model.calculateDelta(clientNow)
            del data['now']

        # TODO: specifiy form_class in urls.py to verify data
        project_name = Project.getNameFromRequest(request)
        project = Project.getProject(project_name, create=True)

        shot_name = Shot.getNameFromRequest(request)
        shot = Shot.getShot(shot_name, project, create=True)

        # associate with the shot
        new_model.shot = shot

        # remove elements not in the actual frame model
        if 'Frame' == new_model.__class__.__name__:
            info = {}
            for key in ('rate', 'start', 'end', 'spark'):
                info[key] = data[key]
                del data[key]

        # seed with POST data
        for (key, val) in data.items():
            print "going to set [%s] with [%s]" % (key, val)
            new_model.__setattr__(key, val)

        # handle file uploads
        if 'Frame' == new_model.__class__.__name__:
            c = new_model.getOrCreateParentClip(**info)
            new_model.clip = c

            if 'image' in request.FILES:
                content = request.FILES['image']['content']
                filename = request.FILES['image']['filename']
                new_model.save_image_file(filename, content, True)

        print "now going to save"
        # If the data contains no errors, save the model,
        # return a "201 Created" response with the model's
        # URI in the location header and a representation
        # of the model in the response body.
        new_model.save()

        model_entry = self.entry_class(self, new_model)
        response = model_entry.read(request)
        response.status_code = 201
        response.headers['Location'] = model_entry.get_url()
        return response


class PkEntry(Entry):
    """
    Creates the Entry from the primary key and only updates
    those fields that are specified in the put data
    """
    def update(self, request):
        # TODO: data validation/checking
        data = self.collection.receiver.get_put_data(request)

        # calculate the time difference between client and server
        if 'now' in data:
            clientNow = self.model.getDateTime(data['now'])
            self.model.calculateDelta(clientNow)
            del data['now']

        for (key, val) in data.items():
            self.model.__setattr__(key, val)

        self.model.save()

        response = self.read(request)
        response.status_code = 200
        response.headers['Location'] = self.get_url()
        return response
