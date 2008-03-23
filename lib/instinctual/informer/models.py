import os
from pprint import pprint
from datetime import datetime
from time import strptime, mktime

from django.db import models
from django.contrib.auth.models import User

from django.db.models import signals
from django.dispatch import dispatcher

import instinctual
from instinctual import informer
from instinctual import settings
from instinctual.informer.mixins import InformerMixIn, InformerBrandingField
from instinctual.informer.mixins import GetUser, GetProject, GetShot, GetRender, GetEvent
from instinctual.informer.signals import Handler, IgnoreSignalException

# ------------------------------------------------------------------------------
class Project(models.Model):
    name        = models.CharField(maxlength=255, unique=True)
    description = models.CharField(maxlength=4096, null=True, blank=True)
    status      = models.CharField(maxlength=64, null=True, blank=True)
    client      = models.CharField(maxlength=255, null=True, blank=True)
    due_date    = models.DateTimeField('due date', null=True, blank=True)
    branding    = InformerBrandingField(upload_to='branding', default='branding/default.gif')

    def get_absolute_url(self):
        return informer.getProjectShotsUrl(self.name, format='html')

    class Admin:
        list_display = ('name', 'description', 'status', 'client', 'due_date', 'branding')

    def __str__(self):
        if self.description:
            return "%s: %s" % (self.name, self.description)
        else:
            return self.name

    def get_absolute_shots_url(self):
        return informer.getProjectShotsUrl(self.name, format='html')

    def getNameFromRequest(cls, request):
        return request.path.split("/")[5]
    getNameFromRequest = classmethod(getNameFromRequest)

    def _shot_count(self):
        return len(Shot.objects.filter(project=self))

    shot_count = property(_shot_count)
    url = property(get_absolute_url)

    class Rest:
        expose_fields = ['name', 'description', 'status', 'client', 'due_date', 'branding',
                         'self.shot_count', 'self.url']

# ------------------------------------------------------------------------------
class Shot(models.Model):
    project     = models.ForeignKey(Project)
    name        = models.CharField('shot', maxlength=255)
    description = models.CharField(maxlength=4096, null=True, blank=True)
    status      = models.CharField(maxlength=64, null=True, blank=True)
    handles     = models.CharField(maxlength=32, null=True, blank=True)
    frames      = models.IntegerField(default=0)

    # --------------------
    def get_absolute_url(self):
        project = self.project.name
        return informer.getProjectShotUrl(project, self.name, format='html')

    def get_absolute_notes_url(self):
        project = self.project.name
        return informer.getProjectShotNotesUrl(project, self.name, format='html')

    def get_absolute_elements_url(self):
        project = self.project.name
        return informer.getProjectShotElementsUrl(project, self.name, format='html')

    def get_absolute_renders_url(self):
        project = self.project.name
        return informer.getProjectShotRendersUrl(project, self.name, format='html')

    # --------------------
    def get_json_logs_url(self):
        project = self.project.name
        return informer.getProjectShotLogsUrl(project, self.name, format='json')

    def get_json_notes_url(self):
        project = self.project.name
        return informer.getProjectShotNotesUrl(project, self.name, format='json')

    def get_json_elements_url(self):
        project = self.project.name
        return informer.getProjectShotElementsUrl(project, self.name, format='json')

    def get_json_renders_url(self):
        project = self.project.name
        return informer.getProjectShotRendersUrl(project, self.name, format='json')

    # --------------------------------------------------------------------------
    def _get_render_event(self):
        if hasattr(self, '_render_event'):
            return self._render_event

        e = Event.objects.filter(type='BATCH PROCESS', shot=self).order_by('-created_on')
        self._render_event = len(e) and e[0] or None
        return self._render_event

    def _get_render_obj(self):
        if hasattr(self, '_render_obj'):
            return self._render_obj

        e = self._get_render_event()
        if e:
            r = Render.objects.filter(event=e, shot=self)
            self._render_obj = len(r) and r[0] or None
        else:
            self._render_obj = None
        return self._render_obj

    def get_render_artist(self):
        e = self._get_render_event()
        return e and e.created_by or None

    def get_render_time(self):
        e = self._get_render_event()
        return e and e.created_on or None

    def get_render_machine(self):
        e = self._get_render_event()
        return e and e.host or None

    def get_render_setup(self):
        e = self._get_render_event()
        return e and e.setup or None

    def get_render_movie_hi_url(self):
        r = self._get_render_obj()
        # TODO: what movie should this return if there aren't any?
        return r and r.get_movie_hi_url() or settings.MEDIA_URL + 'pending.mov'

    def get_render_movie_lo_url(self):
        r = self._get_render_obj()
        # TODO: what movie should this return if there aren't any?
        return r and r.get_movie_lo_url() or settings.MEDIA_URL + 'pending.mov'

    def get_render_id(self):
        r = self._get_render_obj()
        return r and r.id or None

    # --------------------------------------------------------------------------
    url = property(get_absolute_url)
    render_movie_hi_url = property(get_render_movie_hi_url)
    render_movie_lo_url = property(get_render_movie_hi_url)
    last_modified = property(get_render_time)
    last_modified_by = property(get_render_artist)

    # --------------------------------------------------------------------------
    class Meta:
        unique_together = (('project', 'name'),)

    class Admin:
        list_display = ('project', 'name', 'status', 'description', 'handles', 'frames')

    class Logger:
        def created(cls, instance, *args, **kwargs):
            return ('Created this shot', '', '')
        created = classmethod(created)

        def updated(cls, instance, *args, **kwargs):
            return ('Updated this shot', '', '')
        updated = classmethod(updated)

    class Rest:
        expose_fields = ['project', 'name', 'status', 'description', 'handles', 'frames',
                         'self.url', 'self.render_movie_hi_url', 'self.render_movie_lo_url',
                         'self.last_modified', 'self.last_modified_by']

    def __str__(self):
        if self.description:
            return "%s: %s" % (self.name, self.description)
        else:
            return self.name

    def getNameFromRequest(cls, request):
        return request.path.split("/")[7]
    getNameFromRequest = classmethod(getNameFromRequest)

# ------------------------------------------------------------------------------
class Note(InformerMixIn, models.Model):
    shot        = models.ForeignKey(Shot)
    text        = models.CharField('text', maxlength=4096)
    is_checked  = models.BooleanField('completed', default=False)
    assigned_to = models.ForeignKey(User, null=True, blank=True, related_name='assigned')
    created_on  = models.DateTimeField('date created', auto_now_add=True)
    created_by  = models.ForeignKey(User)
    modified_on = models.DateTimeField('date modified', auto_now=True)
    modified_by = models.ForeignKey(User, related_name='modified_note_set')

    class Admin:
        list_display = ('shot', 'is_checked', 'text', 'created_by',
                        'created_on', 'modified_by', 'modified_on')
    class Logger:
        def created(cls, instance, *args, **kwargs):
            return ('Commented', instance.text, '')
        created = classmethod(created)

        def updated(cls, instance, old, new, *args, **kwargs):
            if 'is_checked' not in old:
                raise IgnoreSignalException()

            if new['is_checked']:
                comment = 'as complete'
            else:
                comment = 'as not complete'

            return ('Marked comment', instance.text, comment)
        updated = classmethod(updated)

        def deleted(cls, instance, *args, **kwargs):
            return ('Deleted comment', instance.text, '')
        deleted = classmethod(deleted)

    class Rest:
        expose_fields = ['text', 'is_checked', 'created_by', 'created_on',
                         'modified_by', 'modified_on', 'assigned_to']

    def __str__(self):
        return self.text

# ------------------------------------------------------------------------------
class Element(InformerMixIn, models.Model):
    shot        = models.ForeignKey(Shot)
    kind        = models.CharField(maxlength=32)
    text        = models.CharField(maxlength=4096)
    is_checked  = models.BooleanField('is checked', default=False)
    created_on  = models.DateTimeField('date created', auto_now_add=True)
    created_by  = models.ForeignKey(User)

    class Admin:
        list_display = ('shot', 'kind', 'is_checked', 'text',
                        'created_by', 'created_on')

    class Logger:
        def created(cls, instance, *args, **kwargs):
            return ('Checked in a new element', '', '')
        created = classmethod(created)

    class Rest:
        expose_fields = ['text', 'kind', 'is_checked', 'created_by', 'created_on']

    def __str__(self):
            return self.text

# ------------------------------------------------------------------------------
class Event(InformerMixIn, models.Model):
    shot           = models.ForeignKey(Shot)
    type           = models.CharField(maxlength=255)
    host           = models.CharField(maxlength=255)
    setup          = models.CharField(maxlength=4096)
    created_by     = models.ForeignKey(User)
    created_on     = models.DateTimeField('date created')
    raw_created_on = models.DateTimeField('date created (unadjusted)')

    class Meta:
        unique_together = (('shot', 'type', 'created_on'),)

    class Admin:
        list_display = ('id', 'shot', 'type', 'host', 'setup',
                        'created_by', 'created_on', 'raw_created_on')

    def __str__(self):
        return "Event %s (%s, %s)" % (self.id, self.type, self.created_by)

# ------------------------------------------------------------------------------
class Render(InformerMixIn, models.Model):
    shot  = models.ForeignKey(Shot)
    event = models.ForeignKey(Event)
    created_on = models.DateTimeField('date created', auto_now_add=True)
    modified_on = models.DateTimeField('date modified', auto_now=True)
    is_pending = models.BooleanField('pending', default=True)

    spark = models.CharField(maxlength=255)
    movie_hi = models.FileField(upload_to='movies/%Y/%m/%d')
    movie_lo = models.FileField(upload_to='movies/%Y/%m/%d')

    start = models.IntegerField()
    end = models.IntegerField()

    # storing fps as text to avoid floating point precision issues
    rate = models.CharField(maxlength=32)

    class Admin:
        list_display = ('id', 'shot', 'event', 'movie_hi', 'movie_lo',
                        'start', 'end', 'rate', 'created_on', 'is_pending')
    class Rest:
        expose_fields = ['created_on', 'modified_on', 'movie_hi', 'movie_lo',
                         'start', 'end', 'rate',
                         'event__created_by', 'event__host', 'event__setup']
    class Logger:
        def created(cls, instance, *args, **kwargs):
            return ('Created a new render', '', '')
        created = classmethod(created)

    def __str__(self):
        return "Render %s" % (self.id)

# ------------------------------------------------------------------------------
class Frame(InformerMixIn, models.Model):
    shot = models.ForeignKey(Shot)
    render = models.ForeignKey(Render)
    image = models.FileField(upload_to='frames/%Y/%m/%d')
    in_render = models.BooleanField('in render', default=False)

    host        = models.CharField(maxlength=255)
    created_on  = models.DateTimeField('date created')
    created_by  = models.ForeignKey(User)
    raw_created_on = models.DateTimeField('date created (unadjusted)')

    number = models.IntegerField()

    width = models.IntegerField()
    height = models.IntegerField()
    depth = models.IntegerField()

    resized_width = models.IntegerField()
    resized_height = models.IntegerField()
    resized_depth = models.IntegerField()

    class Meta:
        unique_together = (('render', 'number'),)

    class Admin:
        list_display = ('id', 'in_render', 'render', 'number', 'image', 'host',
                        'created_on', 'created_by', 'raw_created_on')

    def getOrCreateParentRender(self, start, end, rate, spark):
        print "getOrCreateParentRender called for frame:"
        pprint(self.__dict__)

        print "Going to look for BATCH PROCESS < %s" % (self.created_on)
        e = Event.objects.filter(type='BATCH PROCESS',
                                 created_by=self.created_by,
                                 raw_created_on__lte=self.raw_created_on,
                                 host=self.host).order_by('-raw_created_on')[0]

        print "Hey! Found e... it is: %s" % (e)
        pprint(e.__dict__)

        r = Render.getRender(event=e,
                             shot=e.shot,
                             start=start,
                             end=end,
                             rate=rate,
                             spark=spark,
                             create=True)

        if start < r.start:
            print "+++ setting r.start to", r.start
            r.start = start
        if end > r.end:
            print "+++ setting r.end to", r.end
            r.end = end

        r.save()

        print "*" * 80
        print "The render id is:", r.id
        print "*" * 80

        return r

# ------------------------------------------------------------------------------
class Log(InformerMixIn, models.Model):
    shot        = models.ForeignKey(Shot)
    who         = models.ForeignKey(User, null=True, blank=True)
    when        = models.DateTimeField('date occurred', auto_now_add=True)
    action      = models.CharField(maxlength=32)
    type        = models.CharField(maxlength=32)
    object_id   = models.IntegerField()
    msg_prefix  = models.CharField(maxlength=4096)
    object_repr = models.CharField(maxlength=4096)
    msg_suffix  = models.CharField(maxlength=4096)

    class Admin:
        list_display = ('shot', 'who', 'when', 'action', 'type', 'object_id',
                        'msg_prefix', 'object_repr', 'msg_suffix')

    class Rest:
        expose_fields = ['who', 'when', 'action', 'type', 'object_id',
                         'msg_prefix', 'object_repr', 'msg_suffix']

# ------------------------------------------------------------------------------
# monkey patched model methods
# ------------------------------------------------------------------------------
Render.getRender = GetRender(Render)
Shot.getShot = GetShot(Shot)
User.getUser = GetUser(User)
Event.getEvent = GetEvent(Event)
Project.getProject = GetProject(Project)

# ------------------------------------------------------------------------------
# signal handler processing
# ------------------------------------------------------------------------------
h = Handler()
dispatcher.connect(h.handle_post_init,  signal=signals.post_init)
dispatcher.connect(h.handle_pre_save,   signal=signals.pre_save)
dispatcher.connect(h.handle_post_save,  signal=signals.post_save)
dispatcher.connect(h.handle_pre_delete, signal=signals.pre_delete)

def getHandler():
    return h
