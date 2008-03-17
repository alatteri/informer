from new import instancemethod
from django.db import models
from django_restapi.responder import *
from django.contrib import auth
from django.contrib.auth.models import User

class CustomBaseResponder(SerializeResponder):
    def render(self, object_list):
        """
        Serializes a queryset to the format specified in
        self.format.

        Note: This was taken from django_restapi.responder in order to support
        serializing fields of foreign keys.
        """
        # Hide unexposed fields
        hidden_fields = []

        # Find foreign fields
        foreign_fields = [ff for ff in self.expose_fields if '__' in ff]

        # Find object methods
        methods = [m for m in self.expose_fields if '.' in m]

        # Added fields
        added_fields = {}

        for obj in list(object_list):
            for field in obj._meta.fields:
                if not field.name in self.expose_fields and field.serialize:
                    field.serialize = False
                    hidden_fields.append(field)

            # incorporate foreign key fields into the object
            for ff in foreign_fields:
                # split on the delimiter, build up the relationship starting with obj
                relationship = ff.split('__')
                relationship.insert(0, obj)

                field_name = relationship[-1]
                added_fields[field_name] = 1

                # Step 1: find the value
                #   walk the relationship and evaluate the final value (using reduce)
                setattr(obj, field_name, reduce(getattr, relationship))

                # Step 2: find the field
                #   starting at the head, walk the relationship, following foreign keys
                item = relationship[0]
                for attr in relationship:
                    if not hasattr(item, '_meta'):
                        item = item.rel.to

                    for f in item._meta.fields:
                        if f.name == attr:
                            item = f

                # add the new field to _meta so will be identified for serialization
                item.serialize = True
                if field_name not in [f.name for f in obj._meta.fields]:
                    obj._meta.fields.append(item)

            # display method methods as model fields
            for m in methods:
                attrs = m.split('.')[1:]    # ignore self.
                field = attrs[-1]
                val = getattr(obj, '.'.join(attrs))

                lame = None
                if isinstance(val, int):
                    lame = models.IntegerField()
                elif isinstance(val, auth.models.User):
                    lame = models.ForeignKey(User)
                else:
                    # other case: isinstance(val, str):
                    val = str(val)
                    lame = models.CharField()

                setattr(lame, field, val)
                lame.name = field
                lame.attname = field
                lame.serialize = True
                obj._meta.fields.append(lame)
                added_fields[field] = 1

        response = serializers.serialize(self.format, object_list)

        # Show unexposed fields again
        for field in hidden_fields:
            field.serialize = True

        # Remove added attrs and fields
        if len(added_fields.keys()):
            for obj in list(object_list):
                # for field in added_fields.keys():
                #     print "trying to remove", field
                #     delattr(obj, field)
                for i in range(len(obj._meta.fields)-1, 0, -1):
                    if obj._meta.fields[i].name in added_fields:
                        obj._meta.fields.pop(i)

        return str(response)

class PythonResponder(CustomBaseResponder):
    """
    Python data format class.
    """
    def __init__(self, paginate_by=None, allow_empty=False):
        CustomBaseResponder.__init__(self, 'python', 'text/plain',
                    paginate_by=paginate_by, allow_empty=allow_empty)

class CustomXMLResponder(CustomBaseResponder, XMLResponder):
    def __init__(self, paginate_by=None, allow_empty=False):
        CustomBaseResponder.__init__(self, 'custom_xml', 'application/xml',
                    paginate_by=paginate_by, allow_empty=allow_empty)

class CustomJSONResponder(CustomBaseResponder, JSONResponder):
    def __init__(self, paginate_by=None, allow_empty=False):
        CustomBaseResponder.__init__(self, 'custom_json', 'application/json',
                    paginate_by=paginate_by, allow_empty=allow_empty)
