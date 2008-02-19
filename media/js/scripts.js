 function renderFlash(src, width, height, features) {
	document.writeln('<embed src="' + src + '" quality="high" width="' + width + 
					 '" height="' + height + '" align="middle" allowscriptaccess="sameDomain" ' +
					 'type="application/x-shockwave-flash" ' + features + 
  					 'pluginspage="http://www.macromedia.com/go/getflashplayer" />');
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
