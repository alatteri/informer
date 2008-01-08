import os
import sys

os.environ['DJANGO_SETTINGS_MODULE'] = 'instinctual.settings'

projectDir = os.sep.join(__file__.split(os.sep)[:-2])
thirdParty = os.sep.join([projectDir, 'third_party', 'python'])
sys.path.insert(0, thirdParty)
print "path:", sys.path
