import os
from urlparse import urlparse
import logging
import logging.config
import sys, codecs
import ConfigParser

from datetime import datetime
if datetime.now() >  datetime(2008, 7, 1):
    raise ValueError("Trial expiration has expired.")

def getLogger(name):
    return _logger

def getConf():
    global conf
    return conf

def isDebug():
    c = getConf()
    if 'true' == str.lower(c.get('informer', 'debug')):
        return True
    else:
        return False

rootDir = os.sep.join(__file__.split(os.sep)[:-3])
confDir = os.path.join(rootDir, 'conf')
logIni = os.path.join(confDir, 'logging.ini')

CONF_DEFAULTS = {
    "db_engine":                  "sqlite3",
    "db_name":                    os.path.join(rootDir, 'data', 'informer.db'),
    "db_user":                    "informer",
    "db_pass":                    "informer",
    "db_host":                    "localhost",
    "db_port":                    "",
    "debug":                      "true",
    "dir_logs":                   os.path.join(rootDir, 'logs'),
    "dir_third_party_bin":        os.path.join(rootDir, 'third_party', 'bin'),
    "dir_uploads":                os.path.join(rootDir, 'uploads'),
    "dir_filelist_base":          "/tmp",
    "url_api":                    "informer/1.0",
    "url_frames":                 "frames/",
    "url_projects":               "projects/",
    "url_shots":                  "shots/",
    "url_project_print":          "projects/%s/print/",
    "url_project_shots":          "projects/%s/shots/",
    "url_project_shot":           "projects/%s/shots/%s/",
    "url_project_shot_logs":      "projects/%s/shots/%s/logs/",
    "url_project_shot_note":      "projects/%s/shots/%s/notes/%s/",
    "url_project_shot_notes":     "projects/%s/shots/%s/notes/",
    "url_project_shot_renders":   "projects/%s/shots/%s/renders/",
    "url_project_shot_events":    "projects/%s/shots/%s/events/",
    "url_project_shot_element":   "projects/%s/shots/%s/elements/%s/",
    "url_project_shot_elements":  "projects/%s/shots/%s/elements/",
    "url_users":                  "users/",
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
logging.codecs = codecs.getwriter('utf-8')
sys.stdout = codecs.getwriter('utf-8')(sys.stdout)

_logger = logging.getLogger('root')
_logfile = os.path.join(rootDir, 'logs', 'informer.log')
_handler = logging.FileHandler(_logfile)

if isDebug():
    _logger.setLevel(logging.DEBUG)
    _handler.setLevel(logging.DEBUG)
else:
    _logger.setLevel(logging.INFO)
    _handler.setLevel(logging.INFO)

_formater = logging.Formatter("%(asctime)s [%(levelname)s %(name)s %(module)s:%(lineno)d] %(message)s")
_handler.setFormatter(_formater)
_logger.addHandler(_handler)

