from django.conf.urls.defaults import patterns
from django.contrib.auth.models import User

import instinctual
from instinctual.informer.rest_filelist import FileCollection
from instinctual.informer.responder import CustomXMLResponder, CustomJSONResponder
from instinctual.informer.models import Project, Shot, Note, Element, Event, Frame, Clip, Log
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

xml_clips = ProjectShotCollection(
    queryset = Clip.objects.all(),
    permitted_methods = ['GET', 'POST'],
    responder = CustomXMLResponder(),
    expose_fields = Clip.Rest.expose_fields,
    authentication = InformerAuthentication(),
)

xml_logs = ProjectShotCollection(
    queryset = Log.objects.all(),
    permitted_methods = ['GET'],
    responder = CustomXMLResponder(),
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

json_clips = ProjectShotCollection(
    queryset = Clip.objects.all(),
    permitted_methods = ['GET', 'POST'],
    responder = CustomJSONResponder(),
    expose_fields = Clip.Rest.expose_fields,
    authentication = InformerAuthentication(),
)

json_logs = ProjectShotCollection(
    queryset = Log.objects.all(),
    permitted_methods = ['GET'],
    responder = CustomJSONResponder(),
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

xml_url_shots           = conf.get('informer', 'url_shots') % ('xml')
xml_url_projects        = conf.get('informer', 'url_projects') % ('xml')
xml_url_project_shots   = conf.get('informer', 'url_project_shots') % ('xml', pat_project)
xml_url_note            = conf.get('informer', 'url_project_shot_note') % ('xml', pat_project, pat_shot, pat_note)
xml_url_notes           = conf.get('informer', 'url_project_shot_notes') % ('xml', pat_project, pat_shot)
xml_url_element         = conf.get('informer', 'url_project_shot_element') % ('xml', pat_project, pat_shot, pat_element)
xml_url_elements        = conf.get('informer', 'url_project_shot_elements') % ('xml', pat_project, pat_shot)
xml_url_users           = conf.get('informer', 'url_users') % 'xml'
xml_url_events          = conf.get('informer', 'url_project_shot_events') % ('xml', pat_project, pat_shot)
xml_url_frames          = conf.get('informer', 'url_project_shot_frames') % ('xml', pat_project, pat_shot)
xml_url_clips           = conf.get('informer', 'url_project_shot_clips') % ('xml', pat_project, pat_shot)
xml_url_logs            = conf.get('informer', 'url_project_shot_logs') % ('xml', pat_project, pat_shot)

json_url_shots           = conf.get('informer', 'url_shots') % 'json'
json_url_projects        = conf.get('informer', 'url_projects') % 'json'
json_url_project_shots   = conf.get('informer', 'url_project_shots') % ('json', pat_project)
json_url_note            = conf.get('informer', 'url_project_shot_note') % ('json', pat_project, pat_shot, pat_note)
json_url_notes           = conf.get('informer', 'url_project_shot_notes') % ('json', pat_project, pat_shot)
json_url_element         = conf.get('informer', 'url_project_shot_element') % ('json', pat_project, pat_shot, pat_element)
json_url_elements        = conf.get('informer', 'url_project_shot_elements') % ('json', pat_project, pat_shot)
json_url_users           = conf.get('informer', 'url_users') % 'json'
json_url_events          = conf.get('informer', 'url_project_shot_events') % ('json', pat_project, pat_shot)
json_url_frames          = conf.get('informer', 'url_project_shot_frames') % ('json', pat_project, pat_shot)
json_url_clips           = conf.get('informer', 'url_project_shot_clips') % ('json', pat_project, pat_shot)
json_url_logs            = conf.get('informer', 'url_project_shot_logs') % ('json', pat_project, pat_shot)

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
    ('^' + xml_url_clips + '$',             xml_clips,          {'is_entry':False}),
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
    ('^' + json_url_clips + '$',             json_clips,          {'is_entry':False}),
    ('^' + json_url_logs + '$',              json_logs,           {'is_entry':False}),

    ('^listdir/(?P<path>.*)$', FileCollection(type='json',permitted_methods = ['GET', 'POST'] )),
)
