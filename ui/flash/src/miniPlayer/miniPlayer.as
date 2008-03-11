package {
    import com.deviant.layout.AlignToParent;
    
    import flash.display.LoaderInfo;
    import flash.display.Sprite;
    import flash.events.AsyncErrorEvent;
    import flash.events.MouseEvent;
    import flash.events.NetStatusEvent;
    import flash.events.SecurityErrorEvent;
    import flash.media.Video;
    import flash.net.NetConnection;
    import flash.net.NetStream;
    import flash.ui.Mouse;

    [SWF(frameRate="35", backgroundColor="#000000", width="160", height="90")]
    public class miniPlayer extends Sprite
    {
        //[Embed(source="images/background.jpg")]
        //public var backgroundImage:Class;
        
        protected var _button:miniPlayerButton = new miniPlayerButton();
        protected var _video:Video = new Video(160, 90);
        //protected var background:Sprite = new Sprite();
        protected var connection:NetConnection;
        protected var videoURL:String;
        protected var stream:NetStream;
        protected var paused:Boolean = true;
        protected var stopped:Boolean = false;
        protected var _mask:Sprite = new Sprite();
        
        public function miniPlayer()
        {
            var queryString:Object = LoaderInfo(root.loaderInfo).parameters;
            
            videoURL = queryString["video"];
            
            //background.addChild(new backgroundImage());
            
            connection = new NetConnection();
            connection.addEventListener(NetStatusEvent.NET_STATUS, netStatusHandler);
            connection.addEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler);
            connection.connect(null);
            
            _mask.addEventListener(MouseEvent.CLICK, onButtonClick);
            _mask.addEventListener(MouseEvent.MOUSE_OVER, onMouseOver);
            _mask.addEventListener(MouseEvent.MOUSE_OUT, onMouseOut);
            
            //addChild(background);
            
            addChild(_video);
            
            with(_mask.graphics)
            {
                beginFill(0x000000, 0);
                drawRect(0,0,160,90);
                endFill();
            }
            
            //_mask.x = _video.x = 14;
            //_mask.y = _video.y = 13;
            
            addChild(new AlignToParent(_button,this));
            addChild(_mask);
            _button.visible = false;
        }
        
        protected function onButtonClick(e:MouseEvent):void
        {
            if(stopped)
                stream.seek(0);
            
            stream.togglePause();
            
            // simulate the click
            _button.dispatchEvent(e);
        }
        
        protected function onMouseOver(e:MouseEvent):void
        {
            _button.visible = true;
        }
        
        protected function onMouseOut(e:MouseEvent):void
        {
            _button.visible = false;
        }

        private function netStatusHandler(event:NetStatusEvent):void 
        {
            switch (event.info.code)
            {
                case "NetConnection.Connect.Success":
                    connectStream();
                    break;
                case "NetStream.Play.StreamNotFound":
                    trace("Unable to locate video: " + videoURL);
                    break;
                case "NetStream.Play.Stop":
                    stopped = true;
                    stream.pause();
                    _button.toPlay();
            }
        }

        private function connectStream():void 
        {
            stream = new NetStream(connection);
            stream.addEventListener(NetStatusEvent.NET_STATUS, netStatusHandler);
            stream.addEventListener(AsyncErrorEvent.ASYNC_ERROR, asyncErrorHandler);
            stream.client = this;
            
            _video.attachNetStream(stream);
            stream.play(videoURL);
            stream.seek(0.1);
            stream.pause();
            
            _video.smoothing = true;
        }
        
        // needs to be public for the callback for the stream.client
        // freaking awful practise by Adobe this...
        public function onMetaData(info:Object):void 
        {
            setNewDimentions(info.width, info.height);
            //width = info.width;
            //height =  info.height;
        }
        
        public function setNewDimentions(w:Number, h:Number):void
        {
            var R:Number;
            if(w > 160)
            {
                R = 160 / w;
                w *= R;
                h *= R;
            }
            if(h > 90)
            {
                R = 90 / h;
                w *= R;
                h *= R;
            }
            
            _video.width = w;
            _video.height = h;
            
            _video.x = (160 / 2 ) - (w / 2);
            _video.y = (90 / 2) - (h / 2);
        }

        private function securityErrorHandler(event:SecurityErrorEvent):void 
        {
            trace("securityErrorHandler: " + event);
        }
        
        private function asyncErrorHandler(event:AsyncErrorEvent):void 
        {
            // ignore AsyncErrorEvent events.
        }
    }
}
