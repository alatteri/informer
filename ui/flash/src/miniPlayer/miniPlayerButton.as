package
{
    import com.deviant.layout.AlignToParent;
    
    import flash.display.GradientType;
    import flash.display.Sprite;
    import flash.events.MouseEvent;
    import flash.filters.GlowFilter;
    import flash.geom.Matrix;

    public class miniPlayerButton extends Sprite
    {
        protected var play:Sprite = new Sprite();
        protected var pause:Sprite = new Sprite();
        
        protected var paused:Boolean = false;
        
        public function miniPlayerButton()
        {
            with(graphics)
            {
                beginFill(0xFFFFFF, 0.25);
                drawRoundRect(0, 0, 45, 32, 5, 5);
                endFill();
            }
            
            var m:Matrix = new Matrix();
            m.createGradientBox(14, 17, Math.PI/2);
            
            with(play.graphics)
            {
                beginGradientFill(GradientType.LINEAR, [0xFFFFFF, 0xc3c3c3], [1, 1], [0x00, 0xFF], m);
                moveTo(0,0);
                lineTo(14, 8);
                lineTo(0, 16);
                endFill();
            }
            
            play.filters = [new GlowFilter(0xFFFFFF, .25, 6, 6, 2, 3)];
            
            m = new Matrix();
            m.createGradientBox(6, 20, Math.PI/2);
            
            var p1:Sprite = new Sprite();
            
            with(p1.graphics)
            {
                beginGradientFill(GradientType.LINEAR, [0xFFFFFF, 0xc3c3c3], [1, 1], [0x00, 0xFF], m);
                drawRect(0,0,6,20);
                endFill();
            }
            
            var p2:Sprite = new Sprite();
            
            with(p2.graphics)
            {
                beginGradientFill(GradientType.LINEAR, [0xFFFFFF, 0xc3c3c3], [1, 1], [0x00, 0xFF], m);
                drawRect(0,0,6,20);
                endFill();
            }
            
            pause.filters = [new GlowFilter(0xFFFFFF, .25, 6, 6, 2, 3)];
            
            p2.x = 12;
            pause.addChild(p1);
            pause.addChild(p2);
            
            addEventListener(MouseEvent.CLICK, onMouseClick);
            
            toPlay();
        }
        
        public function toPlay():void
        {
            if(!paused)
            {
                try
                {
                    removeChild(pause.parent);
                    pause.parent.removeChild(pause);
                } catch(e:Error) { trace(e); }
            }
            
            addChild(new AlignToParent(play, this));
            // if the play button is showing its paused
            paused = true;
        }
        
        public function toPause():void
        {
            if(paused)
            {
                try
                {
                    removeChild(play.parent);
                    play.parent.removeChild(play);
                } catch(e:Error) { trace(e); }
            }
            
            addChild(new AlignToParent(pause, this));
            // if the pause button is showing its playing
            paused = false;
        }
        
        protected function onMouseClick(e:MouseEvent):void
        {
            if(!paused)
                toPlay();
            else
                toPause();
        }
        
    }
}