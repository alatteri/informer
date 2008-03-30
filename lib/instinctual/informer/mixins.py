from datetime import datetime
from time import strptime, mktime

from django.db import models
from django.contrib.auth.models import User

# ------------------------------------------------------------------------------
class InformerBrandingField(models.FileField):
    """
    Provide a custom FileField for the branding element that does not remove
    the default file on disk.
    """
    def __init__(self, *args, **kwargs):
        self.__default = kwargs['default']
        models.FileField.__init__(self, *args, **kwargs)

    def delete_file(self, instance):
        file_name = getattr(instance, 'get_%s_filename' % self.name)()
        if not file_name.endswith(self.__default):
            models.FileField.delete_file(self, instance)

    def get_internal_type(self):
        return 'FileField'

# ------------------------------------------------------------------------------
class InformerMixIn(object):
    user_fields = ['created_by', 'modified_by', 'who']
    date_fields = ['created_on', 'modified_on']

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

    def __init__(self, *args, **kwargs):
        fields = [x.name for x in self._meta.fields]

        for f in self.user_fields:
            if f in kwargs and kwargs[f]:
                kwargs[f] = User.getUser(kwargs[f], create=True)

        for f in self.date_fields:
            if f in kwargs and kwargs[f]:
                kwargs[f] = self.getDateTime(kwargs[f])

        models.Model.__init__(self, *args, **kwargs)

    def __setattr__(self, key, val):
        if key in self.user_fields:
            val = User.getUser(val, create=True)
        elif key in self.date_fields:
            val = self.getDateTime(val)

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

class GetRender(GetOrCreateObject):
    def get(self, event, *args, **kwargs):
        r = self.model.objects.get(event=event)
        print "-" * 80
        print "Found Render %s" % (r.id)
        print "-" * 80
        return r

    def create(self, event, *args, **kwargs):
        print "-" * 80
        print "Now making new render!"
        print "-" * 80
        kwargs['movie_hi'] = kwargs['movie_lo'] = 'pending.mov'
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

