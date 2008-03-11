package Player
{
    import flash.display.Sprite;
    import flash.events.Event;

    public class MainPlayer extends Sprite
    {
        
        protected var video:NewVid = NewVid.getInstance();
        protected var ui:PlayerUI = PlayerUI.getInstance();
        
        public function MainPlayer()
        {
            ui.y = 306;
            addChild(video);
            addChild(ui);
        }
        
        
    }
}