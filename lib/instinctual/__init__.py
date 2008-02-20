import os
from urlparse import urlparse
import logging
import logging.config
import sys, codecs
import ConfigParser

rootDir = os.sep.join(__file__.split(os.sep)[:-3])
confDir = os.path.join(rootDir, 'conf')
logIni = os.path.join(confDir, 'logging.ini')

CONF_DEFAULTS = {
    "url_api":                    "informer/1.0",
    "url_projects":               "projects/",
    "url_shots":                  "shots/",
    "url_project_shots":          "projects/%s/shots/",
    "url_project_shot":           "projects/%s/shots/%s/",
    "url_project_shot_logs":      "projects/%s/shots/%s/logs/",
    "url_project_shot_note":      "projects/%s/shots/%s/notes/%s/",
    "url_project_shot_notes":     "projects/%s/shots/%s/notes/",
    "url_project_shot_renders":   "projects/%s/shots/%s/renders/",
    "url_project_shot_events":    "projects/%s/shots/%s/events/",
    "url_project_shot_frames":    "projects/%s/shots/%s/frames/",
    "url_project_shot_element":   "projects/%s/shots/%s/elements/%s/",
    "url_project_shot_elements":  "projects/%s/shots/%s/elements/",
    "url_users":                  "users/",
    "dir_logs":                   os.path.join(rootDir, 'logs'),
    "dir_uploads":                os.path.join(rootDir, 'uploads'),
    "dir_filelist_base":          "/tmp",
}

confIni = os.path.join(confDir, 'instinctual.ini')
conf = ConfigParser.ConfigParser(defaults=CONF_DEFAULTS)
conf.read(confIni)

# --------------------
# support the urlparse of python 2.3 as well...
#
o = urlparse(conf.get('informer', 'server'))
proto = o[0]
match = o[1].find(':')
if -1 != match:
    hostname = o[1][:match]
    port = o[1][match+1:]
else:
    hostname = o[1]
    port = ''
conf.set('informer', 'hostname', hostname)
conf.set('informer', 'proto', proto)
conf.set('informer', 'port', port)

# --------------------
# init the logger
#
logging.config.fileConfig(logIni)
logging.codecs = codecs.getwriter('utf-8')
sys.stdout = codecs.getwriter('utf-8')(sys.stdout)

def getLogger(name):
    return logging.getLogger(name)

def getConf():
    global conf
    return conf
