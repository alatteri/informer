package Player.UI
{
    import com.deviant.MyStage;
    
    import flash.display.GradientType;
    import flash.display.SpreadMethod;
    import flash.display.Sprite;
    import flash.events.Event;
    import flash.events.MouseEvent;
    import flash.geom.Matrix;
    import flash.text.TextField;
    import flash.text.TextFormat;
    import flash.text.TextFormatAlign;
    
    public class Scrubber extends Sprite
    {   
        protected static const WIDTH:Number = 448;
        protected static const Y_OFFSET:Number = 5;
        protected static const OVERALL_OFFSET:Number = 40;
        protected static const HEAD_OFFSET:Number = 0;
        protected static const HEAD_WIDTH:Number = WIDTH - HEAD_OFFSET * 2;
        
        protected var currText:TextField = new TextField();
        protected var maxText:TextField = new TextField();
        protected var load:Sprite = new Sprite();
        protected var scrub:Sprite = new Sprite();
        protected var head:Sprite = new Sprite();
        protected var hitMask:Sprite = new Sprite();
        protected var edging:Sprite = new Sprite();
        
        protected var _progress:Number = 0;
        protected var _loadProgress:Number = 0;
        protected var _currFrame:Number = 0;
        protected var _maxFrame:Number = 0;
        protected var _dragged:Boolean = false;
        
        public function Scrubber()
        {
            super();
            
            with(head.graphics)
            {
                beginFill(0xc2c2c2);
                drawRoundRect(0,0,5, 5, 0, 0);
                endFill();
            }
            
            head.rotation = 45;
            head.y = Y_OFFSET + 1;
            Progress = 0;
            
            with(hitMask.graphics)
            {
                beginFill(0x000000, 0);
                drawRect(0,0,10,9);
                endFill();
            }
            
            hitMask.x = OVERALL_OFFSET;
            hitMask.y = Y_OFFSET;
            
            with(scrub.graphics)
            {
                beginFill(0x555555, 1);
                lineStyle(1, 0x7f8183, 1, true);
                drawRoundRect(0,0,WIDTH,9, 0, 0);
                endFill();
            }
            
            scrub.x = OVERALL_OFFSET;
            scrub.y = Y_OFFSET;
            
            with(edging.graphics)
            {
                beginFill(0x555555, 0);
                lineStyle(1, 0x7f8183, 1, true);
                drawRoundRect(0,0,WIDTH,9, 0, 0);
                endFill();
            }
            
            edging.x = OVERALL_OFFSET;
            edging.y = Y_OFFSET;
            
            with(load.graphics)
            {
                var M:Matrix = new Matrix();
                // matt: again Adobe not keeping rotation constant either gradient or radians not both please ...
                M.createGradientBox(10,9,Math.PI/2);

                // matt: Adobe piss me off with their stupid non conformist alpha crap
                //       0-1 or 0-100 but keep it constant guys ffs...
                beginGradientFill(GradientType.LINEAR, [0x323436, 0x1b1e21], [100, 100], [0x00, 0xFF], M, SpreadMethod.PAD);  
                drawRect(0,0,10,9);
                endFill();
            }
            
            load.x = OVERALL_OFFSET + 1;
            load.y = Y_OFFSET;
            LoadProgress = 0;
            
            currText.defaultTextFormat = new TextFormat("Arial", 12, 0x888888, null, null, null, null, null, TextFormatAlign.RIGHT);
            currText.selectable = false;
            currText.text = "0";
            currText.width = OVERALL_OFFSET - 5;
            currText.height = currText.textHeight + 4;
            
            maxText.defaultTextFormat = new TextFormat("Arial", 12, 0x888888, null, null, null, null, null, TextFormatAlign.LEFT);
            maxText.selectable = false;
            maxText.text = "0";
            maxText.width = 35;
            maxText.x = currText.width + scrub.width + 10;
            maxText.height = currText.textHeight + 4;
           
            addChild(scrub);
            addChild(load);
            addChild(edging);
            addChild(hitMask);
            addChild(head);
            addChild(currText);
            addChild(maxText);
            
            head.addEventListener(MouseEvent.MOUSE_DOWN, onHeadMouseDown);
            hitMask.addEventListener(MouseEvent.MOUSE_DOWN, onHeadMouseDown);
            hitMask.addEventListener(MouseEvent.MOUSE_DOWN, onLoadClick);
            MyStage.getStage().addEventListener(MouseEvent.MOUSE_UP, onMouseUp);
        }
        
        protected function onHeadMouseDown(e:MouseEvent):void
        {
            _dragged = true;
            head.x = e.stageX - x - OVERALL_OFFSET;
            Progress = (head.x - HEAD_OFFSET) / HEAD_WIDTH;
            
            MyStage.getStage().addEventListener(MouseEvent.MOUSE_MOVE, onMouseMove);
            
            dispatchEvent(new Event(Event.CHANGE));
        }
        
        protected function onMouseMove(e:MouseEvent):void
        {
            head.x = e.stageX - x - OVERALL_OFFSET;
            Progress = (head.x - HEAD_OFFSET) / HEAD_WIDTH;
            
            dispatchEvent(new Event(Event.CHANGE));
        }
        
        protected function onMouseUp(e:MouseEvent):void
        {
            try
            {
                MyStage.getStage().removeEventListener(MouseEvent.MOUSE_MOVE, onMouseMove);
            }
            catch(e:Error) { }
            _dragged = false;
        }
        
        public function get Dragged():Boolean
        {
            return _dragged;
        }
        
        protected function onLoadClick(e:MouseEvent):void
        {
            head.x = e.stageX - x - OVERALL_OFFSET;
            Progress = (head.x - HEAD_OFFSET) / HEAD_WIDTH;
            
            dispatchEvent(new Event(Event.CHANGE));
        }
        
        public function get NewEstimatedFrame():Number
        {
            return Math.round(MaxFrame * Progress);
        }
        
        public function get CurrentFrame():Number
        {
            return _currFrame;
        }
        
        public function set CurrentFrame(n:Number):void
        {
            _currFrame = Math.min(n, MaxFrame);
            
            if(isNaN(_currFrame))
                _currFrame = 0;
                
            currText.text = String(_currFrame);
        }
        
        public function get MaxFrame():Number
        {
            return _maxFrame;
        }
        
        public function set MaxFrame(n:Number):void
        {
            _maxFrame = n;
            
            if(isNaN(_maxFrame))
                _maxFrame = 1;
                
            maxText.text = String(_maxFrame);
        }
        
        public function get LoadProgress():Number
        {
            return _loadProgress;
        }
        
        public function set LoadProgress(n:Number):void
        {
            _loadProgress = (n > 1)?1:(n<0)?0:n;
            
            load.width = (WIDTH - 1)*_loadProgress;
            hitMask.width = load.width;
        }
        
        public function get Progress():Number
        {
            return _progress;
        }
        
        public function set Progress(n:Number):void
        {
            _progress = (n > 1)?1:(n<0)?0:n;
            
            head.x = OVERALL_OFFSET + HEAD_OFFSET + load.width*_progress;
            
            head.x = (head.x > (load.width + HEAD_OFFSET + OVERALL_OFFSET))?(load.width + HEAD_OFFSET + OVERALL_OFFSET): head.x;
        }

    }
}