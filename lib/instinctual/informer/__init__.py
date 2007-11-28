import re
import instinctual

LOG = instinctual.getLogger(__name__)

def getServerRoot():
    conf = instinctual.getConf()
    proto = conf.get('informer', 'proto')
    server = conf.get('informer', 'server')
    port = conf.get('informer', 'port')
    base = conf.get('informer', 'url_base')
    return "%s://%s:%s/%s" % (proto, server, port, base)

def _getUrlComponent(component):
    conf = instinctual.getConf()
    root = getServerRoot()
    return "%s/%s" % (root, conf.get('informer', component))

def getProjectShotsUrl(project, format='xml'):
    template = _getUrlComponent('url_project_shots')
    return template % (format, project)

def getProjectShotUrl(project, shot, format='xml'):
    template = _getUrlComponent('url_project_shot')
    return template % (format, project, shot)

def getProjectShotNoteUrl(project, shot, pk, format='xml'):
    template = _getUrlComponent('url_project_shot_note')
    return template % (format, project, shot, pk)

def getProjectShotNotesUrl(project, shot, format='xml'):
    template = _getUrlComponent('url_project_shot_notes')
    return template % (format, project, shot)

def getProjectShotElementUrl(project, shot, pk, format='xml'):
    template = _getUrlComponent('url_project_shot_element')
    return template % (format, project, shot, pk)

def getProjectShotElementsUrl(project, shot, format='xml'):
    template = _getUrlComponent('url_project_shot_elements')
    return template % (format, project, shot)

def getProjectShotEventsUrl(project, shot, format='xml'):
    template = _getUrlComponent('url_project_shot_events')
    return template % (format, project, shot)

def getProjectShotFramesUrl(project, shot, format='xml'):
    template = _getUrlComponent('url_project_shot_frames')
    return template % (format, project, shot)

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
