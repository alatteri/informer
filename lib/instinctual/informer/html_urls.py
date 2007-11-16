from django.conf.urls.defaults import patterns

import instinctual
from instinctual.informer.models import Project, Shot, Note, Element, Event, Output

urlpatterns = patterns('',
    ('^projects/$', 'django.views.generic.list_detail.object_list', {'queryset':Project.objects.all()}),
    ('^projects/(?P<project_name>[^/]+)/$', 'instinctual.informer.views.project_detail'),
    ('^projects/(?P<project_name>[^/]+)/(?P<shot_name>[^/]+)/$', 'instinctual.informer.views.shot_detail'),
)

