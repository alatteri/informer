from django.conf.urls.defaults import *
from django_restapi.model_resource import Collection, Entry, reverse
from django_restapi.responder import *
from django_restapi.authentication import *

from informer.models import Project, Shot

# --------------------
# RESTful patterns:
#
xml_project_collection = Collection(
    queryset = Project.objects.all(),
    permitted_methods = ['GET'],
    responder = XMLResponder()
)

xml_shot_collection = Collection(
    queryset = Shot.objects.all(),
    permitted_methods = ['GET'],
    responder = XMLResponder()
)

# --------------------
# base patterns:
#
urlpatterns = patterns('',
    # Uncomment this for admin:
    (r'^admin/', include('django.contrib.admin.urls')),
    (r'^xml/projects/$', xml_project_collection),
    (r'^xml/projects/\d+/shots/$', xml_shot_collection),
)

