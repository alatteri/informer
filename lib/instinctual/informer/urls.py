from django.conf.urls.defaults import patterns

import instinctual
from instinctual.informer.responder import XMLResponder
from instinctual.informer.models import Project, Shot, Note, Element, Event, Output
from instinctual.informer.rest import Collection, ProjectShots, ProjectShotCollection, AppEvent, XMLReceiver, PkEntry

fields_note = ['text', 'is_checked', 'added_by', 'added_on', 'modified_by', 'modified_on']

# --------------------
# RESTful patterns:
#
xml_projects = Collection(
    queryset = Project.objects.all(),
    permitted_methods = ['GET'],
    responder = XMLResponder()
)

xml_shots = Collection(
    queryset = Shot.objects.all(),
    permitted_methods = ['GET'],
    responder = XMLResponder()
)

xml_project_shots = ProjectShots(
    queryset = Shot.objects.all(),
    permitted_methods = ['GET'],
    responder = XMLResponder()
)

xml_note = Collection(
    queryset = Note.objects.all(),
    permitted_methods = ['GET','PUT'],
    responder = XMLResponder(),
    entry_class = PkEntry,
    expose_fields = fields_note,
)

xml_notes = ProjectShotCollection(
    queryset = Note.objects.all(),
    permitted_methods = ['GET', 'POST'],
    responder = XMLResponder(),
    expose_fields = fields_note,
)

xml_elements = ProjectShotCollection(
    queryset = Element.objects.all(),
    permitted_methods = ['GET'],
    responder = XMLResponder()
)

xml_app_events = AppEvent(
    permitted_methods = ['POST'],
)

# --------------------
# informer urls:
#
conf = instinctual.getConf()

pat_project = '[^/]+'
pat_shot    = '[^/]+'
pat_note    = '(\d+)'

url_shots           = conf.get('informer', 'url_shots')
url_projects        = conf.get('informer', 'url_projects')
url_app_events      = conf.get('informer', 'url_app_events')
url_project_shots   = conf.get('informer', 'url_project_shots') % (pat_project)
url_note            = conf.get('informer', 'url_project_shot_note') % (pat_project, pat_shot, pat_note)
url_notes           = conf.get('informer', 'url_project_shot_notes') % (pat_project, pat_shot)
url_elements        = conf.get('informer', 'url_project_shot_elements') % (pat_project, pat_shot)

urlpatterns = patterns('',
    ('^' + url_note + '$',              xml_note),
    ('^' + url_notes + '$',             xml_notes,          {'is_entry':False}),
    ('^' + url_shots + '$',             xml_shots),
    ('^' + url_projects + '$',          xml_projects),
    ('^' + url_elements + '$',          xml_elements,       {'is_entry':False}),
    ('^' + url_app_events + '$',        xml_app_events),
    ('^' + url_project_shots + '$',     xml_project_shots,  {'is_entry':False}),
)

