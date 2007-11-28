from datetime import datetime
from time import strptime, mktime

from django.db import models
from django.contrib.auth.models import User

import instinctual
from instinctual import informer
from instinctual.informer.mixins import InformerMixIn
from instinctual.informer.mixins import GetUser, GetProject, GetShot, GetEvent

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
        list_display = ('shot', 'kind', 'is_checked', 'text', 'created_by', 'created_on')

    def __str__(self):
            return self.text

# ------------------------------------------------------------------------------
class Event(InformerMixIn, models.Model):
    shot        = models.ForeignKey(Shot)
    type        = models.CharField(maxlength=255)
    host        = models.CharField(maxlength=255)
    outputs     = models.CharField(maxlength=4096, null=True, blank=True)
    created_by  = models.ForeignKey(User)
    created_on  = models.DateTimeField('date created')

    class Meta:
        unique_together = (('shot', 'type', 'created_on'),)

    class Admin:
        list_display = ('shot', 'type', 'outputs', 'host',
                        'created_by', 'created_on')

    def __str__(self):
        return "%s - %s - %s" % (self.type, self.created_by, self.shot.name)

# ------------------------------------------------------------------------------
class Frame(InformerMixIn, models.Model):
    shot = models.ForeignKey(Shot)
    spark = models.CharField(maxlength=255)
    image = models.FileField(upload_to='frames')

    host        = models.CharField(maxlength=255)
    created_on  = models.DateTimeField('date created')
    created_by  = models.ForeignKey(User)

    width = models.IntegerField()
    height = models.IntegerField()
    depth = models.IntegerField()

    resized_width = models.IntegerField()
    resized_height = models.IntegerField()
    resized_depth = models.IntegerField()

    start = models.IntegerField()
    number = models.IntegerField()
    end = models.IntegerField()

    # storing fps as text to avoid floating point precision issues
    rate = models.CharField(maxlength=32)

    class Admin:
        list_display = ('spark', 'host', 'image', 'created_on', 'created_by',
                        'width', 'height', 'depth',
                        'resized_width', 'resized_height', 'resized_depth',
                        'start', 'number', 'end', 'rate')

# ------------------------------------------------------------------------------
# monkey patched model methods
# ------------------------------------------------------------------------------
Shot.getShot = GetShot(Shot)
User.getUser = GetUser(User)
Event.getEvent = GetEvent(Event)
Project.getProject = GetProject(Project)
