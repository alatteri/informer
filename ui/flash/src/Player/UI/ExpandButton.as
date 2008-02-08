package Player.UI
{
    import flash.display.Bitmap;
    import flash.text.TextField;
    import flash.text.TextFormat;
    import flash.filters.DropShadowFilter;
    
    public class ExpandButton extends BaseMiniButton
    {
        
		[Embed(source="/images/expand.png")]
		private var expandImage:Class
		
		protected var expand:Bitmap;
		protected var expandText:TextField = new TextField();
		protected var textFormat:TextFormat = new TextFormat("Arial", 12, 0xFFFFFF);
		
        public function ExpandButton()
        {
            super();
            expand = new expandImage();
            addChild(expand);
            
            expandText.defaultTextFormat = textFormat;
            expandText.selectable = false;
            expandText.text = "View Larger";
            expandText.height = expandText.textHeight + 4;
			expandText.x = expand.width + 5;
			
            expandText.filters = [ new DropShadowFilter(2) ];
			
			addChild(expandText);
        }
        
        
    }
}