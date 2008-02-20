import os
import re
import instinctual

LOG = instinctual.getLogger(__name__)

def relativeUrl(url):
    root = getServerRoot()
    if url.startswith(root + '/'):
        url = url[len(root)+1:]
    return url

def relativeApiUrl(url):
    url = relativeUrl(url)
    api = getApiFragment()
    if url.startswith(api + '/'):
        url = url[len(api)+1:]
    return url

def getServerRoot():
    conf = instinctual.getConf()
    return conf.get('informer', 'server')

def _getUrlComponent(component, format):
    conf = instinctual.getConf()
    root = getServerRoot()
    component = conf.get('informer', component)

    if 'html' == format:
        url = "%s/%s" % (root, component)
    else:
        api = getApiFragment()
        url = "%s/%s/%s/%s" % (root, api, format, component)
    return url

def getApiFragment():
    conf = instinctual.getConf()
    return conf.get('informer', 'url_api')

def getUsersUrl(format):
    template = _getUrlComponent('url_users', format)
    return template

def getShotsUrl(format):
    template = _getUrlComponent('url_shots', format)
    return template

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

def getProjectShotRendersUrl(project, shot, format):
    template = _getUrlComponent('url_project_shot_renders', format)
    return template % (project, shot)

"""
/x1/j_lo/batch/001_100/0001_comp-01.batch
    project: j_lo
    shot: 0001
    # sometimes sh0001 or sc0001
"""
_reProject = re.compile('/project/([^/]+)/')
def parseSetup(setup):
    parsed = {}
    match = _reProject.search(setup)
    if match != None:
        parsed['project'] = match.group(1)
    else:
        raise ValueError("Unable to parse setup (unknown project): %s" % (setup))

    # start with the batch file name
    shot = os.path.basename(setup)

    # remove the trailing .batch if present
    shot = re.sub('\.batch\.?$', '', shot)

    # remove trailing -001 etc (for Alan)
    shot = re.sub('-\w+$', '', shot)

    # remove trailing v3, b2 etc
    shot = re.sub('(?i)_?(v|b)\d\w*$', '', shot)

    parsed['shot'] = shot
    return parsed
