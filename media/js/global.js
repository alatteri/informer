/* Window Load Events
 * ------------------------------------------------------ */

if(!isElements_browser()) addLoadEvent(styleSelects);

/* Application Functions
 * ------------------------------------------------------ */

/* Adds hover functionality to IE6 on non-link elements */
function addIEOverviewHoverStyles() {

	// Return if not overview page
	if(document.body.id != "overview")
		return;

	// Add hover states to list items
	list_items = document.getElementById('overview_entries').getElementsByTagName('li');
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

/* Styles <select>'s */
function styleSelects() {
	var s = document.getElementsByTagName('select');
     for (var i=0; i<s.length; i++) {
       selectReplacement(s[i]);
     }
}

/* Highlights given table heading (assumes given heading is a <li>)*/
function highlightHeading(li, log) {
    // If not restoring the highlighted column
	if(!Element.hasClassName(li, 'highlight')) {
        list_items = li.parentNode.getElementsByTagName('li');
		for(var i=0; i<list_items.length; i++)
		    if(Element.hasClassName(list_items[i], 'highlight'))
	            list_items[i].className = list_items[i].className.split(' ')[0];
	} 
	
	// Highlight with appropriate up or down arrow
	if (Element.hasClassName(li, 'down'))
	    li.className = log._sorter + " up highlight";
	else
        li.className = log._sorter + " down highlight";
	    
	// Highlight entire column
	highlightColumn(log._sorter);
}

/* Highlights column with given sorter name */
function highlightColumn(sorter) {
	if(document.body.id == "overview")
		var column_cells = $('overview_entries').getElementsByTagName('span');
	else
		var column_cells = $(document.body.id+'_entries').getElementsByTagName('li');
	
	for(var i=0; i <column_cells.length; i++) {
        Element.removeClassName(column_cells[i], 'highlight');
        if(Element.hasClassName(column_cells[i], sorter))
            Element.addClassName(column_cells[i], 'highlight');
	}
}


/* Adds links to relvent log items */

function addLogItemLinks() {
	var deleted_shots = new Hash();
	var logs = $$('span.activity');
	
	// Populate hash with deleted objects
	for(var i=0; i<logs.length; i++) {
		if(Element.hasClassName(logs[i], 'deleted'))
			deleted_shots.set(logs[i].className.split(' ')[3], true);
	}
	
	// Add links to any undeleted note, element, and render log items
	for(var i=0; i<logs.length; i++) {
		if(!Element.hasClassName(logs[i], 'shot') && !Element.hasClassName(logs[i], 'deleted') && !deleted_shots.get(logs[i].className.split(' ')[3]))
			createLogLink(logs[i].className.split(' ')[1],logs[i].className.split(' ')[3],logs[i]);
	}
}

/* Creates a log link given the object's type and id */
function createLogLink(object_type, object_id, object) {
	var link = document.createElement('A');
	var link_text = document.createTextNode("View " + object_type);
	link.appendChild(link_text);
	link.title = "View " + object_type;
	link.href = object_type + "s/#" + object_id;
	object.appendChild(link);
}


/* Helper Functions
 * ------------------------------------------------------ */

function selectReplacement(obj) {
	obj.className += ' replaced';
	var ul = document.createElement('ul');
	ul.className = 'selectReplacement';
	var opts = obj.options;
	for (var i=0; i<opts.length; i++) {
		var selectedOpt;
		if (opts[i].selected) {
			selectedOpt = i;
			break;
		} else {
			selectedOpt = 0;
		}
	}

	for (var i=0; i<opts.length; i++) {
		var li = document.createElement('li');
		var txt = document.createTextNode(opts[i].text);
		li.appendChild(txt);
		li.selIndex = opts[i].index;
		li.selectID = obj.id;
		li.onclick = function() {
			selectMe(this);
		}
		if (i == selectedOpt) {
			li.className = 'selected';
			li.onclick = function() {
				this.parentNode.className += ' selectOpen';
				this.onclick = function() {
					selectMe(this);
				}
			}
		}
		
		li.onmouseover = function() {
			if(this.className != "selected")
				this.className = "hover";
		}
		li.onmouseout = function() {
			if(this.className != "selected")
				this.className = "";
		}
		ul.appendChild(li);
	}
	obj.parentNode.insertBefore(ul,obj);
}

function selectMe(obj) {
	var lis = obj.parentNode.getElementsByTagName('li');
	for (var i=0; i<lis.length; i++) {
		if (lis[i] != obj) {
			lis[i].className='';
			lis[i].onclick = function() {
				selectMe(this);
			}
		} else {
			setVal(obj.selectID, obj.selIndex);
			obj.className='selected';
			obj.parentNode.className = 
			obj.parentNode.className.replace(new RegExp(" selectOpen\\b"), '');
			obj.onclick = function() {	
				obj.parentNode.className += ' selectOpen';
				this.onclick = function() {
					selectMe(this);
				}
			}
		}
	}
	if(isElements_browser()) browse(obj.innerHTML);
}

function setVal(objID, selIndex) {
	var obj = document.getElementById(objID);
	obj.selectedIndex = selIndex;
}

function isElements_browser() {
	return (window.location+"").search('elements_browser') >= 0;
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
