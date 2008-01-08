# Create your views here.
from django.shortcuts import render_to_response, get_object_or_404
from django.contrib.auth.decorators import login_required
from django.views.generic.list_detail import object_list

from instinctual.informer.models import Project, Shot, Note, Element, Event

@login_required
def project_detail(request, project_name):
    p = get_object_or_404(Project, name=project_name)
    return render_to_response('informer/project_detail.html', {'project':p})

@login_required
def shot_detail(request, project_name, shot_name):
    p = get_object_or_404(Project, name=project_name)
    shot = get_object_or_404(Shot, project=p, name=shot_name)
    return render_to_response('informer/shot_detail.html', {'project':p,'shot':shot})

@login_required
def limited_object_list(*args, **kwargs):
    return object_list(*args, **kwargs)

