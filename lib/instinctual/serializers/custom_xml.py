from django.core.serializers.xml_serializer import Serializer as XMLSerializer
from django.core.serializers.xml_serializer import Deserializer as XMLDeserializer

class Serializer(XMLSerializer):

    def handle_fk_field(self, obj, field):
        self._start_relational_field(field)
        related = getattr(obj, field.name)
        if related is not None:
            if related._meta.object_name == 'User':
            	related = related.username
            elif field.rel.field_name == related._meta.pk.name:
                # Related to remote object via primary key
                related = related._get_pk_val()
            else:
                # Related to remote object via other field
                related = getattr(related, field.rel.field_name)
            self.xml.characters(str(related))
        else:
            self.xml.addQuickElement("None")
        self.xml.endElement("field")

class Deserializer(XMLDeserializer):
    pass

