package Player.UI
{
    import flash.display.Bitmap;
    import flash.text.TextField;
    import flash.text.TextFormat;
    import flash.filters.DropShadowFilter;
    
    public class DownloadButton extends BaseMiniButton
    {
        
		[Embed(source="/images/download.png")]
		private var downImage:Class
		
		protected var down:Bitmap;
		protected var downText:TextField = new TextField();
		protected var textFormat:TextFormat = new TextFormat("Arial", 12, 0xFFFFFF);
		
        public function DownloadButton()
        {
            super();
            down = new downImage();
            addChild(down);
			
            downText.defaultTextFormat = textFormat;
            downText.selectable = false;
            downText.text = "Download Shot";
            downText.height = downText.textHeight + 2;
			downText.x = down.width + 5;
			
			downText.filters = [ new DropShadowFilter(2) ];
			
			addChild(downText);
        }
        
    }
}