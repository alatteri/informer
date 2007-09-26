import time
from datetime import datetime
from xml.dom import pulldom

# This code is mirrored from django/core/serializers/xml_serializer.py
# this was done to remove a dependency on the djanog code on the client


class DeserializedObject(object):
    pass

class Deserializer(object):
    def __init__(self, stream_or_string):
        self.event_stream = pulldom.parseString(stream_or_string)

    def next(self):
        for event, node in self.event_stream:
            if event == "START_ELEMENT" and node.nodeName == "object":
                self.event_stream.expandNode(node)
                return self._handle_object(node)
        raise StopIteration

    def _handle_object(self, node):
        object = DeserializedObject()

        pk = node.getAttribute("pk")
        if not pk:
            raise ValueError("<object> node is missing the 'pk' attribute")

        object.id = pk

        # Deseralize each field.
        for field_node in node.getElementsByTagName("field"):
            field_name = field_node.getAttribute("name")
            if not field_name:
                raise ValueError("<field> node is missing the 'name' attribute")

            # for now, ignore the relationship key fields
            field_rel = field_node.getAttribute("rel")
            if field_rel:
                # print "skipping %s, it is rel=%s" % (field_name, field_rel)
                continue

            field_type = field_node.getAttribute("type")
            if not field_type:
                raise ValueError("<field> node is missing the 'type' attribute")

            field_val = getInnerText(field_node)
            if 'CharField' == field_type:
                pass
            elif 'BooleanField' == field_type:
                if 'True' == field_val or '1' == field_val:
                    field_val = True
                elif 'False' == field_val or '0' == field_val:
                    field_val = False
                else:
                    err = "BooleanField (%s) was not 'True' or 'False': %s"
                    raise ValueError(err % (field_name, field_val))
            elif 'DateTimeField' == field_type:
                fmt = '%Y-%m-%d %H:%M:%S'
                field_val = datetime(*time.strptime(field_val, fmt)[:6])
            else:
                # TODO: add support as new fields come up
                raise ValueError("Unspecified field_type: %s" % (field_type))

            object.__setattr__(field_name, field_val)

        # To maintain compatibility with Django's own Deserializer
        # return the object as '.object' off of some base object
        base = DeserializedObject()
        base.object = object

        return base

def getInnerText(node):
    """
    Get all the inner text of a DOM node (recursively).
    """
    # inspired by http://mail.python.org/pipermail/xml-sig/2005-March/011022.html
    inner_text = []
    for child in node.childNodes:
        if child.nodeType == child.TEXT_NODE or child.nodeType == child.CDATA_SECTION_NODE:
            inner_text.append(child.data)
        elif child.nodeType == child.ELEMENT_NODE:
            inner_text.extend(getInnerText(child))
        else:
           pass
    return "".join(inner_text)

