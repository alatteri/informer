from django_restapi.resource import Resource
from django.http import HttpResponse
from django.utils import simplejson

import os
from xml.dom.minidom import getDOMImplementation

import instinctual
conf = instinctual.getConf()
BASE = conf.get('informer', 'dir_filelist_base')

class FileCollection(Resource):
	def __init__(self, *args, **kwargs):
		if kwargs.has_key('type'):
			self.type = kwargs['type'].upper()
			del kwargs['type']
		else:
			self.type = 'JSON'
		Resource.__init__(self, *args, **kwargs)
	
	def read(self, request, path=None):
		if path:
			path = os.path.join(BASE, path)
		else:
			path = BASE
		
		dirs = []
                if path != BASE: dirs.append('..')
		files = []
		
		for f in os.listdir(path):
			if os.path.isdir(os.path.join(path, f)):
				dirs.append(f)
			else:
				files.append(f)

                dirs.sort()
                files.sort()
				
		data = {'directories':dirs, 'files':files}
		
		if self.type == 'XML':
			return HttpResponse(dumpToXML(data), 'text/xml')
		else:
			return HttpResponse(simplejson.dumps(data))
		
def dumpToXML(data):
	domimpl = getDOMImplementation()
	doc = domimpl.createDocument(None, None, None)
	d_element = doc.createElement('django')
	
	dir_element = doc.createElement('directories')
	for d in data['directories']:
		spam = doc.createElement('dir')
		spam.appendChild(doc.createTextNode(d))
		dir_element.appendChild(spam)
	d_element.appendChild(dir_element)
	
	file_element = doc.createElement('files')
	for f in data['files']:
		spam = doc.createElement('file')
		spam.appendChild(doc.createTextNode(f))
		file_element.appendChild(spam)
	d_element.appendChild(file_element)
	doc.appendChild(d_element)
	
	return doc.toxml()
