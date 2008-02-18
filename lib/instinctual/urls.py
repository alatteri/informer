from django.conf.urls.defaults import *

import instinctual
from instinctual import informer
from instinctual.settings import media_path
from instinctual.informer.models import Project
from instinctual.informer import relativeUrl as rel

conf = instinctual.getConf()
root = informer.getServerRoot()

pat_project = "(?P<project_name>[^/]+)"
pat_shot    = "(?P<shot_name>[^/]+)"

url_api = informer.getApiFragment()

url_projects              = rel(informer.getProjectsUrl(format='html'))
url_project_shots         = rel(informer.getProjectShotsUrl(pat_project, format='html'))
url_project_shot          = rel(informer.getProjectShotUrl(pat_project, pat_shot, format='html'))
url_project_shot_notes    = rel(informer.getProjectShotNotesUrl(pat_project, pat_shot, format='html'))
url_project_shot_elements = rel(informer.getProjectShotElementsUrl(pat_project, pat_shot, format='html'))
url_project_shot_renders  = rel(informer.getProjectShotClipsUrl(pat_project, pat_shot, format='html'))

# --------------------
# base patterns:
#
urlpatterns = patterns('',
    (r'^admin/', include('django.contrib.admin.urls')),
    (r'^' + url_api + '/', include('instinctual.informer.urls')),
    (r'^site_media/(?P<path>.*)$', 'django.views.static.serve', {'document_root': media_path}),
    (r'^accounts/login/$', 'django.contrib.auth.views.login'),
    (r'^accounts/logout/$', 'django.contrib.auth.views.logout'),

    (r'^/?$',                                'instinctual.informer.views.project_list'),
    (r'^' + url_projects + '$',              'instinctual.informer.views.project_list'),
    (r'^' + url_project_shots + '$',         'instinctual.informer.views.project_detail'),
    (r'^' + url_project_shot + '$',          'instinctual.informer.views.shot_detail'),
    (r'^' + url_project_shot_notes + '$',    'instinctual.informer.views.shot_notes'),
    (r'^' + url_project_shot_elements + '$', 'instinctual.informer.views.shot_elements'),
    (r'^' + url_project_shot_renders + '$',  'instinctual.informer.views.shot_renders'),
)
