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

