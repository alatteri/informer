package 
{
    import Player.MainPlayer;
    
    import com.deviant.MyStage;
    
    import flash.display.LoaderInfo;
    import flash.display.Sprite;
    import flash.system.Security;
    
	[SWF(frameRate="35", backgroundColor="#44474A")]
    public class PlayerForAlan extends Sprite
    {
        public static var DownloadLink:String = "COUNTER_NTSC_60.mov";
        public static var Video:String = "COUNTER_NTSC_60.mov";
        public static var AutoPlay:Boolean = true;
        public static var viewLargerUrl:String = "COUNTER_NTSC_60.mov";

        public function PlayerForAlan()
        {
            MyStage.init(stage);
            
            //Security.allowDomain("*");
            
            var queryString:Object = LoaderInfo(root.loaderInfo).parameters;
            Video = queryString["video"];
            AutoPlay = Boolean(queryString["videoAutoPlay"]);
            DownloadLink = queryString["downloadURL"];
            viewLargerUrl = queryString["viewLargerURL"];
            
            addChild( new MainPlayer() );
        }
    }
}