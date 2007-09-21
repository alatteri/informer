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

def getAppEventUrl():
    return _getUrlComponent('url_app_event')

def getProjectShotNoteUrl(project, shot, pk):
    template = _getUrlComponent('url_project_shot_note')
    return template % (project, shot, pk)

def getProjectShotNotesUrl(project, shot):
    template = _getUrlComponent('url_project_shot_notes')
    return template % (project, shot)

def getProjectShotElementsUrl(project, shot):
    template = _getUrlComponent('url_project_shot_elements')
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
        return None
