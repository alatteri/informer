from django.conf.urls.defaults import patterns
from django.contrib.auth.models import User

import instinctual
from instinctual import informer
from instinctual.informer import relativeApiUrl as rel
from instinctual.informer.rest_filelist import FileCollection
from instinctual.informer.responder import CustomXMLResponder, CustomJSONResponder
from instinctual.informer.models import Project, Shot, Note, Element, Event, Frame, Render, Log
from instinctual.informer.rest import ProjectShots, ProjectShotCollection, PkEntry, Collection, InformerAuthentication

# --------------------
# RESTful patterns:
#
xml_projects = Collection(
    queryset = Project.objects.all(),
    permitted_methods = ['GET'],
    responder = CustomXMLResponder(),
    authentication = InformerAuthentication(),
)

xml_shots = Collection(
    queryset = Shot.objects.all(),
    permitted_methods = ['GET'],
    responder = CustomXMLResponder(),
    authentication = InformerAuthentication(),
)

xml_project_shots = ProjectShots(
    queryset = Shot.objects.all(),
    permitted_methods = ['GET'],
    responder = CustomXMLResponder(),
    authentication = InformerAuthentication(),
)

xml_note = Collection(
    queryset = Note.objects.all(),
    permitted_methods = ['GET', 'PUT'],
    responder = CustomXMLResponder(),
    entry_class = PkEntry,
    expose_fields = Note.Rest.expose_fields,
    authentication = InformerAuthentication(),
)

xml_notes = ProjectShotCollection(
    queryset = Note.objects.all(),
    permitted_methods = ['GET', 'POST'],
    responder = CustomXMLResponder(),
    expose_fields = Note.Rest.expose_fields,
    authentication = InformerAuthentication(),
)

xml_element = Collection(
    queryset = Element.objects.all(),
    permitted_methods = ['GET', 'PUT'],
    responder = CustomXMLResponder(),
    entry_class = PkEntry,
    expose_fields = Element.Rest.expose_fields,
    authentication = InformerAuthentication(),
)

xml_elements = ProjectShotCollection(
    queryset = Element.objects.all(),
    permitted_methods = ['GET', 'POST'],
    responder = CustomXMLResponder(),
    authentication = InformerAuthentication(),
)

xml_events = ProjectShotCollection(
    queryset = Event.objects.all(),
    permitted_methods = ['POST'],
    responder = CustomXMLResponder(),
    authentication = InformerAuthentication(),
)

xml_users = Collection(
    queryset = User.objects.all(),
    permitted_methods = ['GET', 'POST'],
    responder = CustomXMLResponder(),
    authentication = InformerAuthentication(),
)

xml_frames = ProjectShotCollection(
    queryset = Frame.objects.all(),
    permitted_methods = ['GET', 'POST'],
    responder = CustomXMLResponder(),
    authentication = InformerAuthentication(),
)

xml_renders = ProjectShotCollection(
    queryset = Renders.objects.all(),
    permitted_methods = ['GET', 'POST'],
    responder = CustomXMLResponder(),
    expose_fields = Renders.Rest.expose_fields,
    authentication = InformerAuthentication(),
)

xml_logs = ProjectShotCollection(
    queryset = Log.objects.all(),
    permitted_methods = ['GET'],
    responder = CustomXMLResponder(),
    expose_fields = Log.Rest.expose_fields,
    authentication = InformerAuthentication(),
)


json_projects = Collection(
    queryset = Project.objects.all(),
    permitted_methods = ['GET'],
    responder = CustomJSONResponder(),
    authentication = InformerAuthentication(),
)

json_shots = Collection(
    queryset = Shot.objects.all(),
    permitted_methods = ['GET'],
    responder = CustomJSONResponder(),
    authentication = InformerAuthentication(),
)

json_project_shots = ProjectShots(
    queryset = Shot.objects.all(),
    permitted_methods = ['GET'],
    responder = CustomJSONResponder(),
    authentication = InformerAuthentication(),
)

json_note = Collection(
    queryset = Note.objects.all(),
    permitted_methods = ['GET', 'PUT'],
    responder = CustomJSONResponder(),
    entry_class = PkEntry,
    expose_fields = Note.Rest.expose_fields,
    authentication = InformerAuthentication(),
)

json_notes = ProjectShotCollection(
    queryset = Note.objects.all(),
    permitted_methods = ['GET', 'POST'],
    responder = CustomJSONResponder(),
    expose_fields = Note.Rest.expose_fields,
    authentication = InformerAuthentication(),
)

json_element = Collection(
    queryset = Element.objects.all(),
    permitted_methods = ['GET', 'PUT', 'POST'],
    responder = CustomJSONResponder(),
    entry_class = PkEntry,
    expose_fields = Element.Rest.expose_fields,
    authentication = InformerAuthentication(),
)

json_elements = ProjectShotCollection(
    queryset = Element.objects.all(),
    permitted_methods = ['GET', 'POST'],
    responder = CustomJSONResponder(),
    authentication = InformerAuthentication(),
)

json_events = ProjectShotCollection(
    queryset = Event.objects.all(),
    permitted_methods = ['POST'],
    responder = CustomJSONResponder(),
    authentication = InformerAuthentication(),
)

json_users = Collection(
    queryset = User.objects.all(),
    permitted_methods = ['GET', 'POST'],
    responder = CustomJSONResponder(),
    authentication = InformerAuthentication(),
)

json_frames = ProjectShotCollection(
    queryset = Frame.objects.all(),
    permitted_methods = ['GET', 'POST'],
    responder = CustomJSONResponder(),
    authentication = InformerAuthentication(),
)

json_renders = ProjectShotCollection(
    queryset = Render.objects.all(),
    permitted_methods = ['GET', 'POST'],
    responder = CustomJSONResponder(),
    expose_fields = Render.Rest.expose_fields,
    authentication = InformerAuthentication(),
)

json_logs = ProjectShotCollection(
    queryset = Log.objects.all(),
    permitted_methods = ['GET'],
    responder = CustomJSONResponder(),
    expose_fields = Log.Rest.expose_fields,
    authentication = InformerAuthentication(),
)

# --------------------
# informer urls:
#
conf = instinctual.getConf()

pat_project = '[^/]+'
pat_shot    = '[^/]+'
pat_note    = '(\d+)'
pat_element = '(\d+)'

xml_url_shots           = rel(informer.getShotsUrl(format='xml'))
xml_url_projects        = rel(informer.getProjectsUrl(format='xml'))
xml_url_project_shots   = rel(informer.getProjectShotsUrl(pat_project, format='xml'))
xml_url_note            = rel(informer.getProjectShotNoteUrl(pat_project, pat_shot, pat_note, format='xml'))
xml_url_notes           = rel(informer.getProjectShotNotesUrl(pat_project, pat_shot, format='xml'))
xml_url_element         = rel(informer.getProjectShotElementUrl(pat_project, pat_shot, pat_element, format='xml'))
xml_url_elements        = rel(informer.getProjectShotElementsUrl(pat_project, pat_shot, format='xml'))
xml_url_users           = rel(informer.getUsersUrl(format='xml'))
xml_url_events          = rel(informer.getProjectShotEventsUrl(pat_project, pat_shot, format='xml'))
xml_url_frames          = rel(informer.getProjectShotFramesUrl(pat_project, pat_shot, format='xml'))
xml_url_renders         = rel(informer.getProjectShotRendersUrl(pat_project, pat_shot, format='xml'))
xml_url_logs            = rel(informer.getProjectShotLogsUrl(pat_project, pat_shot, format='xml'))

json_url_shots           = rel(informer.getShotsUrl(format='json'))
json_url_projects        = rel(informer.getProjectsUrl(format='json'))
json_url_project_shots   = rel(informer.getProjectShotsUrl(pat_project, format='json'))
json_url_note            = rel(informer.getProjectShotNoteUrl(pat_project, pat_shot, pat_note, format='json'))
json_url_notes           = rel(informer.getProjectShotNotesUrl(pat_project, pat_shot, format='json'))
json_url_element         = rel(informer.getProjectShotElementUrl(pat_project, pat_shot, pat_element, format='json'))
json_url_elements        = rel(informer.getProjectShotElementsUrl(pat_project, pat_shot, format='json'))
json_url_users           = rel(informer.getUsersUrl(format='json'))
json_url_events          = rel(informer.getProjectShotEventsUrl(pat_project, pat_shot, format='json'))
json_url_frames          = rel(informer.getProjectShotFramesUrl(pat_project, pat_shot, format='json'))
json_url_renders         = rel(informer.getProjectShotRendersUrl(pat_project, pat_shot, format='json'))
json_url_logs            = rel(informer.getProjectShotLogsUrl(pat_project, pat_shot, format='json'))

pat_project = "(?P<project_name>%s)" % pat_project
pat_shot    = "(?P<shot_name>%s)" % pat_shot

urlpatterns = patterns('',
    ('^' + xml_url_note + '$',              xml_note),
    ('^' + xml_url_notes + '$',             xml_notes,          {'is_entry':False}),
    ('^' + xml_url_shots + '$',             xml_shots),
    ('^' + xml_url_projects + '$',          xml_projects),
    ('^' + xml_url_element + '$',           xml_element),
    ('^' + xml_url_elements + '$',          xml_elements,       {'is_entry':False}),
    ('^' + xml_url_events + '$',            xml_events),
    ('^' + xml_url_project_shots + '$',     xml_project_shots,  {'is_entry':False}),
    ('^' + xml_url_users + '$',             xml_users,          {'is_entry':False}),
    ('^' + xml_url_frames + '$',            xml_frames,         {'is_entry':False}),
    ('^' + xml_url_renders + '$',           xml_renders,        {'is_entry':False}),
    ('^' + xml_url_logs + '$',              xml_logs,           {'is_entry':False}),

    ('^' + json_url_note + '$',              json_note),
    ('^' + json_url_notes + '$',             json_notes,          {'is_entry':False}),
    ('^' + json_url_shots + '$',             json_shots),
    ('^' + json_url_projects + '$',          json_projects),
    ('^' + json_url_element + '$',           json_element),
    ('^' + json_url_elements + '$',          json_elements,       {'is_entry':False}),
    ('^' + json_url_events + '$',            json_events),
    ('^' + json_url_project_shots + '$',     json_project_shots,  {'is_entry':False}),
    ('^' + json_url_users + '$',             json_users,          {'is_entry':False}),
    ('^' + json_url_frames + '$',            json_frames,         {'is_entry':False}),
    ('^' + json_url_renders + '$',           json_renders,        {'is_entry':False}),
    ('^' + json_url_logs + '$',              json_logs,           {'is_entry':False}),

    ('^listdir/(?P<path>.*)$', FileCollection(type='json',permitted_methods = ['GET', 'POST'] )),
)
