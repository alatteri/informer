from django.db import models

# ------------------------------------------------------------------------------
class Project(models.Model):
    name        = models.CharField(maxlength=255, unique=True)
    description = models.CharField(maxlength=4096, null=True, blank=True)

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
    user        = models.CharField(maxlength=255)
    text        = models.CharField('text', maxlength=4096)
    comment     = models.CharField(maxlength=4096, null=True, blank=True)
    is_checked  = models.BooleanField('is checked', default=False)
    date_added  = models.DateTimeField('date added', auto_now_add=True)
    date_modified = models.DateTimeField('date modified', auto_now=True)

    class Admin:
        pass

    def __str__(self):
        if self.comment:
            return "%s: %s" % (self.text, self.comment)
        else:
            return self.text

# ------------------------------------------------------------------------------
class Element(models.Model):
    shot        = models.ForeignKey(Shot)
    text        = models.CharField(maxlength=4096)
    comment     = models.CharField(maxlength=4096, null=True, blank=True)
    is_checked  = models.BooleanField('is checked', default=False)
    date_added  = models.DateTimeField('date added', auto_now_add=True)
    date_modified = models.DateTimeField('date modified', auto_now=True)

    class Admin:
        pass

    def __str__(self):
        if self.comment:
            return "%s: %s" % (self.text, self.comment)
        else:
            return self.text

# ------------------------------------------------------------------------------
class Event(models.Model):
    shot        = models.ForeignKey(Shot)
    type        = models.CharField(maxlength=255)
    host        = models.CharField(maxlength=255)
    user        = models.CharField(maxlength=255)
    # date_added  = models.DateTimeField('date added', auto_now_add=True)
    date_added  = models.DateTimeField('date')

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
