# Create your views here.
from django.shortcuts import render_to_response, get_object_or_404
from django.contrib.auth.decorators import login_required
from django.views.generic.list_detail import object_list
from django.contrib.auth.models import User
from django.http import HttpResponseForbidden

from instinctual import informer

from instinctual.informer.responder import CustomJSONResponder
from instinctual.informer.models import Project, Shot, Note, Render, Element, ElementCategory, Log

PROJECT_DENIED = 'You do not have permission to view this project'
SHOT_DENIED = 'You do not have permission to view this shot'

# ------------------------------------------------------------------------------
def project_list(request):
    responder = CustomJSONResponder()
    responder.expose_fields = Project.Rest.expose_fields

    projects = []
    for p in Project.objects.all():
        if request.user.has_perm('informer.change_project', object=p):
            projects.append(p)

    c = {
        'user':         request.user,
        'data':         responder.render(projects),
    }
    return render_to_response('informer/project_list.html', c)
project_list = login_required(project_list)

# ------------------------------------------------------------------------------
def project_detail(request, project_name):
    p = get_object_or_404(Project, name=project_name)
    if not request.user.has_perm('informer.change_project', object=p):
        return HttpResponseForbidden(PROJECT_DENIED)

    responder = CustomJSONResponder()
    responder.expose_fields = Shot.Rest.expose_fields

    shots = []
    for s in Shot.objects.filter(project=p):
        if request.user.has_perm('informer.change_shot', object=s):
            shots.append(s)

    c = {
        'user':     request.user,
        'project':  p,
        'data':     responder.render(shots),
    }
    return render_to_response('informer/project_detail.html', c)
project_detail = login_required(project_detail)

# ------------------------------------------------------------------------------
def shot_logs(request, project_name, shot_name):
    p = get_object_or_404(Project, name=project_name)
    if not request.user.has_perm('informer.change_project', object=p):
        return HttpResponseForbidden(PROJECT_DENIED)

    shot = get_object_or_404(Shot, project=p, name=shot_name)
    if not request.user.has_perm('informer.change_shot', object=shot):
        return HttpResponseForbidden(SHOT_DENIED)

    responder = CustomJSONResponder()
    responder.expose_fields = Log.Rest.expose_fields
    c = {
        'user':     request.user,
        'project':  p,
        'shot':     shot,
        'data':     responder.render(shot.log_set.all()),
    }
    return render_to_response('informer/shot_detail_logs.html', c)
shot_detail = login_required(shot_logs)

# ------------------------------------------------------------------------------
def shot_notes(request, project_name, shot_name):
    p = get_object_or_404(Project, name=project_name)
    if not request.user.has_perm('informer.change_project', object=p):
        return HttpResponseForbidden(PROJECT_DENIED)

    shot = get_object_or_404(Shot, project=p, name=shot_name)
    if not request.user.has_perm('informer.change_shot', object=shot):
        return HttpResponseForbidden(SHOT_DENIED)

    users = User.objects.all()
    responder = CustomJSONResponder()
    responder.expose_fields = Note.Rest.expose_fields
    c = {
        'user':     request.user,
        'project':  p,
        'shot':     shot,
        'data':     responder.render(shot.note_set.all()),
        'users':    users,
    }
    return render_to_response('informer/shot_detail_notes.html', c)
shot_notes = login_required(shot_notes)

# ------------------------------------------------------------------------------
def shot_elements(request, project_name, shot_name):
    p = get_object_or_404(Project, name=project_name)
    if not request.user.has_perm('informer.change_project', object=p):
        return HttpResponseForbidden(PROJECT_DENIED)

    shot = get_object_or_404(Shot, project=p, name=shot_name)
    if not request.user.has_perm('informer.change_shot', object=shot):
        return HttpResponseForbidden(SHOT_DENIED)

    responder = CustomJSONResponder()
    responder.expose_fields = Element.Rest.expose_fields
    c = {
        'user':       request.user,
        'project':    p,
        'shot':       shot,
        'categories': ElementCategory.objects.all(),
        'data':       responder.render(shot.element_set.all()),
    }
    return render_to_response('informer/shot_detail_elements.html', c)
shot_elements = login_required(shot_elements)

# ------------------------------------------------------------------------------
def shot_renders(request, project_name, shot_name):
    p = get_object_or_404(Project, name=project_name)
    if not request.user.has_perm('informer.change_project', object=p):
        return HttpResponseForbidden(PROJECT_DENIED)

    shot = get_object_or_404(Shot, project=p, name=shot_name)
    if not request.user.has_perm('informer.change_shot', object=shot):
        return HttpResponseForbidden(SHOT_DENIED)

    responder = CustomJSONResponder()
    responder.expose_fields = Render.Rest.expose_fields
    c = {
        'user':     request.user,
        'project':  p,
        'shot':     shot,
        'data':     responder.render(shot.render_set.all()),
    }
    return render_to_response('informer/shot_detail_renders.html', c)
shot_renders = login_required(shot_renders)

def limited_object_list(*args, **kwargs):
    return object_list(*args, **kwargs)
limited_object_list = login_required(limited_object_list)
