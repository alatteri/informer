from pprint import pprint

class CustomBaseSerializer(object):
    def _get_fk_string_value(self, obj, field, type=None):
        related = getattr(obj, field.name)
        if related is not None:
            if related._meta.object_name == 'User':
                related = related.username
            elif related._meta.object_name == 'ContentType':
                related = related.name
            elif field.rel.field_name == related._meta.pk.name:
                # Related to remote object via primary key
                related = related._get_pk_val()
            else:
                # Related to remote object via other field
                related = getattr(related, field.rel.field_name)

        return related
