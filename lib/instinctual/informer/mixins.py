from datetime import datetime
from time import strptime, mktime

from django.db import models
from django.contrib.auth.models import User

# ------------------------------------------------------------------------------
class InformerMixIn(object):
    user_fields = ['created_by', 'modified_by', 'who']
    date_fields = ['created_on', 'modified_on']

    def calculateDelta(self, clientNow):
        serverNow = datetime.now()
        print "Server says now is: ", serverNow
        print "Client says now is: ", clientNow
        self._delta = (serverNow - clientNow)
        print "The delta is: %s" % (self._delta)

    def strToDateTime(self, val):
        dot = val.rindex('.')
        t = val[0:dot]
        time = strptime(t, "%Y-%m-%d %H:%M:%S")[0:6]
        ms = int(float(val[dot:]) * 10**6)
        time = list(time)
        time.append(ms)
        return datetime(*time)

    def getDateTime(self, val):
        if isinstance(val, datetime):
            return val
        elif isinstance(val, str):
            return self.strToDateTime(val)
        else:
            # TODO: raise ValueError...
            return val

    def applyDelta(self, val):
        if self._delta:
            print "Applying delta of %s" % (self._delta)
            result = val + self._delta
            print "Relative = %s" % (result)
            return result
        else:
            return val

    def __init__(self, *args, **kwargs):
        self._delta = None
        fields = [x.name for x in self._meta.fields]

        for f in self.user_fields:
            if f in kwargs and kwargs[f]:
                kwargs[f] = User.getUser(kwargs[f], create=True)

        for f in self.date_fields:
            if f in kwargs and kwargs[f]:
                # convert the time, save it as raw_<field> and apply the
                # delta for <field>
                raw = self.getDateTime(kwargs[f])
                if 'raw_' + f in fields:
                    # make sure raw_ + f is in fields
                    kwargs['raw_' + f] = raw
                kwargs[f] = self.applyDelta(raw)

        models.Model.__init__(self, *args, **kwargs)

    def __setattr__(self, key, val):
        if key in self.user_fields:
            val = User.getUser(val, create=True)
        elif key in self.date_fields:
            raw = self.getDateTime(val)
            self.__setattr__('raw_' + key, raw)
            val = self.applyDelta(raw)

        # print "__setattr__ [%s] [%s] (%s)" % (key, val, type(val))
        return models.Model.__setattr__(self, key, val)

# ------------------------------------------------------------------------------
class GetOrCreateObject(object):
    def __init__(self, model):
        self.model = model

    def __call__(self, *args, **kwargs):
        if 'create' in kwargs:
            create = kwargs['create']
            del kwargs['create']
        else:
            create = False

        if args and isinstance(args[0], self.model):
            print "%s was of type %s" % (args[0], self.model)
            return args[0]
        try:
            # print "looking up %s" % (self.model)
            return self.get(*args, **kwargs)
        except self.model.DoesNotExist, e:
            if create:
                print "creating %s" % (self.model)
                o = self.create(*args, **kwargs)
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

class GetClip(GetOrCreateObject):
    def get(self, event, *args, **kwargs):
        c = self.model.objects.get(event=event)
        print "-" * 80
        print "Found clip %s" % (c.id)
        print "-" * 80
        return c

    def create(self, event, *args, **kwargs):
        print "-" * 80
        print "Now making new clip!"
        print "-" * 80
        kwargs['movie_hi'] = kwargs['movie_lo'] = 'pending'
        return self.model(event=event, **kwargs)

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

