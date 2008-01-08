from django.conf.urls.defaults import *

from instinctual.settings import media_path
# --------------------
# base patterns:
#
urlpatterns = patterns('',
    (r'^admin/', include('django.contrib.admin.urls')),
    (r'^informer/1.0/', include('instinctual.informer.urls')),
    (r'^informer/html/', include('instinctual.informer.html_urls')),

    (r'^site_media/(?P<path>.*)$', 'django.views.static.serve', {'document_root': media_path}),
    (r'^accounts/login/$', 'django.contrib.auth.views.login'),
)

