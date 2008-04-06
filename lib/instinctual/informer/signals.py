import instinctual
LOG = instinctual.getLogger(__name__)

CREATED = 'created'
UPDATED = 'updated'
DELETED = 'deleted'

NEW_OBJ  = '__this_is_a_new_object__'
PREV_OBJ = '__the_previous_object__'

class IgnoreSignalException(Exception):
    pass

class Handler(object):
    user = None
    enabled = True

    def setUser(self, user):
        # print "((((((((((( setting user to %s ))))))))))))" % (user)
        self.user = user

    def handle_class_prepared(self, signal, sender, instance, *args, **kwargs):
        # print "-" * 80
        # print "class prepared: sender (%s) instance (%s)" % (sender, instance)
        pass

    def handle_pre_init(self, signal, sender, *args, **kwargs):
        # print "-" * 80
        # print "pre init: sender (%s) args (%s) kwargs (%s)" % (sender, args, kwargs)
        pass

    def handle_post_init(self, signal, sender, instance, *args, **kwargs):
        # print "-" * 80
        # print "post init: sender (%s) instance (%s)" % (sender, instance)

        if 'Session' == instance.__class__.__name__:
            # init the user on a new session
            self.setUser(None)
            self.enabled = True
        elif 'User' == instance.__class__.__name__ and self.user is None:
            # This is a hack to capture the currently logged in user
            # without a request object. This is fragile.
            self.setUser(instance)

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
        LOG.debug("DELETE CALLED FOR: %s" % instance.__class__.__name__)
        if hasattr(instance, 'Logger') and hasattr(instance.Logger, DELETED):
            self.log(instance, DELETED)

    def log(self, instance, action, old={}, new={}):
        LOG.debug("*" * 80)
        LOG.debug("now going to register %s for %s" % (action, type(instance)))
        LOG.debug("*" * 80)

        if self.enabled:
            from instinctual.informer.models import Log
            l = Log()

            if self.user:
                l.who = self.user
            else:
                l.who = 'UNKNOWN'

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
        else:
            LOG.debug("(((((( not enabled ))))))) not logging...")
