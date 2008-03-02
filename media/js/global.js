/* Window Load Events
 * ------------------------------------------------------ */
addLoadEvent(styleSelects);


/* Application Functions
 * ------------------------------------------------------ */

/* Adds hover functionality to IE6 on non-link elements */
function addIEOverviewHoverStyles() {

	// Return if not overview page
	if(document.body.id != "overview")
		return;

		list_items = document.getElementById('overview_entries').getElementsByTagName('li');
	
	alert(list_items.length);
	
	// Add hover states to list items
	for(var i=0; i<list_items.length; i++) {
		list_items[i].onmouseover = function(){
			if(this.className == "last")
				this.className = "lastHover";
			else
				this.className = "hover";
			if(link = this.getElementsByTagName('a')[0])
				link.style.display = "block";
		}
		
		list_items[i].onmouseout = function(){
			if(this.className == "lastHover")
				this.className = "last";
			else
				this.className = "";
			if(link = this.getElementsByTagName('a')[0])
				link.style.display = "none";			
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
	if(!li.hasClassName('highlight')) {
        list_items = li.parentNode.getElementsByTagName('li');
		for(var i=0; i<list_items.length; i++)
		    if(list_items[i].hasClassName('highlight'))
	            list_items[i].className = list_items[i].className.split(' ')[0];
	} 
	
	// Highlight with appropriate up or down arrow
	if (li.hasClassName('down'))
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
		column_cells[i].removeClassName('highlight');
		if(column_cells[i].hasClassName(sorter))
			column_cells[i].addClassName('highlight');
	}
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
		if (window.attachEvent) {
			li.onmouseover = function() {
				this.className += ' hover';
			}
			li.onmouseout = function() {
				this.className = 
				this.className.replace(new RegExp(" hover\\b"), '');
			}
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
}

function setVal(objID, selIndex) {
	var obj = document.getElementById(objID);
	obj.selectedIndex = selIndex;
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
