package Player
{
    import flash.display.Sprite;
    import Player.UI.Scrubber;
    import Player.UI.DownloadButton;
    import Player.UI.ExpandButton;
    import Player.UI.PlaySet;
    import flash.geom.Matrix;
    import flash.display.GradientType;
    import flash.display.SpreadMethod;
    import flash.filters.BevelFilter;
    import flash.filters.DropShadowFilter;
    import flash.events.Event;
    import flash.utils.Timer;
    import flash.events.TimerEvent;
    import flash.events.MouseEvent;
    import flash.net.navigateToURL;
    import flash.net.URLRequest;
    import flash.net.FileReference;
    import flash.display.Bitmap;
    import flash.net.URLRequestMethod;

    public class PlayerUI extends Sprite
    {
        
        public var scrubber:Scrubber = new Scrubber();
        
        [Embed(source="/images/background.png")]
        public var backgroundImage:Class;
        
        protected var download:DownloadButton = new DownloadButton();
        protected var expand:ExpandButton = new ExpandButton();
        protected var playSet:PlaySet = new PlaySet();
        protected var video:NewVid;
        
        protected var _playClicked:Boolean = false;
        protected var _stopped:Boolean = false;
        
        private static var _instance:PlayerUI;
        
        public function PlayerUI()
        {
            super();
            
            var background:Bitmap = new backgroundImage();
            
            scrubber.y = 5;
            scrubber.x = 7;
            
            download.y = 40;
            download.x = 15
            
            playSet.x = 250;
            playSet.y = 27;
            
            expand.x = playSet.x + playSet.width + 90;
            expand.y = 40;
            
            addChild(background);
            addChild(scrubber);
            addChild(download);
            addChild(playSet);
            addChild(expand);
            
            playSet.addEventListener(PlaySet.PLAY_CLICK, onPlayClick);
            playSet.addEventListener(PlaySet.FORWARD_DOWN, onForwardDown);
            playSet.addEventListener(PlaySet.FORWARD_UP, onForwardUp);
            playSet.addEventListener(PlaySet.REWIND_DOWN, onRewindDown);
            playSet.addEventListener(PlaySet.REWIND_UP, onRewindUp);
            
            download.addEventListener(MouseEvent.CLICK, onDownloadClick);
            expand.addEventListener(MouseEvent.CLICK, onExpandClick);
            
            scrubber.addEventListener(Scrubber.PROGRESS_CHANGE, onProgressChange);
        }
        
        // fscking ugly fix
        // The Garbage Collector collects this instance before the user returns.
        // So the instance that actually downloads once the user has clicked ok
        // is now null. So you place in class scope so it remains alive and the GC
        // doesnt pick it up. Must remember this problem for future refrence.
        protected var file:FileReference = new FileReference();
        
        public function onDownloadClick(e:MouseEvent):void
        {
            var downloadURL:URLRequest = new URLRequest();
            downloadURL.url = PlayerForAlan.DownloadLink;
            downloadURL.method = URLRequestMethod.POST;

            file.download(downloadURL);
        }
        
        public function onExpandClick(e:MouseEvent):void
        {
            navigateToURL(new URLRequest(PlayerForAlan.viewLargerUrl), "qtFrame");
        }
        
        protected var _forwardDown:Boolean = false;
        
        public function onForwardDown(e:Event):void
        {
            _forwardDown = true;
            Video.pause();
            Video.CurrentFrame ++;
            
            //timer
            var T:Timer = new Timer(1000, 1);
            
            T.addEventListener(TimerEvent.TIMER, onForwardDownTimer);
            T.start();
        }
        
        public function onForwardDownTimer(e:TimerEvent):void
        {
            if(_forwardDown)
                addEventListener(Event.ENTER_FRAME, onForwardEnterFrame);
        }
        
        
        public function onForwardEnterFrame(e:Event):void
        {
            Video.CurrentFrame ++;
        }
        
        public function onForwardUp(e:Event):void
        {
            _forwardDown = false;

            try
            {
                removeEventListener(Event.ENTER_FRAME, onForwardEnterFrame);
            }
            catch(e:Event){ }
        }
        
        protected var _rewindDown:Boolean = false;
        
        public function onRewindDown(e:Event):void
        {
            _rewindDown = true;
            Video.pause();
            Video.CurrentFrame = Video.CurrentFrame - 2;
            
            
            //timer
            var T:Timer = new Timer(1000, 1);
            
            T.addEventListener(TimerEvent.TIMER, onRewindDownTimer);
            T.start()
        }
        
        public function onRewindDownTimer(e:TimerEvent):void
        {
            if(_rewindDown)
                addEventListener(Event.ENTER_FRAME, onRewindEnterFrame);
        }
        
        public function onRewindEnterFrame(e:Event):void
        {
            Video.CurrentFrame = Video.CurrentFrame - 2;
        }
        
        public function onRewindUp(e:Event):void
        {
            _rewindDown = false;

            try
            {
                removeEventListener(Event.ENTER_FRAME, onRewindEnterFrame);
            }
            catch(e:Event){ }
        }
        
        public static function getInstance():PlayerUI
        {
            if(_instance == null)
                _instance = new PlayerUI();
                
            return _instance;
        }
        
        public function stop():void
        {
            if(!scrubber.Dragged)
                _stopped = true;
        }
        
        public function get Video():NewVid
        {
            if(video == null)
                video = NewVid.getInstance();
            
            return video;
        }
        
        public function onProgressChange(e:Event):void
        {
            Video.pause();
            _stopped = false;
            Video.Position = scrubber.Progress;
            //Video.CurrentFrame = scrubber.Progress * Video.Framerate * Video.Duration;
        }
        
        public function setPlayStatus(b:Boolean):void
        {
            if(b)
            {
                _playClicked = true
                playSet.showPaused();
            }
            else
            {
                _playClicked = false;
                playSet.showPlay();
            }
        }
        
        public function onPlayClick(e:Event):void
        {
            if(_stopped)
                Video.seek(0);
                
            if(_playClicked)
                Video.pause();
            else
                Video.play();
            
            //_playClicked = !_playClicked;
            _stopped = false;
        }
        
    }
}