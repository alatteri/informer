from django.conf.urls.defaults import *
from django_restapi.model_resource import Collection, Entry, reverse
from django_restapi.resource import Resource
from django_restapi.responder import *
from django_restapi.authentication import *

from django_restapi.receiver import FormReceiver

from instinctual.informer.models import Project, Shot, Note, Element, Event, Output
from django.http import HttpResponseRedirect

# --------------------
# RESTful patterns:
#
xml_all_P_collection = Collection(
    queryset = Project.objects.all(),
    permitted_methods = ['GET'],
    responder = XMLResponder()
)

xml_all_S_collection = Collection(
    queryset = Shot.objects.all(),
    permitted_methods = ['GET'],
    responder = XMLResponder()
)

xml_event_collection = Collection(
    queryset = Shot.objects.all(),
    permitted_methods = ['GET'],
    responder = XMLResponder()
)

class ProjectShots(Collection):
    def read(self, request):
        parts = request.path.split("/")
        project_name = parts[3]
        project = Project.objects.get(name=project_name)
        filtered_set = self.queryset._clone()
        filtered_set = filtered_set.filter(project=project)
        return self.responder.list(request, filtered_set)

xml_S_of_P_collection = ProjectShots(
    queryset = Shot.objects.all(),
    permitted_methods = ['GET'],
    responder = XMLResponder()
)

class ProjectShotCollection(Collection):
    def read(self, request):
        parts = request.path.split("/")
        project_name = parts[3]
        shot_name = parts[5]
        project = Project.objects.get(name=project_name)
        shot = Shot.objects.get(name=shot_name, project=project)
        filtered_set = self.queryset._clone()
        filtered_set = filtered_set.filter(shot=shot)
        return self.responder.list(request, filtered_set)

xml_N_of_S_of_P_collection = ProjectShotCollection(
    queryset = Note.objects.all(),
    permitted_methods = ['GET'],
    responder = XMLResponder()
)

xml_E_of_S_of_P_collection = ProjectShotCollection(
    queryset = Element.objects.all(),
    permitted_methods = ['GET'],
    responder = XMLResponder()
)

from datetime import datetime
from time import strptime, mktime
def strToDatetime(s):
    s = s[0:s.rindex('.')]
    return datetime(*strptime(s, "%m/%d/%y:%H:%M:%S")[0:6])

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
        return HttpResponseRedirect('/')

app_event = AppEvent(
    permitted_methods = ['POST'],
)

# --------------------
# base patterns:
#
urlpatterns = patterns('',
    # Uncomment this for admin:
    (r'^admin/', include('django.contrib.admin.urls')),
    (r'^xml/projects/$', xml_all_P_collection),
    (r'^xml/shots/$', xml_all_S_collection),
    (r'^xml/projects/(?P<project_name>[^/]+)/$',
        xml_S_of_P_collection,
        {'is_entry':False}),
    (r'^xml/projects/(?P<project_name>[^/]+)/shots/(?P<shot_name>[^/]+)/notes/$',
        xml_N_of_S_of_P_collection,
        {'is_entry':False}),
    (r'^xml/projects/(?P<project_name>[^/]+)/shots/(?P<shot_name>[^/]+)/elements/$',
        xml_E_of_S_of_P_collection,
        {'is_entry':False}),
    (r'^xml/app_events/$', app_event),
    # (r'^xml/events/$', xml_event_collection),
)

