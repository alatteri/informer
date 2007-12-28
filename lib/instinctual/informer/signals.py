CREATED = 'created'
UPDATED = 'updated'
DELETED = 'deleted'

NEW_OBJ  = '__this_is_a_new_object__'
PREV_OBJ = '__the_previous_object__'

class IgnoreSignalException(Exception):
    pass

class Handler(object):
    def handle_pre_save(self, signal, sender, instance, *args, **kwargs):
        pk = instance._get_pk_val()

        if hasattr(instance, 'Logger') and hasattr(instance.Logger, CREATED):
            if not pk:
                setattr(instance, NEW_OBJ, True)
                return

        if hasattr(instance, 'Logger') and hasattr(instance.Logger, UPDATED):
            if pk:
                previous = sender.objects.get(pk=pk)
                setattr(instance, PREV_OBJ, previous)

    def handle_post_save(self, signal, sender, instance, *args, **kwargs):
        if hasattr(instance, 'Logger') and hasattr(instance.Logger, CREATED):
            if hasattr(instance, NEW_OBJ):
                delattr(instance, NEW_OBJ)
                self.log(instance, CREATED)
                return

        if hasattr(instance, 'Logger') and hasattr(instance.Logger, UPDATED):
            if hasattr(instance, PREV_OBJ):
                previous = getattr(instance, PREV_OBJ)
                delattr(instance, PREV_OBJ)

                old = {}
                new = {}

                for field in instance._meta.fields:
                    f_old = getattr(previous, field.name)
                    f_new = getattr(instance, field.name)
                    if f_old != f_new:
                        old[field.name] = f_old
                        new[field.name] = f_new

                if len(old):
                    self.log(instance, UPDATED, old, new)

                return

    def handle_pre_delete(self, signal, sender, instance, *args, **kwargs):
        if hasattr(instance, 'Logger') and hasattr(instance.Logger, DELETED):
            self.log(instance, DELETED)

    def log(self, instance, action, old={}, new={}):
        print "*" * 80
        print "now going to register", action
        print "*" * 80

        from instinctual.informer.models import Log
        l = Log()

        # TODO: who <--- Not sure how to get this for all cases.
        l.action = action
        l.object_id = instance._get_pk_val()
        l.type = instance.__class__.__name__

        if 'Shot' == l.type:
            l.shot = instance
        else:
            l.shot = instance.shot

        func = getattr(instance.Logger, action)

        try:
            (l.msg_prefix, l.object_repr, l.msg_suffix) = func(instance, old, new)
            l.save()
        except IgnoreSignalException, e:
            pass
