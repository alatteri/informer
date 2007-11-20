from django.core.serializers.json import Serializer as JSONSerializer
from django.core.serializers.json import Deserializer as JSONDeserializer

class Serializer(JSONSerializer):
    def handle_fk_field(self, obj, field):
        related = getattr(obj, field.name)
        if related is not None:
            if related._meta.object_name == 'User':
                related = {'username':related.username, 'pk':related.id}
            elif field.rel.field_name == related._meta.pk.name:
                # Related to remote object via primary key
                related = related._get_pk_val()
            else:
                # Related to remote object via other field
                related = getattr(related, field.rel.field_name)
        self._current[field.name] = related

def Deserializer(object_list, **options):
    JSONDeserializer(object_list)

