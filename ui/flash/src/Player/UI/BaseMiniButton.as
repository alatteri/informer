package Player.UI
{
    import com.deviant.MyStage;
    
    import flash.display.Bitmap;
    import flash.display.Sprite;
    import flash.events.MouseEvent;

    public class BaseMiniButton extends Sprite
    {
   		[Embed(source="/images/miniButtonBack.png")]
		private var backImage:Class
		
		protected var back:Bitmap;
        public function BaseMiniButton()
        {            
            back = new backImage();
            addChild(back);
            
            addEventListener(MouseEvent.MOUSE_DOWN, onMouseDown);
            MyStage.getStage().addEventListener(MouseEvent.MOUSE_UP, onMouseUp);
        }
        
        protected function onMouseDown(e:MouseEvent):void
        {
            back.rotation = 180;
            back.x = back.width;
            back.y = back.height;
        }
        
        protected function onMouseUp(e:MouseEvent):void
        {
            back.x = back.y = back.rotation = 0;
        }
    }
}