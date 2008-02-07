package Player.UI
{
    import com.deviant.MyStage;
    
    import flash.display.Bitmap;
    import flash.display.Sprite;
    import flash.events.MouseEvent;

    public class BaseSideButton extends Sprite
    {
   		[Embed(source="/images/sideBackButton.png")]
		private var backImage:Class
		
		protected var back:Bitmap;
        public function BaseSideButton()
        {            
            back = new backImage();
            addChild(back);
            
            back.smoothing = true;
            
            addEventListener(MouseEvent.MOUSE_DOWN, onMouseDown);
            MyStage.getStage().addEventListener(MouseEvent.MOUSE_UP, onMouseUp);
        }
        
        protected function onMouseDown(e:MouseEvent):void
        {
            back.scaleY = -1;
            back.y = back.height;
        }
        
        protected function onMouseUp(e:MouseEvent):void
        {
            back.scaleY = 1;
            back.y = 0;
        }
    }
}