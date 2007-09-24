import os
import sys

os.environ['DJANGO_SETTINGS_MODULE'] = 'instinctual.settings'

projectDir = os.sep.join(__file__.split(os.sep)[:-2])
thirdParty = os.sep.join([projectDir, 'third_party', 'lib'])
sys.path.append(thirdParty)
