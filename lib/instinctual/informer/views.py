# Create your views here.
from django.shortcuts import render_to_response, get_object_or_404

from instinctual.informer.models import Project, Shot, Note, Element, Event

def project_detail(request, project_name):
    p = get_object_or_404(Project, name=project_name)
    return render_to_response('informer/project_detail.html', {'project':p})

def shot_detail(request, project_name, shot_name):
    p = get_object_or_404(Project, name=project_name)
    shot = get_object_or_404(Shot, project=p, name=shot_name)
    return render_to_response('informer/shot_detail.html', {'project':p,'shot':shot})

