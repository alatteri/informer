from django.core.serializers.json import Serializer as JSONSerializer
from django.core.serializers.json import Deserializer as JSONDeserializer

from instinctual.serializers.custom_base import CustomBaseSerializer

class Serializer(JSONSerializer, CustomBaseSerializer):

    def handle_fk_field(self, obj, field):
        value = self._get_fk_string_value(obj, field)

        related = getattr(obj, field.name)
        if related is not None and related._meta.object_name == 'User':
            # special json hack -- only Bryan knows why this is needed
            value = {'username': value, 'pk': related.id}

        self._current[field.name] = value

def Deserializer(object_list, **options):
    JSONDeserializer(object_list)
