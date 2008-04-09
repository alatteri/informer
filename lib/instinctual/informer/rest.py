import os
from django.shortcuts import get_object_or_404
from django_restapi.model_resource import Collection, Entry, load_put_and_files
from django_restapi.authentication import HttpBasicAuthentication
from django.contrib.auth.models import User
from instinctual.informer.models import Project, Shot, Note, Element, Event, Frame, Render, getHandler

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

class InformerCollection(Collection):
    def __init__(self, *args, **kwargs):
        if 'entry_class' not in kwargs:
            kwargs['entry_class'] = PkEntry
        Collection.__init__(self, *args, **kwargs)

    def _pre_init(self, request, new_model, data):
        pass

    def _pre_save(self, request, new_model):
        pass

    def dispatch(self, request, target, *args, **kwargs):
        """
        Override the django_restapi's Collection object to allow support for
        Prototype's _method argument style.
        """
        request_method = request.method.upper()
        self.request = request

        if request_method == 'POST' and '_method' in request.POST:
            method = str.lower(request.POST['_method'])
            if 'put' == method:
                request_method = 'PUT'
                request.PUT = request.POST
            elif 'delete' == method:
                request_method = 'DELETE'
                # request.DELETE = request.POST

        if request_method not in self.permitted_methods:
            raise HttpMethodNotAllowed

        if request_method == 'GET':
            return target.read(request, *args, **kwargs)
        elif request_method == 'POST':
            return target.create(request, *args, **kwargs)
        elif request_method == 'PUT':
            load_put_and_files(request)
            return target.update(request, *args, **kwargs)
        elif request_method == 'DELETE':
            return target.delete(request, *args, **kwargs)
        else:
            raise Http404

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

        self._pre_init(request, new_model, data)

        # seed with POST data
        for (key, val) in data.items():
            if new_model.__class__.__name__ == 'Note' and key == 'assigned_to' and val:
                val = User.getUser(val) 
            print "going to set [%s] with [%s]" % (key, val)
            new_model.__setattr__(key, val)

        # set created|modified by
        for key in ('created_by', 'modified_by'):
            if key in [f.name for f in new_model._meta.fields]:
                if key not in data:
                    print "NOT SET: going to set [%s] with [%s]" % (key, request.user)
                    new_model.__setattr__(key, request.user)

        print "now going to save"
        self._pre_save(request, new_model)

        # If the data contains no errors, save the model,
        # return a "201 Created" response with the model's
        # URI in the location header and a representation
        # of the model in the response body.
        new_model.save()

        model_entry = self.entry_class(self, new_model)
        response = model_entry.read(request)
        response.status_code = 201
        print "the model entry is: %s, type %s" % (model_entry, type(model_entry))
        response.headers['Location'] = model_entry.get_url()
        print "the location is: %s" % (response.headers['Location'])
        return response
    create = LogHandlerWrapper(create, 'POST')

class FrameCollection(InformerCollection):
    def _pre_init(self, request, new_model, data):
        # associate the frame with the shot and render
        r = get_object_or_404(Render, job=data['job'])
        new_model.render = r
        new_model.shot = r.shot

        # remove elements not in the actual frame model
        for key in ('spark', 'job'):
            del data[key]

    def _pre_save(self, request, new_model):
        r = new_model.render

        # handle file uploads
        if 'image' in request.FILES:
            content = request.FILES['image']['content']
            filename = "%06d-%06d.jp2" % (int(new_model.render.id), int(new_model.number))
            new_model.save_image_file(filename, content, True)

class ProjectShots(InformerCollection):
    def read(self, request):
        project_name = Project.getNameFromRequest(request)
        project = Project.getProject(project_name)

        filtered_set = self.queryset._clone()
        filtered_set = filtered_set.filter(project=project)
        return self.responder.list(request, filtered_set)

class ProjectShotCollection(InformerCollection):
    def read(self, request):
        project_name = Project.getNameFromRequest(request)
        project = get_object_or_404(Project, name=project_name)

        shot_name = Shot.getNameFromRequest(request)
        shot = get_object_or_404(Shot, project=project, name=shot_name)

        filtered_set = self.queryset._clone()
        filtered_set = filtered_set.filter(shot=shot)

        return self.responder.list(request, filtered_set)

    def _pre_init(self, request, new_model, data):
        """
        Associates a resource with shot and project specified in URI
        """
        # TODO: specifiy form_class in urls.py to verify data
        project_name = Project.getNameFromRequest(request)
        project = Project.getProject(project_name, create=True)

        shot_name = Shot.getNameFromRequest(request)
        shot = Shot.getShot(shot_name, project, create=True)

        # associate with the shot
        new_model.shot = shot

class PkEntry(Entry):
    """
    Creates the Entry from the primary key and only updates
    those fields that are specified in the put data
    """
    def update(self, request):
        # TODO: data validation/checking
        data = self.collection.receiver.get_put_data(request)

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

    def get_url(self):
        if self.collection and self.collection.request:
            path = self.collection.request.META['PATH_INFO']
            if (self.model.__class__.__name__ in ('Project', 'Shot')):
                return "%s%s" % (path, self.model.name)
            else:
                pk_value = getattr(self.model, self.model._meta.pk.name)
                return "%s%s" % (path, pk_value)
        else:
            return '/'

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
