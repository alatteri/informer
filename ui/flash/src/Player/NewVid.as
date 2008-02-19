package Player
{
    import flash.display.Sprite;
    import flash.events.Event;
    import flash.events.NetStatusEvent;
    import flash.events.SecurityErrorEvent;
    import flash.media.Video;
    import flash.net.NetConnection;
    import flash.net.NetStream;
    
    public class NewVid extends Sprite
    { 
        protected var _duration:Number;
        protected var _maxWidth:Number = 544;
        protected var _maxHeight:Number = 304;
        protected var _width:Number = 544;
        protected var _height:Number = 304;
        protected var _framerate:Number;
        protected var _ready:Boolean = false;
        protected var connection:NetConnection = new NetConnection();
        protected var video:Video = new Video(_width, _height);
        protected var stream:NetStream;
        protected var ui:PlayerUI = PlayerUI.getInstance();
        protected var paused:Boolean = true;
        
        private static var instance:NewVid;
        
        public function NewVid()
        {
            super();
            connection.addEventListener(NetStatusEvent.NET_STATUS, netStatusHandler);
            connection.addEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler);
            connection.connect(null);
            
            addChild(video);
            
            video.smoothing = true;
            
            addEventListener(Event.ENTER_FRAME, oEF);
        }
        
        public static function getInstance():NewVid
        {
            if(instance == null)
                instance = new NewVid();
            return instance;
        }
        
        public function oEF(e:Event):void
        {
            if(Playing)
                ui.scrubber.Progress = Position;
                
            ui.scrubber.LoadProgress = stream.bytesLoaded / stream.bytesTotal;
            ui.scrubber.CurrentFrame = CurrentFrame;
        }
        
        protected var _currFrame:Number = 0;
        
        // frames in flash start at 0 in the video they start at 1, so add one.
        public function get CurrentFrame():Number
        {
            if(Playing)
                _currFrame = Math.round(stream.time * Framerate) + 1;
                
            return _currFrame;
        }
        
        public function set CurrentFrame(n:Number):void
        {
            _currFrame = (n < 1)?1:n;
            stream.seek(_currFrame / Framerate);
            
            ui.scrubber.Progress = Position;
        }
        
        public function get Position():Number
        {
            return stream.time / Duration;
        }
        
        public function set Position(n:Number):void
        {
            stream.seek(n * Duration);
            var p:Boolean = paused;
            paused = false;
            ui.scrubber.CurrentFrame = CurrentFrame;
            paused = p;
        }
        
        public function get Paused():Boolean
        {
            return paused;
        }
        
        public function get Playing():Boolean
        {
            return !paused;
        }
        
        public function pause():void
        {
            paused = true;
            stream.pause();
            ui.setPlayStatus(false);
        }
        
        public function play():void
        {
            paused = false;
            stream.resume();
            ui.setPlayStatus(true);
        }
        
        public function seek(n:Number):void
        {
            stream.seek(n);
        }

        private function netStatusHandler(event:NetStatusEvent):void 
        {
            trace(event.info.code);
            switch (event.info.code) 
            {
                case "NetConnection.Connect.Success":
                    _ready = true;
                    connectStream(PlayerForAlan.Video);
                    break;
                case "NetStream.Play.StreamNotFound":
                    trace("Stream not found: " + event.info);
                    break;
                case "NetStream.Play.Stop":
                    pause();
                    ui.stop();
                    break;
            }
        }
        
        private function securityErrorHandler(event:SecurityErrorEvent):void 
        {
            trace("securityErrorHandler: " + event);
        }
        
        private function connectStream(videoURL:String):void 
        {
            if(_ready)
            {
                stream = new NetStream(connection);
                stream.addEventListener(NetStatusEvent.NET_STATUS, netStatusHandler);
                stream.client = this;
                
                video.attachNetStream(stream);
                if(PlayerForAlan.AutoPlay)
                {
                    stream.play(videoURL);
                    paused = false;
                }
                
                ui.setPlayStatus(true);
                dispatchEvent(new Event(Event.RESIZE));
            }
            else
                throw Error("Tried to connect without the Net Connection being ready!");
        }
        
        public function get Duration():Number
        {
            return _duration;
        }
        
        public function set Duration(n:Number):void
        {
            _duration = n;
            
            ui.scrubber.MaxFrame = Math.round(n*Framerate);
        }

        public override function set width(n:Number):void
        {
            _width = n;
            
            video.width = width;
            dispatchEvent(new Event(Event.RESIZE));
        }
        
        public override function set height(n:Number):void
        {
            _height = n
            
            video.height = height;
            dispatchEvent(new Event(Event.RESIZE));
        }
        
        public function get Framerate():Number
        {
            return _framerate;
        }
        
        public function set Framerate(n:Number):void
        {
            _framerate = Math.round(n);
        }
        
        public override function get width():Number
        {
            return _width;
        }
        
        public override function get height():Number
        {
            return _height;
        }
        
        public function setNewDimentions(w:Number, h:Number):void
        {
            var R:Number;
            if(w > _maxWidth)
            {
                R = _maxWidth / w;
                w *= R;
                h *= R;
            }
            if(h > _maxHeight)
            {
                R = _maxHeight / h;
                w *= R;
                h *= R;
            }
            
            width = w;
            height = h;
        }
        
        public function alignVideo():void
        {
            video.x = _maxWidth / 2 - width / 2;
            video.y = _maxHeight / 2 - height / 2;
        }
        
        public function onMetaData(info:Object):void 
        {
            Framerate = (info.framerate == null)?info.videoframerate:info.framerate;
            Duration = info.duration;
            setNewDimentions(info.width, info.height);
            alignVideo();
            //width = info.width;
            //height =  info.height;
        }
        
        public function onCuePoint(info:Object):void 
        {
            trace("cuepoint: time=" + info.time + " name=" + info.name + " type=" + info.type);
        }
        
    }
}