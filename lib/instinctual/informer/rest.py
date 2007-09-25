from datetime import datetime
from time import strptime, mktime

from django.http import HttpResponse

from django_restapi.resource import Resource
from django_restapi.receiver import FormReceiver, XMLReceiver
from django_restapi.model_resource import Collection, Entry, reverse, InvalidModelData

from instinctual.informer.models import Project, Shot, Note, Element, Event, Output

def strToDatetime(s):
    s = s[0:s.rindex('.')]
    return datetime(*strptime(s, "%m/%d/%y:%H:%M:%S")[0:6])

def getProjectNameFromRequest(request):
    parts = request.path.split("/")
    project_name = parts[5]
    return project_name

def getShotNameFromRequest(request):
    parts = request.path.split("/")
    shot_name = parts[7]
    return shot_name

class ProjectShots(Collection):
    def read(self, request):
        project_name = getProjectNameFromRequest(request)
        project = Project.objects.get(name=project_name)

        filtered_set = self.queryset._clone()
        filtered_set = filtered_set.filter(project=project)
        return self.responder.list(request, filtered_set)

class ProjectShotCollection(Collection):
    def read(self, request):
        project_name = getProjectNameFromRequest(request)
        project = Project.objects.get(name=project_name)

        shot_name = getShotNameFromRequest(request)
        shot = Shot.objects.get(name=shot_name, project=project)

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
        # TODO: specifiy form_class in urls.py to verify data
        project_name = getProjectNameFromRequest(request)
        project = Project.objects.get(name=project_name)

        shot_name = getShotNameFromRequest(request)
        shot = Shot.objects.get(name=shot_name, project=project)

        data = self.receiver.get_post_data(request)
        new_model = self.queryset.model()

        # associate with the shot
        new_model.shot = shot

        # seed with POST data
        for (key, val) in data.items():
            new_model.__setattr__(key, val)

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

        for (key, val) in data.items():
            self.model.__setattr__(key, val)

        self.model.save()

        response = self.read(request)
        response.status_code = 200
        response.headers['Location'] = self.get_url()
        return response

class AppEvent(Resource):
    def create(self, request):
        nowServer = datetime.now()
        print "Server says now is: ", nowServer

        receiver = FormReceiver()
        data = receiver.get_post_data(request)
        print "I got this data:", data

        p = None
        project = data['project']
        try:
            p = Project.objects.get(name=project)
            print "--- project already existed ---"
        except Project.DoesNotExist:
            p = Project(name=project)
            p.save()
            print "--- saved p ---"

        s = None
        shot = data['shot']
        setup = data['setup']
        try:
            s = Shot.objects.get(name=shot, project=p)
            print "--- shot already existed ---"
        except Shot.DoesNotExist:
            s = Shot(name=shot, setup=setup, project=p)
            s.save()
            print "--- saved s ---"

        e = None
        user = data['user']
        host = data['hostname']
        event = data['event']

        date = strToDatetime(data['date'])
        print "Unadjusted date is: ", date

        nowClient = strToDatetime(data['now'])
        print "Client says now is: ", nowClient

        date = date + (nowServer - nowClient)
        print "Adjusted date is: ", date
        try:
            e = Event.objects.get(shot=s, type=event, date_added=date)
            print "--- event already existed ---"
        except Event.DoesNotExist:
            e = Event(shot=s, type=event, host=host, user=user, date_added=date)
            e.save()
            print "--- saved e ---"

        if 'outputs' in data:
            print "((((( outputs exist... ))))))"
            for output in data['outputs'].split('|'):
                print "((((((((( working on %s ))))))))" % output
                try:
                    o = Output.objects.get(location=output, event=e)
                    print "--- output already existed ---"
                except Output.DoesNotExist:
                    o = Output(location=output, event=e)
                    o.save()
                    print "--- saved o ---"

        return HttpResponse('')

