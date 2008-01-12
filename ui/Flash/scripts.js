 function renderFlash(src, width, height, features) {
	document.writeln('<embed src="' + src + '" quality="high" width="' + width + 
					 '" height="' + height + '" align="middle" allowscriptaccess="sameDomain" ' +
					 'type="application/x-shockwave-flash" ' + features + 
  					 'pluginspage="http://www.macromedia.com/go/getflashplayer" />');
  }