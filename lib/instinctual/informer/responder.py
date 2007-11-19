from django_restapi.responder import *

class PythonResponder(SerializeResponder):
    """
    Python data format class.
    """
    def __init__(self, paginate_by=None, allow_empty=False):
        SerializeResponder.__init__(self, 'python', 'text/plain',
                    paginate_by=paginate_by, allow_empty=allow_empty)

    def render(*args, **kwargs):
        return str(SerializeResponder.render(*args, **kwargs))

class CustomXMLResponder(XMLResponder):
    def __init__(self, paginate_by=None, allow_empty=False):
        SerializeResponder.__init__(self, 'custom_xml', 'application/xml',
                    paginate_by=paginate_by, allow_empty=allow_empty)

    def render(*args, **kwargs):
        return str(XMLResponder.render(*args, **kwargs))

class CustomJSONResponder(JSONResponder):
    def __init__(self, paginate_by=None, allow_empty=False):
        SerializeResponder.__init__(self, 'custom_json', 'application/json',
                    paginate_by=paginate_by, allow_empty=allow_empty)

    def render(*args, **kwargs):
        return str(JSONResponder.render(*args, **kwargs))


