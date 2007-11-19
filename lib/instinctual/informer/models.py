from django.db import models
from django.contrib.auth.models import User

import instinctual
from instinctual import informer

def getOrCreateUser(username):
    # if it's a User, all is well
    if isinstance(username, User):
        return username
    try:
        return User.objects.get(username=username)
    except User.DoesNotExist:
        print "no. did not find the user."
        user = User(username=username, password='x')
        user.save()
        print "ok! just created user %s" % (username)
        return user

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

    class Meta:
        unique_together = (('project', 'name'),)

    class Admin:
        list_display = ('project', 'name', 'setup', 'description')

    def __str__(self):
        if self.description:
            return "%s: %s" % (self.name, self.description)
        else:
            return self.name

# ------------------------------------------------------------------------------
class Note(models.Model):
    shot        = models.ForeignKey(Shot)
    text        = models.CharField('text', maxlength=4096)
    is_checked  = models.BooleanField('completed', default=False)
    created_on  = models.DateTimeField('date created', auto_now_add=True)
    created_by  = models.ForeignKey(User)
    modified_on = models.DateTimeField('date modified', auto_now=True)
    modified_by = models.ForeignKey(User, related_name='modified_note_set')

    def __setattr__(self, key, val):
        if key in ('created_by', 'modified_by'):
            val = getOrCreateUser(val)
        return models.Model.__setattr__(self, key, val)

    class Admin:
        list_display = ('shot', 'is_checked', 'text', 'created_by',
                        'created_on', 'modified_by', 'modified_on')

    def __str__(self):
        return self.text

# ------------------------------------------------------------------------------
class Element(models.Model):
    shot        = models.ForeignKey(Shot)
    kind        = models.CharField(maxlength=32)
    text        = models.CharField(maxlength=4096)
    is_checked  = models.BooleanField('is checked', default=False)
    created_on  = models.DateTimeField('date created', auto_now_add=True)
    created_by  = models.ForeignKey(User)

    def __setattr__(self, key, val):
        if key in ('created_by'):
            val = getOrCreateUser(val)
        return models.Model.__setattr__(self, key, val)

    class Admin:
        list_display = ('shot', 'kind', 'is_checked', 'text', 'created_by', 'created_on')

    def __str__(self):
            return self.text

# ------------------------------------------------------------------------------
class Event(models.Model):
    shot        = models.ForeignKey(Shot)
    type        = models.CharField(maxlength=255)
    host        = models.CharField(maxlength=255)
    user        = models.ForeignKey(User)
    # date_added  = models.DateTimeField('date added', auto_now_add=True)
    date_added  = models.DateTimeField('date')

    def __setattr__(self, key, val):
        if key in ('user'):
            val = getOrCreateUser(val)
        return models.Model.__setattr__(self, key, val)

    class Meta:
        unique_together = (('shot', 'type', 'date_added'),)

    class Admin:
        list_display = ('shot', 'type', 'user', 'host', 'date_added')

    def __str__(self):
        return "%s - %s - %s" % (self.type, self.user, self.shot.name)

# ------------------------------------------------------------------------------
class Output(models.Model):
    event       = models.ForeignKey(Event)
    location    = models.CharField(maxlength=4096)

    class Meta:
        unique_together = (('event', 'location'),)

    class Admin:
        pass

    class Admin:
        list_display = ('event', 'location')

    def __str__(self):
        return self.location

