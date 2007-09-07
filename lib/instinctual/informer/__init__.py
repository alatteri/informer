import re
import instinctual

LOG = instinctual.getLogger(__name__)

TYPE_XML = 'xml'
TYPE_PYTHON = 'py'

def getServerRoot():
    conf = instinctual.getConf()
    proto = conf.get('informer', 'proto')
    server = conf.get('informer', 'server')
    port = conf.get('informer', 'port')
    return "%s://%s:%s" % (proto, server, port)

def _getUrlComponent(component):
    conf = instinctual.getConf()
    root = getServerRoot()
    return "%s/%s" % (root, conf.get('informer', component))

def getAppEventUrl(type):
    return _getUrlComponent('%s_app_event_url' % type)

def getProjectShotNotesUrl(type, project, shot):
    template = _getUrlComponent('%s_project_shot_notes_url' % type)
    return template % (project, shot)

def getProjectShotElementsUrl(type, project, shot):
    template = _getUrlComponent('%s_project_shot_elements_url' % type)
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
        return {'project': match.group(1), 'shot': match.group(2)}
    else:
        return None
