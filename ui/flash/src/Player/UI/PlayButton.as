package Player.UI
{
    import flash.display.Bitmap;
    import flash.events.MouseEvent;
    
    public class PlayButton extends BaseBigButton
    {
        
		[Embed(source="/images/play.png")]
		private var playImage:Class;
		
		[Embed(source="/images/pause.png")]
		private var pauseImage:Class;
		
		protected var pause:Bitmap;
		protected var play:Bitmap;
		protected var _clicked:Boolean = false;
		
        public function PlayButton()
        {
            super();
            play = new playImage();
            pause = new pauseImage();
            
            addChild(play);
            addChild(pause);
            
            pause.visible = false;
            addEventListener(MouseEvent.CLICK, onClick);
        }
        
        public function showPlay():void
        {
            pause.visible = false;
            play.visible = true;
        }
        
        public function showPaused():void
        {
            pause.visible = true;
            play.visible = false;
        }
        
        public function onClick(e:MouseEvent):void
        {
            if(_clicked)
                showPlay();
            else
                showPaused();
            
            _clicked = !_clicked;
        }
    }
}