// Globals
// Major version of Flash required
var requiredMajorVersion = 9;
// Minor version of Flash required
var requiredMinorVersion = 0;
// Minor version of Flash required
var requiredRevision = 115;

/* wrapper function to get code for the flash player.
   Returns error message string if required flash version not found.

   Args:
    movie_hi: url to the high-res version of the movie
    movie_lo: url to the low-res version of the movie
    width: the requested width of the flash player
    height: the requested height of the flash player
    autoplay: should the movie start playing automatically? (bool)
*/
function getFlashPlayer(movie_hi, movie_lo, width, height, autoplay) {
    // Version check based upon the values entered above in "Globals"
    var hasReqestedVersion = DetectFlashVer(requiredMajorVersion, requiredMinorVersion, requiredRevision);
    if (!hasReqestedVersion) {
        var msg = 'This site requires the latest version of the Adobe Flash Player. ';
        var a = document.createElement('a');
        a.href = 'http://www.adobe.com/go/getflash/';
        a.appendChild(document.createTextNode(msg));
        // return non-flash content
        return a;
    } else {
        if (autoplay == null) autoplay = false;
        if (width == null) width = 544;
        if (height == null) height = 382;

        now = new Date().getTime();
        var src = "/site_media/flash/player.swf?";
        src += "video=" + movie_lo + "&";
        src += "videoAutoPlay=" + autoplay + "&";
        src += "downloadURL=" + movie_hi + "&";
        src += "viewLargerURL=/site_media/flash/qtFrame.html%3Fmovie%3D" + movie_hi + "&";
        src += "unique=" + now;

        var player = document.createElement('embed');
        player.src = src;
        player.quality = 'high';
        player.width = width;
        player.height = height;
        player.align = 'middle';
        player.allowscriptaccess = 'sameDomain';
        player.type = 'application/x-shockwave-flash';
        player.wmode = 'transparent';
        player.pluginspage = 'http://www.macromedia.com/go/getflashplayer';
        return player;       
    }
}

function renderQuickTimeLauncher(clip) {
	  document.writeln('<embed src="placeholder.mov" autohref="true" target="quicktimeplayer" controller="false" width="114" height="27" autoplay="true" href="' + clip + '"></embed>');
  }

function getArgs() {
    var args = new Array();
    var query = window.location.search.substring(1);
    var parms = query.split('&');

    for (var i=0; i<parms.length; i++) {
        var pos = parms[i].indexOf('=');
        if (pos > 0) {
            var key = parms[i].substring(0,pos);
            var val = parms[i].substring(pos+1);
            args[key] = val;
        }
    }

    return args;
}
