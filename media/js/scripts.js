// Globals
// Major version of Flash required
var requiredMajorVersion = 9;
// Minor version of Flash required
var requiredMinorVersion = 0;
// Minor version of Flash required
var requiredRevision = 115;

var requiredVersion = requiredMajorVersion + '.' + requiredMinorVersion + '.' + requiredRevision;

/* wrapper function to embed the flash player. */
function FlashPlayer(movie_hi, movie_lo, div) {
    embedFlashPlayerCode(movie_hi, movie_lo, 544, 382, false, 'player.swf', div);
}

/* wrapper function embed the mini-flash player. */
function MiniFlashPlayer(movie_hi, movie_lo, div) {
    embedFlashPlayerCode(movie_hi, movie_lo, 160, 90, false, 'miniPlayer.swf', div);
}

/* wrapper function to embed the flash player.
   Returns error message string if required flash version not found.

   Args:
    movie_hi: url to the high-res version of the movie
    movie_lo: url to the low-res version of the movie
    width:    the requested width of the flash player
    height:   the requested height of the flash player
    autoplay: should the movie start playing automatically? (bool)
    player:   string to specify the type of player to get
    div:      the div to render to use for embedding the player
*/
function embedFlashPlayerCode(movie_hi, movie_lo, width, height, autoplay, player, div) {
    now = new Date().getTime();
    var src = "/media/flash/" + player + '?';
    src += "video=" + movie_lo + "&";
    src += "videoAutoPlay=" + autoplay + "&";
    src += "downloadURL=" + movie_hi + "&";
    src += "viewLargerURL=/media/flash/qtFrame.html%3Fmovie%3D" + movie_hi + "&";
    src += "unique=" + now;

    var params = {
        align: 'middle',
        wmode: 'transparent',
        quality: 'high',
        allowscriptaccess:'sameDomain'
    };

    swfobject.embedSWF(src, div, width, height, requiredVersion, "/media/flash/expressInstall.swf", '', params);
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
