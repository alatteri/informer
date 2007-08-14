import logging
import logging.config
import sys, codecs

iniFile = '/usr/discreet/instinctual/informer/instinctual/informer/logging.ini'
logging.config.fileConfig(iniFile)
logging.codecs = codecs.getwriter('utf-8')
sys.stdout = codecs.getwriter('utf-8')(sys.stdout)

def getLogger(name):
    return logging.getLogger(name)
