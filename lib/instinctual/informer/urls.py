from django.conf.urls.defaults import patterns

import instinctual
from instinctual.informer.responder import XMLResponder, JSONResponder
from instinctual.informer.models import Project, Shot, Note, Element, Event, Output
from instinctual.informer.rest import Collection, ProjectShots, ProjectShotCollection, AppEvent, XMLReceiver, PkEntry

fields_note = ['text', 'is_checked', 'created_by', 'created_on', 'modified_by', 'modified_on']
fields_element = ['text', 'kind', 'is_checked', 'created_by', 'created_on']

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
    permitted_methods = ['GET', 'PUT'],
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

xml_element = Collection(
    queryset = Element.objects.all(),
    permitted_methods = ['GET', 'PUT'],
    responder = XMLResponder(),
    entry_class = PkEntry,
    expose_fields = fields_element,
)

xml_elements = ProjectShotCollection(
    queryset = Element.objects.all(),
    permitted_methods = ['GET'],
    responder = XMLResponder()
)

xml_app_events = AppEvent(
    permitted_methods = ['POST'],
)

json_projects = Collection(
    queryset = Project.objects.all(),
    permitted_methods = ['GET'],
    responder = JSONResponder()
)

json_shots = Collection(
    queryset = Shot.objects.all(),
    permitted_methods = ['GET'],
    responder = JSONResponder()
)

json_project_shots = ProjectShots(
    queryset = Shot.objects.all(),
    permitted_methods = ['GET'],
    responder = JSONResponder()
)

json_note = Collection(
    queryset = Note.objects.all(),
    permitted_methods = ['GET', 'PUT'],
    responder = JSONResponder(),
    entry_class = PkEntry,
    expose_fields = fields_note,
)

json_notes = ProjectShotCollection(
    queryset = Note.objects.all(),
    permitted_methods = ['GET', 'POST'],
    responder = JSONResponder(),
    expose_fields = fields_note,
)

json_element = Collection(
    queryset = Element.objects.all(),
    permitted_methods = ['GET', 'PUT'],
    responder = JSONResponder(),
    entry_class = PkEntry,
    expose_fields = fields_element,
)

json_elements = ProjectShotCollection(
    queryset = Element.objects.all(),
    permitted_methods = ['GET'],
    responder = JSONResponder()
)

json_app_events = AppEvent(
    permitted_methods = ['POST'],
)

# --------------------
# informer urls:
#
conf = instinctual.getConf()

pat_project = '[^/]+'
pat_shot    = '[^/]+'
pat_note    = '(\d+)'
pat_element = '(\d+)'

xml_url_shots           = conf.get('informer', 'url_shots') % ('xml')
xml_url_projects        = conf.get('informer', 'url_projects') % ('xml')
xml_url_app_events      = conf.get('informer', 'url_app_events') % ('xml')
xml_url_project_shots   = conf.get('informer', 'url_project_shots') % ('xml', pat_project)
xml_url_note            = conf.get('informer', 'url_project_shot_note') % ('xml', pat_project, pat_shot, pat_note)
xml_url_notes           = conf.get('informer', 'url_project_shot_notes') % ('xml', pat_project, pat_shot)
xml_url_element         = conf.get('informer', 'url_project_shot_element') % ('xml', pat_project, pat_shot, pat_element)
xml_url_elements        = conf.get('informer', 'url_project_shot_elements') % ('xml', pat_project, pat_shot)

json_url_shots           = conf.get('informer', 'url_shots') % 'json'
json_url_projects        = conf.get('informer', 'url_projects') % 'json'
json_url_app_events      = conf.get('informer', 'url_app_events') % 'json'
json_url_project_shots   = conf.get('informer', 'url_project_shots') % ('json', pat_project)
json_url_note            = conf.get('informer', 'url_project_shot_note') % ('json', pat_project, pat_shot, pat_note)
json_url_notes           = conf.get('informer', 'url_project_shot_notes') % ('json', pat_project, pat_shot)
json_url_element         = conf.get('informer', 'url_project_shot_element') % ('json', pat_project, pat_shot, pat_element)
json_url_elements        = conf.get('informer', 'url_project_shot_elements') % ('json', pat_project, pat_shot)

pat_project = "(?P<project_name>%s)" % pat_project
pat_shot    = "(?P<shot_name>%s)" % pat_shot

html_url_projects        = conf.get('informer', 'url_projects') % 'html'
html_url_project_shots   = conf.get('informer', 'url_project_shots') % ('html', pat_project)
html_url_project_shot    = conf.get('informer', 'url_project_shot') % ('html', pat_project, pat_shot)

urlpatterns = patterns('',
    ('^' + xml_url_note + '$',              xml_note),
    ('^' + xml_url_notes + '$',             xml_notes,          {'is_entry':False}),
    ('^' + xml_url_shots + '$',             xml_shots),
    ('^' + xml_url_projects + '$',          xml_projects),
    ('^' + xml_url_element + '$',           xml_element),
    ('^' + xml_url_elements + '$',          xml_elements,       {'is_entry':False}),
    ('^' + xml_url_app_events + '$',        xml_app_events),
    ('^' + xml_url_project_shots + '$',     xml_project_shots,  {'is_entry':False}),

    ('^' + json_url_note + '$',              json_note),
    ('^' + json_url_notes + '$',             json_notes,          {'is_entry':False}),
    ('^' + json_url_shots + '$',             json_shots),
    ('^' + json_url_projects + '$',          json_projects),
    ('^' + json_url_element + '$',           json_element),
    ('^' + json_url_elements + '$',          json_elements,       {'is_entry':False}),
    ('^' + json_url_app_events + '$',        json_app_events),
    ('^' + json_url_project_shots + '$',     json_project_shots,  {'is_entry':False}),

    ('^' + html_url_projects + '$', 'django.views.generic.list_detail.object_list', {'queryset':Project.objects.all()}),
    ('^' + html_url_project_shots + '$', 'instinctual.informer.views.project_detail'),
    ('^' + html_url_project_shot + '$', 'instinctual.informer.views.shot_detail'),


)


