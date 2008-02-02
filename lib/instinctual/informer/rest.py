import os
from django_restapi.model_resource import Collection, Entry
from django_restapi.authentication import HttpBasicAuthentication
from django.contrib.auth.models import User
from instinctual.informer.models import Project, Shot, Note, Element, Event, Frame, Clip, getHandler

def LogHandlerWrapper(func, method):
    def wrapper(obj, request):
        try:
            handler = getHandler()
            data = getattr(request, method)

            if 'modified_by' in data:
                handler.setUser(data['modified_by'])
            elif 'created_by' in data:
                handler.setUser(data['created_by'])
            else:
                handler.setUser(request.user)

            return func(obj, request)
        finally:
            handler.setUser(None)
    return wrapper

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
            if new_model.__class__.__name__=='Note' and key=='assigned_to' and val:
                val = User.getUser(val) 
            print "going to set [%s] with [%s]" % (key, val)
            new_model.__setattr__(key, val)

        # set created|modified by
        for key in ('created_by', 'modified_by'):
            if key in [f.name for f in new_model._meta.fields]:
                if key not in data:
                    print "NOT SET: going to set [%s] with [%s]" % (key, request.user)
                    new_model.__setattr__(key, request.user)

        # handle file uploads
        if 'Frame' == new_model.__class__.__name__:
            c = new_model.getOrCreateParentClip(**info)
            new_model.clip = c

            if 'image' in request.FILES:
                content = request.FILES['image']['content']
                filename = "%06d-%06d.png" % (int(c.id), int(new_model.number))
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
    create = LogHandlerWrapper(create, 'POST')

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

        # set modified by
        for key in ('modified_by'):
            if key in [f.name for f in self.model._meta.fields]:
                if key not in data:
                    print "NOT SET: going to set [%s] with [%s]" % (key, request.user)
                    self.model.__setattr__(key, request.user)

        self.model.save()

        response = self.read(request)
        response.status_code = 200
        response.headers['Location'] = self.get_url()
        return response
    update = LogHandlerWrapper(update, 'PUT')


class InformerAuthentication(HttpBasicAuthentication):
    def is_authenticated(self, request):
        if request.user.is_authenticated():
            return True
        if not request.META.has_key('HTTP_AUTHORIZATION'):
            return False
        (authmeth, auth) = request.META['HTTP_AUTHORIZATION'].split(' ', 1)
        if authmeth.lower() != 'basic':
            return False
        auth = auth.strip().decode('base64')
        username, password = auth.split(':', 1)
        return self.authfunc(username=username, password=password)
