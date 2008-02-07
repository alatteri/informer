package Player.UI
{
    import flash.display.Bitmap;
    
    public class RewindButton extends BaseSideButton
    {
        
		[Embed(source="/images/forward.png")]
		private var rewindImage:Class
		
		protected var rewind:Bitmap;
        public function RewindButton()
        {
            super();
            rewind = new rewindImage();
            addChild(rewind);
            rewind.smoothing = true;
            
            //back.rotation = 180
            
            scaleX = -1;
        }
        
    }
}