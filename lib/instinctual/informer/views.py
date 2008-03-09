# Create your views here.
from django.shortcuts import render_to_response, get_object_or_404
from django.contrib.auth.decorators import login_required
from django.views.generic.list_detail import object_list
from django.contrib.auth.models import User

from instinctual import informer

from instinctual.informer.responder import CustomJSONResponder
from instinctual.informer.models import Project, Shot, Note, Render, Element, Log

# ------------------------------------------------------------------------------
def project_list(request):
    responder = CustomJSONResponder()
    responder.expose_fields = Project.Rest.expose_fields
    #    data = responder.render(Project.objects.all()))
    c = {
        'user':      request.user,
        'object_list':  Project.objects.all(),
    }
    return render_to_response('informer/project_list.html', c)
project_list = login_required(project_list)

# ------------------------------------------------------------------------------
def project_detail(request, project_name):
    p = get_object_or_404(Project, name=project_name)
    c = {
        'user':     request.user,
        'project':  p,
    }
    return render_to_response('informer/project_detail.html', c)
project_detail = login_required(project_detail)

# ------------------------------------------------------------------------------
def shot_logs(request, project_name, shot_name):
    p = get_object_or_404(Project, name=project_name)
    shot = get_object_or_404(Shot, project=p, name=shot_name)
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
    shot = get_object_or_404(Shot, project=p, name=shot_name)
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
    shot = get_object_or_404(Shot, project=p, name=shot_name)
    responder = CustomJSONResponder()
    responder.expose_fields = Element.Rest.expose_fields
    c = {
        'user':     request.user,
        'project':  p,
        'shot':     shot,
        'data':     responder.render(shot.element_set.all()),
    }
    return render_to_response('informer/shot_detail_elements.html', c)
shot_elements = login_required(shot_elements)

# ------------------------------------------------------------------------------
def shot_renders(request, project_name, shot_name):
    p = get_object_or_404(Project, name=project_name)
    shot = get_object_or_404(Shot, project=p, name=shot_name)
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
