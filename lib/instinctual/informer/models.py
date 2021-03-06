import os
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
    title       = models.CharField(maxlength=255, null=True, blank=True)

    client      = models.CharField(maxlength=255, null=True, blank=True)
    producer    = models.CharField(maxlength=255, null=True, blank=True)
    creatives   = models.CharField(maxlength=255, null=True, blank=True)
    due_date    = models.DateTimeField('due date', null=True, blank=True)

    prod_company  = models.CharField('production co', maxlength=255, null=True, blank=True)
    director      = models.CharField(maxlength=255, null=True, blank=True)
    prod_producer = models.CharField('prod producer', maxlength=255, null=True, blank=True)
    exec_producer = models.CharField('exec producer', maxlength=255, null=True, blank=True)

    editorial          = models.CharField(maxlength=255, null=True, blank=True)
    editor             = models.CharField(maxlength=255, null=True, blank=True)
    editorial_assist   = models.CharField('editorial assist', maxlength=255, null=True, blank=True)
    editorial_producer = models.CharField('editorial prod', maxlength=255, null=True, blank=True)

    flame        = models.CharField(maxlength=255, null=True, blank=True)
    flame_assist = models.CharField('flame assist', maxlength=255, null=True, blank=True)
    cg           = models.CharField(maxlength=255, null=True, blank=True)
    telecine     = models.CharField(maxlength=255, null=True, blank=True)

    shotlist_no  = models.CharField('shotlist no', maxlength=255, null=True, blank=True)
    duration     = models.CharField(maxlength=255, null=True, blank=True)
    audio_relaid = models.CharField('audio relaid', maxlength=255, null=True, blank=True)

    branding = InformerBrandingField(upload_to='branding', default='branding/default.gif')

    class Admin:
        show_all_rows = False
        grant_change_row_level_perm = True
        grant_delete_row_level_perm = True
        list_display = ('name', 'description', 'status', 'title',
                        'client', 'producer', 'creatives', 'due_date',
                        'prod_company', 'director', 'prod_producer', 'exec_producer',
                        'editorial', 'editor', 'editorial_assist', 'editorial_producer',
                        'flame', 'flame_assist', 'cg', 'telecine',
                        'shotlist_no', 'duration', 'audio_relaid',
                        'branding')

    class Rest:
        expose_fields = ['name', 'description', 'status', 'client', 'due_date', 'branding',
                         'self.shot_count', 'self.url', 'self.print_url']

    class Meta:
        row_level_permissions = True

    def __str__(self):
        if self.description:
            return "%s: %s" % (self.name, self.description)
        else:
            return self.name

    def get_absolute_url(self):
        return informer.getProjectShotsUrl(self.name, format='html')

    def get_absolute_shots_url(self):
        return informer.getProjectShotsUrl(self.name, format='html')

    def get_absolute_print_url(self):
        return informer.getProjectPrintUrl(self.name, format='html')

    def getNameFromRequest(cls, request):
        return request.path.split("/")[5]
    getNameFromRequest = classmethod(getNameFromRequest)

    def _shot_count(self):
        return len(Shot.objects.filter(project=self))

    shot_count = property(_shot_count)
    url = property(get_absolute_url)
    print_url = property(get_absolute_print_url)

# ------------------------------------------------------------------------------
class Shot(models.Model):
    project     = models.ForeignKey(Project)
    name        = models.CharField('shot', maxlength=255)
    description = models.CharField(maxlength=4096, null=True, blank=True)
    status      = models.CharField(maxlength=64, null=True, blank=True)
    handles     = models.CharField(maxlength=32, null=True, blank=True)
    frames      = models.IntegerField(default=0)
    sequence    = models.CharField(maxlength=4096, null=True, blank=True)

    def delete(self, *args, **kwargs):
        h = getHandler()
        h.enabled = False
        models.Model.delete(self, *args, **kwargs)

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
    def _get_render_obj(self):
        if hasattr(self, '_render_obj'):
            return self._render_obj

        r = Render.objects.filter(shot=self).order_by('-created_on')
        self._render_obj = len(r) and r[0] or None
        return self._render_obj

    def get_render_artist(self):
        r = self._get_render_obj()
        return r and r.created_by or None

    def get_render_time(self):
        r = self._get_render_obj()
        return r and r.created_on or None

    def get_render_machine(self):
        r = self._get_render_obj()
        return r and r.host or None

    def get_render_setup(self):
        r = self._get_render_obj()
        return r and r.setup or None

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

    def get_notes(self):
        return Note.objects.filter(shot=self).order_by('created_on')

    # --------------------------------------------------------------------------
    url = property(get_absolute_url)
    notes = property(get_notes)
    render_movie_hi_url = property(get_render_movie_hi_url)
    render_movie_lo_url = property(get_render_movie_lo_url)
    last_modified = property(get_render_time)
    last_modified_by = property(get_render_artist)

    # --------------------------------------------------------------------------
    class Meta:
        row_level_permissions = True
        unique_together = (('project', 'name'),)

    class Admin:
        show_all_rows = False
        grant_change_row_level_perm = True
        grant_delete_row_level_perm = True
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

            if 'true' == new['is_checked']:
                comment = 'as complete'
            else:
                comment = 'as pending'

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

    class Meta:
        unique_together = (('shot', 'type', 'created_on'),)

    class Admin:
        list_display = ('id', 'shot', 'type', 'host', 'setup',
                        'created_by', 'created_on')

    def __str__(self):
        return "Event %s (%s, %s)" % (self.id, self.type, self.created_by)

# ------------------------------------------------------------------------------
class Render(InformerMixIn, models.Model):
    shot  = models.ForeignKey(Shot)
    host  = models.CharField(maxlength=255)
    job   = models.CharField(maxlength=255, unique=True)
    setup = models.CharField(maxlength=4096)

    created_by  = models.ForeignKey(User)
    created_on  = models.DateTimeField('date created', auto_now_add=True)
    modified_on = models.DateTimeField('date modified', auto_now=True)
    is_pending  = models.BooleanField('pending', default=True)

    spark = models.CharField(maxlength=255)
    movie_hi = models.FileField(upload_to='movies/%Y/%m/%d', blank=True, default='pending.mov')
    movie_lo = models.FileField(upload_to='movies/%Y/%m/%d', blank=True, default='pending.mov')

    # storing fps as text to avoid floating point precision issues
    rate = models.CharField(maxlength=32, default='')

    class Admin:
        list_display = ('id', 'is_pending', 'shot', 'movie_hi', 'job',
                        'created_on')
    class Rest:
        expose_fields = ['created_on', 'modified_on', 'movie_hi', 'movie_lo',
                         'rate', 'job', 'created_by', 'host', 'setup']
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
                        'created_on', 'created_by')

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

    class Rest:
        expose_fields = ['who', 'when', 'action', 'type', 'object_id',
                         'msg_prefix', 'object_repr', 'msg_suffix']

# ------------------------------------------------------------------------------
class ElementCategory(models.Model):
    type = models.CharField(maxlength=255, unique=True)

    class Admin:
        list_display = ('type',)

    class Meta:
        verbose_name_plural = "Element Categories"

    def __str__(self):
        return self.type

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
