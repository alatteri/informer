from django.conf.urls.defaults import *

# --------------------
# base patterns:
#
urlpatterns = patterns('',
    (r'^admin/', include('django.contrib.admin.urls')),
    (r'^informer/1.0/', include('instinctual.informer.urls')),
)

