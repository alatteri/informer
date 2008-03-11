package Player.UI
{
    import flash.display.Bitmap;
    import flash.display.Sprite;
    
    public class FastForwardButton extends BaseSideButton
    {
        
		[Embed(source="/images/forward.png")]
		private var forwardImage:Class
		
		protected var forward:Bitmap;
        public function FastForwardButton()
        {
            super();
            forward = new forwardImage();
            addChild(forward);
            forward.smoothing = true;
            
            //scaleX = scaleY = 0.75;
        }
        
    }
}