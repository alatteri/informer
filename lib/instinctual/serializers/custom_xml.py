from django.core.serializers.xml_serializer import Serializer as XMLSerializer
from django.core.serializers.xml_serializer import Deserializer as XMLDeserializer

from instinctual.serializers.custom_base import CustomBaseSerializer

class Serializer(XMLSerializer, CustomBaseSerializer):

    def handle_fk_field(self, obj, field):
        self._start_relational_field(field)
        related = self._get_fk_string_value(obj, field)
        if related is not None:
            self.xml.characters(str(related))
        else:
            self.xml.addQuickElement("None")
        self.xml.endElement("field")

class Deserializer(XMLDeserializer):
    pass
