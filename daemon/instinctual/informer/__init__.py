import os
import logging
import logging.config
import sys, codecs
import ConfigParser

rootDir = os.sep.join(__file__.split(os.sep)[:-3])
confDir = os.path.join(rootDir, 'conf')
logIni = os.path.join(confDir, 'logging.ini')

logging.config.fileConfig(logIni)
logging.codecs = codecs.getwriter('utf-8')
sys.stdout = codecs.getwriter('utf-8')(sys.stdout)

confIni = os.path.join(confDir, 'instinctual.ini')
conf = ConfigParser.ConfigParser()
conf.read(confIni)

def getLogger(name):
    return logging.getLogger(name)

def getConf():
    global conf
    return conf

def getServerRoot():
    conf = getConf()
    proto = conf.get('informer', 'proto')
    server = conf.get('informer', 'server')
    port = conf.get('informer', 'port')
    return "%s://%s:%s" % (proto, server, port)

def _getUrlComponent(component):
    conf = getConf()
    root = getServerRoot()
    return "%s/%s" % (root, conf.get('informer', component))

def getAppEventUrl():
    return _getUrlComponent('app_event_url')
