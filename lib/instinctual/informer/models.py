from pprint import pprint
from datetime import datetime
from time import strptime, mktime

from django.db import models
from django.contrib.auth.models import User

from django.db.models import signals
from django.dispatch import dispatcher

import instinctual
from instinctual import informer
from instinctual.informer.mixins import InformerMixIn
from instinctual.informer.mixins import GetUser, GetProject, GetShot, GetClip, GetEvent
from instinctual.informer.signals import Handler, IgnoreSignalException

# ------------------------------------------------------------------------------
class Project(models.Model):
    name        = models.CharField(maxlength=255, unique=True)
    description = models.CharField(maxlength=4096, null=True, blank=True)

    def get_absolute_url(self):
        return informer.getProjectShotsUrl(self.name, format='html')

    class Admin:
        list_display = ('name', 'description')

    def __str__(self):
        if self.description:
            return "%s: %s" % (self.name, self.description)
        else:
            return self.name

    def getNameFromRequest(cls, request):
        return request.path.split("/")[5]
    getNameFromRequest = classmethod(getNameFromRequest)

# ------------------------------------------------------------------------------
class Shot(models.Model):
    project     = models.ForeignKey(Project)
    name        = models.CharField('shot', maxlength=255)
    description = models.CharField(maxlength=4096, null=True, blank=True)

    def get_absolute_url(self):
        project = self.project.name
        return informer.getProjectShotUrl(project, self.name, format='html')

    def get_json_logs_url(self):
        project = self.project.name
        return informer.getProjectShotLogsUrl(project, self.name, format='json')

    def get_json_notes_url(self):
        project = self.project.name
        return informer.getProjectShotNotesUrl(project, self.name, format='json')

    def get_json_elements_url(self):
        project = self.project.name
        return informer.getProjectShotElementsUrl(project, self.name, format='json')

    def get_json_clips_url(self):
        project = self.project.name
        return informer.getProjectShotClipsUrl(project, self.name, format='json')

    # need to settle on if these are "clips" or "renders"
    get_json_renders_url = get_json_clips_url

    class Meta:
        unique_together = (('project', 'name'),)

    class Admin:
        list_display = ('project', 'name', 'description')

    class Logger:
        def created(cls, instance, *args, **kwargs):
            return ('Created this shot', '', '')
        created = classmethod(created)

        def updated(cls, instance, *args, **kwargs):
            return ('Updated this shot', '', '')
        updated = classmethod(updated)

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
                         'modified_by', 'modified_on']

    def __str__(self):
        return self.text

# ------------------------------------------------------------------------------
class Element(InformerMixIn, models.Model):
    shot        = models.ForeignKey(Shot)
    kind        = models.CharField(maxlength=32)
    text        = models.CharField(maxlength=4096)
    is_checked  = models.BooleanField('is checked', default=False)
    start_frame = models.IntegerField()
    end_frame   = models.IntegerField()
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
class Clip(InformerMixIn, models.Model):
    shot  = models.ForeignKey(Shot)
    event = models.ForeignKey(Event)
    created_on = models.DateTimeField('date created', auto_now_add=True)
    modified_on = models.DateTimeField('date modified', auto_now=True)

    spark = models.CharField(maxlength=255)
    movie = models.FileField(upload_to='clips')

    start = models.IntegerField()
    end = models.IntegerField()

    # storing fps as text to avoid floating point precision issues
    rate = models.CharField(maxlength=32)

    class Admin:
        list_display = ('id', 'shot', 'event', 'spark', 'movie',
                        'start', 'end', 'rate', 'created_on')
    class Rest:
        expose_fields = ['created_on', 'modified_on', 'movie', 'start', 'end', 'rate',
                         'event__created_by', 'event__host', 'event__setup']

    def __str__(self):
        return "Clip %s" % (self.id)

# ------------------------------------------------------------------------------
class Frame(InformerMixIn, models.Model):
    shot = models.ForeignKey(Shot)
    clip = models.ForeignKey(Clip)
    image = models.FileField(upload_to='frames')
    in_clip = models.BooleanField('in clip', default=False)

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
        unique_together = (('clip', 'number'),)

    class Admin:
        list_display = ('id', 'host', 'image', 'clip', 'in_clip',
                        'created_on', 'created_by', 'raw_created_on',
                        'number', 'width', 'height', 'depth',
                        'resized_width', 'resized_height', 'resized_depth')

    def getOrCreateParentClip(self, start, end, rate, spark):
        print "getOrCreateParentClip called for frame:"
        pprint(self.__dict__)

        print "Going to look for BATCH PROCESS < %s" % (self.created_on)
        e = Event.objects.filter(type='BATCH PROCESS',
                                 created_by=self.created_by,
                                 raw_created_on__lte=self.raw_created_on,
                                 host=self.host).order_by('-raw_created_on')[0]

        print "Hey! Found e... it is: %s" % (e)
        pprint(e.__dict__)

        c = Clip.getClip(event=e,
                         start=start,
                         end=end,
                         rate=rate,
                         spark=spark,
                         create=True)

        if start < c.start:
            print "+++ setting c.start to", c.start
            c.start = start
        if end > c.end:
            print "+++ setting c.end to", c.end
            c.end = end

        c.save()

        print "*" * 80
        print "The clip id is:", c.id
        print "*" * 80

        return c

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

# ------------------------------------------------------------------------------
# monkey patched model methods
# ------------------------------------------------------------------------------
Clip.getClip = GetClip(Clip)
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
