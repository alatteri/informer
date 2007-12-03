from pprint import pprint
from datetime import datetime
from time import strptime, mktime

from django.db import models
from django.contrib.auth.models import User

import instinctual
from instinctual import informer
from instinctual.informer.mixins import InformerMixIn
from instinctual.informer.mixins import GetUser, GetProject, GetShot, GetClip, GetEvent

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
    setup       = models.CharField(maxlength=4096)

    def get_absolute_url(self):
        project = self.project.name
        return informer.getProjectShotUrl(project, self.name, format='html')

    def get_json_note_url(self):
        project = self.project.name
        return informer.getProjectShotNotesUrl(project, self.name, format='json')

    def get_json_element_url(self):
        project = self.project.name
        return informer.getProjectShotElementsUrl(project, self.name, format='json')

    class Meta:
        unique_together = (('project', 'name'),)

    class Admin:
        list_display = ('project', 'name', 'setup', 'description')

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

    def __str__(self):
            return self.text

# ------------------------------------------------------------------------------
class Event(InformerMixIn, models.Model):
    shot           = models.ForeignKey(Shot)
    type           = models.CharField(maxlength=255)
    host           = models.CharField(maxlength=255)
    created_by     = models.ForeignKey(User)
    created_on     = models.DateTimeField('date created')
    raw_created_on = models.DateTimeField('date created (unadjusted)')

    class Meta:
        unique_together = (('shot', 'type', 'created_on'),)

    class Admin:
        list_display = ('id', 'shot', 'type', 'host',
                        'created_by', 'created_on', 'raw_created_on')

    def __str__(self):
        return "Event %s (%s, %s)" % (self.id, self.type, self.created_by)

# ------------------------------------------------------------------------------
class Clip(InformerMixIn, models.Model):
    event = models.ForeignKey(Event)
    spark = models.CharField(maxlength=255)
    movie = models.FileField(upload_to='clips')

    start = models.IntegerField()
    end = models.IntegerField()

    # storing fps as text to avoid floating point precision issues
    rate = models.CharField(maxlength=32)

    class Admin:
        list_display = ('id', 'event', 'spark', 'movie', 'start', 'end', 'rate')

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
# monkey patched model methods
# ------------------------------------------------------------------------------
Clip.getClip = GetClip(Clip)
Shot.getShot = GetShot(Shot)
User.getUser = GetUser(User)
Event.getEvent = GetEvent(Event)
Project.getProject = GetProject(Project)
