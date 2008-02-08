package Player.UI
{
    import flash.display.Sprite;
    import flash.events.Event;
    import flash.events.MouseEvent;

    public class PlaySet extends Sprite
    {
        public static const PLAY_CLICK:String = "PLAY_CLICK";
        public static const FORWARD_DOWN:String = "FORWARD_DOWN";
        public static const FORWARD_UP:String = "FORWARD_UP";
        public static const REWIND_DOWN:String = "REWIND_DOWN";
        public static const REWIND_UP:String = "REWIND_UP";
        
        protected var rewind:RewindButton = new RewindButton();
        protected var forward:FastForwardButton = new FastForwardButton();
        protected var play:PlayButton = new PlayButton();
        
        public function PlaySet()
        {
            super();
            
            play.x = -1;
            forward.x = play.width - 2;
            forward.y = rewind.y = 4;
            
            addChild(rewind);
            addChild(forward);
            addChild(play);
            
            play.addEventListener(MouseEvent.CLICK, onPlayClick);
            forward.addEventListener(MouseEvent.MOUSE_DOWN, onForwardMouseDown);
            forward.addEventListener(MouseEvent.MOUSE_UP, onForwardMouseUp);
            rewind.addEventListener(MouseEvent.MOUSE_DOWN, onRewindMouseDown);
            rewind.addEventListener(MouseEvent.MOUSE_UP, onRewindMouseUp);
        }
        
        public function showPlay():void
        {
            play.showPlay();
        }
        
        public function showPaused():void
        {
            play.showPaused();
        }
        
        protected function onPlayClick(e:MouseEvent):void
        {
            dispatchEvent(new Event(PLAY_CLICK));
        }
        
        protected function onForwardMouseDown(e:MouseEvent):void
        {
            dispatchEvent(new Event(FORWARD_DOWN));
        }
        
        protected function onForwardMouseUp(e:MouseEvent):void
        {
            dispatchEvent(new Event(FORWARD_UP));
        }
        
        protected function onRewindMouseDown(e:MouseEvent):void
        {
            dispatchEvent(new Event(REWIND_DOWN));
        }
        
        protected function onRewindMouseUp(e:MouseEvent):void
        {
            dispatchEvent(new Event(REWIND_UP));   
        }
    }
}