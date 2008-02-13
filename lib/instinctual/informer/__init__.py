import re
import instinctual

LOG = instinctual.getLogger(__name__)

def getServerRoot():
    conf = instinctual.getConf()
    proto = conf.get('informer', 'proto')
    server = conf.get('informer', 'server')
    port = conf.get('informer', 'port')

    if port:
        return "%s://%s:%s" % (proto, server, port)
    else:
        return "%s://%s" % (proto, server)

def _getUrlComponent(component, format='xml'):
    conf = instinctual.getConf()
    root = getServerRoot()

    if 'html' == format:
        fragment = conf.get('informer', component)
        if '%s/' == fragment[0:3]:
            fragment = fragment[3:]
        url = "%s/%s" % (root, fragment)
    else:
        base = conf.get('informer', 'url_base')
        url = "%s/%s/%s" % (root, base, conf.get('informer', component))
    return url

def getProjectsUrl(format):
    template = _getUrlComponent('url_projects', format)
    return template

def getProjectShotsUrl(project, format):
    template = _getUrlComponent('url_project_shots', format)
    return template % (project)

def getProjectShotUrl(project, shot, format):
    template = _getUrlComponent('url_project_shot', format)
    return template % (project, shot)

def getProjectShotNoteUrl(project, shot, pk, format):
    template = _getUrlComponent('url_project_shot_note', format)
    return template % (project, shot, pk)

def getProjectShotNotesUrl(project, shot, format):
    template = _getUrlComponent('url_project_shot_notes', format)
    return template % (project, shot)

def getProjectShotElementUrl(project, shot, pk, format):
    template = _getUrlComponent('url_project_shot_element', format)
    return template % (project, shot, pk)

def getProjectShotElementsUrl(project, shot, format):
    template = _getUrlComponent('url_project_shot_elements', format)
    return template % (project, shot)

def getProjectShotEventsUrl(project, shot, format):
    template = _getUrlComponent('url_project_shot_events', format)
    return template % (project, shot)

def getProjectShotFramesUrl(project, shot, format):
    template = _getUrlComponent('url_project_shot_frames', format)
    return template % (project, shot)

def getProjectShotLogsUrl(project, shot, format):
    template = _getUrlComponent('url_project_shot_logs', format)
    return template % (project, shot)

def getProjectShotClipsUrl(project, shot, format):
    template = _getUrlComponent('url_project_shot_clips', format)
    return template % (project, shot)

"""
/x1/j_lo/batch/001_100/0001_comp-01.batch
    project: j_lo
    shot: 0001
    # sometimes sh0001 or sc0001
"""
_reSetup = re.compile(r'/project/([^/]+)/batch/(\w+)')
def parseSetup(setup):
    match = _reSetup.search(setup)
    if match != None:
        parsed = {'project': match.group(1), 'shot': match.group(2)}
        LOG.info("parsed ---> %s" % (parsed))
        return parsed
    else:
        raise ValueError("Unable to parse setup: %s" % (setup))
