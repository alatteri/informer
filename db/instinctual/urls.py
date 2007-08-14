from django.conf.urls.defaults import *

from django_restapi.model_resource import Collection
from django_restapi.responder import *
from django_restapi.authentication import *

from informer.models import Project, Shot

urlpatterns = patterns('',
    # Example:
    # (r'^instinctual/', include('instinctual.foo.urls')),

    # Uncomment this for admin:
    (r'^admin/', include('django.contrib.admin.urls')),
)

# REST:
urls = (
    (Project.objects.all(),     '^xml/projects/$'),
    (Shot.objects.all(),        '^xml/projects/\d+/shots/$'),
)

for entry in urls:
    resource = Collection(
        queryset = entry[0],
        responder = XMLResponder(),
        collection_url_pattern = r'%s' % entry[1],
    )

    urlpatterns += resource.get_url_pattern()

# resource = Collection(
#     queryset = Project.objects.all(),
#     responder = XMLResponder(),
#     collection_url_pattern = r'^xml/project/$'
# )
# 
# urlpatterns += project_resource.get_url_pattern()
