from datetime import datetime
from time import strptime, mktime

from django.db import models
from django.contrib.auth.models import User

# ------------------------------------------------------------------------------
class InformerMixIn(object):
    user_fields = ['created_by', 'modified_by']
    date_fields = ['created_on', 'modified_on']

    def calculateDelta(self, clientNow):
        serverNow = datetime.now()
        print "Server says now is: ", serverNow
        print "Client says now is: ", clientNow
        self._delta = (serverNow - clientNow)

    def strToDateTime(self, val):
        val = val[0:val.rindex('.')]
        try:
            # would be nice to have only one format...
            return datetime(*strptime(val, "%m/%d/%y:%H:%M:%S")[0:6])
        except ValueError, e:
            return datetime(*strptime(val, "%Y-%m-%d %H:%M:%S")[0:6])

    def getDateTime(self, val):
        if isinstance(val, datetime):
            return val
        elif isinstance(val, str):
            return self.strToDateTime(val)
        else:
            # TODO: raise ValueError...
            return val

    def getRelativeDateTime(self, val):
        val = self.getDateTime(val)
        if self._delta:
            return val + self._delta
        else:
            return val

    def __init__(self, *args, **kwargs):
        self._delta = None

        print "__init__ [%s]" % (kwargs)
        for f in self.user_fields:
            if f in kwargs and kwargs[f]:
                kwargs[f] = User.getUser(kwargs[f], create=True)
        for f in self.date_fields:
            if f in kwargs and kwargs[f]:
                kwargs[f] = self.getRelativeDateTime(kwargs[f])
        models.Model.__init__(self, *args, **kwargs)

    def __setattr__(self, key, val):
        print "__setattr__ [%s] [%s] (%s)" % (key, val, type(val))
        if key in self.user_fields:
            val = User.getUser(val, create=True)
        elif key in self.date_fields:
            val = self.getRelativeDateTime(val)
        return models.Model.__setattr__(self, key, val)

# ------------------------------------------------------------------------------
class GetOrCreateObject(object):
    def __init__(self, model):
        self.model = model

    def __call__(self, obj, *args, **kwargs):
        if 'create' in kwargs:
            create = kwargs['create']
            del kwargs['create']
        else:
            create = False

        if isinstance(obj, self.model):
            print "%s was of type %s" % (obj, self.model)
            return obj
        try:
            print "looking up %s" % (obj)
            return self.get(obj, *args, **kwargs)
        except self.model.DoesNotExist, e:
            if create:
                print "creating %s" % (obj)
                o = self.create(obj, *args, **kwargs)
                o.save()
                return o
            else:
                raise e

class GetUser(GetOrCreateObject):
    def get(self, user, *args, **kwargs):
        return self.model.objects.get(username=user)

    def create(self, user, *args, **kwargs):
        return self.model(username=user, password='x')

class GetProject(GetOrCreateObject):
    def get(self, proj, *args, **kwargs):
        return self.model.objects.get(name=proj)

    def create(self, proj, *args, **kwargs):
        return self.model(name=proj)

class GetShot(GetOrCreateObject):
    def get(self, shot, proj, *args, **kwargs):
        return self.model.objects.get(name=shot, project=proj)

    def create(self, shot, proj, *args, **kwargs):
        return self.model(name=shot, project=proj, **kwargs)

class GetEvent(GetOrCreateObject):
    def get(self, shot, type, created_on, *args, **kwargs):
        return self.model.objects.get(shot=shot, type=type, created_on=created_on)

    def create(self, shot, type, created_on, *args, **kwargs):
        return self.model(shot=shot, type=type, created_on=created_on, **kwargs)

