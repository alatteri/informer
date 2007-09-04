import os
import sys


libDir = os.sep.join(__file__.split(os.sep)[:-1])
thirdParty = os.sep.join([libDir, 'third_party'])
sys.path.append(thirdParty)
