import instinctual
from instinctual.serializable import Serializable
from instinctual.informer.frame import Frame

class Clip(Serializable):
    def __init__(self, parent=None):
        if not parent:
            conf = instinctual.getConf()
            parent = conf.get('informer', 'clip_dir')
        Serializable.__init__(self, parent, 'clip.pkl')

        self.isFinished = False
        self.frames = []
        self.startFrame = None
        self.endFrame = None

        self.nodeName = None
        self.fps = None
        self.creator = None
        self.created = None
        self.isDone = False

    def newFrame(self):
        frame = Frame(self.container)
        self.frames.append(frame)

        if 1 == len(self.frames):
            self.startFrame = frame
        self.endFrame = frame

        return frame

    def save(self):
        for frame in self.frames:
            frame.save()
        Serializable.save(self)

    def delete(self):
        for frame in self.frames:
            frame.delete()
        Serializable.delete(self)

    def finish(self):
        self.isFinished = True
