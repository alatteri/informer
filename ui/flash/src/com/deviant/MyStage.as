/**
	My Stage
	version 2.0.0
	Created by Matthew Lloyd
	http://www.Matt-Lloyd.co.uk/
	
	This is release under a Creative Commons license. 
    More information can be found here:
	http://creativecommons.org/licenses/by-nc-sa/2.0/uk/
	
	----------------------------------------------------------------

*/
package com.deviant
{
	import flash.display.*;

	public class MyStage extends Sprite
	{
		public static var instance:MyStage = null;
		
		public function MyStage()
		{}
		
		public static function getInstance():MyStage
		{
			if(instance == null)
				instance = new MyStage();
			return instance;
		}
		
		public static function getStage():Stage
		{
			return getInstance().stage;
		}
		
		public static function init(stg:Stage):void
		{
			stg.addChild(MyStage.getInstance());
			stg.scaleMode = StageScaleMode.NO_SCALE;
			stg.align = StageAlign.TOP_LEFT;
		}
	}
}