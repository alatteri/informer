/* Window Load Events
 * ------------------------------------------------------ */
addLoadEvent(addIEOverviewHoverStyles);
addLoadEvent(styleSelects);


/* Application Functions
 * ------------------------------------------------------ */

/* Adds hover functionality to IE6 on non-link elements */
function addIEOverviewHoverStyles() {

	// Return if not overview page
	if(document.body.id != "overview")
		return;

	var lists = document.getElementsByTagName('ul');
	var list_items = new Array();
	
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

/* Styles <select>'s */
function styleSelects() {
	var s = document.getElementsByTagName('select');
     for (var i=0; i<s.length; i++) {
       selectReplacement(s[i]);
     }
}

/* Highlights given table heading (assumes given heading is a <li>)*/
function highlightHeading(li, log) {
    list_items = li.parentNode.getElementsByTagName('li');

    // Clears highlight class
    for(var i=0; i<list_items.length; i++)
        list_items[i].className = list_items[i].className.split(' ')[0] + " " + list_items[i].className.split(' ')[1];
    
	li.className = log._sorter + " " + log._reversed + " highlight";
	
	highlightColumn(log._sorter);
}


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
