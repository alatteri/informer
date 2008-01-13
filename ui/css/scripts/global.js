addLoadEvent(addIEOverviewHoverStyles);

function addIEOverviewHoverStyles() {

	// Return if not overview page
	if(document.body.id != "overview")
		return;

	var lists = document.getElementsByTagName('ul');
	var list_items;
	
	// Find entry list elements
	for(var i=0; i<lists.length; i++) {
		if(lists[i].className == "entries")
			list_items = lists[i].getElementsByTagName('li');
	}
	
	// Add hover states to list items
	for(var i=0; i<list_items.length; i++) {
		list_items[i].onmouseover = function(){
			if(this.className == "last")
				this.className = "lastHover";
			else
				this.className = "hover";
		}
		
		list_items[i].onmouseout = function(){
			if(this.className == "lastHover")
				this.className = "last";
			else
				this.className = "";
		}
		
	}
}


function addLoadEvent(func) {
  var oldonload = window.onload;
  if (typeof window.onload != 'function') {
    window.onload = func;
  } else {
    window.onload = function() {
      oldonload();
      func();
    }
  }
 }