<%@ Page Language="C#" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
<title>H.264 Flash Player</title>
<script language="javascript" type="text/javascript" src="scripts.js"></script>
</head>
<body>
<script src="AC_OETags.js" language="javascript"></script>
<script language="JavaScript" type="text/javascript">
<!--
// -----------------------------------------------------------------------------
// Globals
// Major version of Flash required
var requiredMajorVersion = 9;
// Minor version of Flash required
var requiredMinorVersion = 0;
// Minor version of Flash required
var requiredRevision = 115;
// -----------------------------------------------------------------------------
// -->
</script>
<script language="JavaScript" type="text/javascript">
<!--
// Version check based upon the values entered above in "Globals"
var hasReqestedVersion = DetectFlashVer(requiredMajorVersion, requiredMinorVersion, requiredRevision);
if (!hasReqestedVersion) {
	var alternateContent = 'This site requires the latest version of the Adobe Flash Player. '
	+ '<a href=http://www.adobe.com/go/getflash/>Get Flash</a>';
	document.write(alternateContent);  // insert non-flash content
} else {
	renderFlash("player.swf?video=nasa_shuttle_m420p.mov&videoAutoPlay=false&downloadURL=sample.flv&viewLargerURL=test.qtl&unique=51687384", 541, 382, 'wmode="transparent"');
}
</script>
<br /><br />
<a href="default.aspx?vid=HD_5994">HD_5994 Sample</a><br />
<a href="default.aspx?vid=NTSC_30">NTSC_30 Sample</a><br />
<a href="default.aspx?vid=NTSC_2397">NTSC_2397 Sample</a><br />
<a href="default.aspx?vid=NTSC_2997">NTSC_2997 Sample</a><br />
<a href="default.aspx?vid=PAL_25">PAL_25 Sample</a><br />
</body>
</html>