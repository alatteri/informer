function LOG(x) {
    if (window.console) {
        window.console.log(x);
    } else {
        alert(x);
    }
}

/* 
 * Informer Object
 * ------------------------------------------------------- */
var Informer = {};

/* 
 * Informer Filter: represents one filtered category
 * ------------------------------------------------------- */
Informer.Filter = Class.create();
Informer.Filter.prototype = {
    // initialize: create a new Filter object
    //      name:   string of the id of the UL element to use
    //      field:  the field description to filter
    //      format: optional formatting function
    initialize: function(name, field, format) {
        this.name = name;
        this.field = field;

        // the format function is optional
        this.format = format ? format : function(x) { return x };

        this.use = false;
        this.value = null;
        this.matches = $H();
    },

    // clears the match count for the Filter object
    reset_filter: function() {
        this.matches = $H();
    },

    // accepts a data object, increments internal counts for fields
    observe_data: function(data) {
        var val = this.format(get_value(this.field, data));
        var count = this.matches.get(val);
        if (count) {
            this.matches.set(val, count + 1);
        } else {
            this.matches.set(val, 1);
        }
    },

    // render the Filter object to the screen
    draw: function() {
        var ul = $(this.name);
        while (ul.hasChildNodes())
            ul.removeChild(ul.firstChild);

        var create_li = function (text, func) {
            var li = document.createElement('li');
            li.appendChild(document.createTextNode(text));
            Event.observe(li, 'click', func);
            return li;
        };

        var keys = this.matches.keys().sort();
        if (keys.length) {
            // XXX TODO: need to add click handlers
            // display the show all link first
            ul.appendChild(create_li('Show All', function(x) { }));

            // followed by the counts for all rows
            for (var i=0; i<keys.length; i++) {
                var key = keys[i];
                var text = key + ' (' + this.matches.get(key) + ')';
                var func = function(x) { };
                ul.appendChild(create_li(text, func));
            }
        } else {
            ul.appendChild(create_li('None', function(x) {} ));
        }
    },
};


/* 
 * Informer Filter Manager: manages several Filter objects
 * ------------------------------------------------------- */
Informer.FilterManger = Class.create();
Informer.FilterManger.prototype = {
    // initialize: create a new Filter Manager object
    initialize: function() {
        this.filters = $H();
    },
  
    // associate a Filter object with the Manger
    register_filter: function(filter) {
        this.filters.set(filter.name, filter);
    },
  
    // associates a data object with all registered filters
    register_data: function(data) {
        var names = this.filters.keys();
        for (var i=0; i<names.length; i++)
            this.filters.get(names[i]).observe_data(data);
    },

    // clears the match count for all registered filters
    reset_all_filters: function() {
        var names = this.filters.keys();
        for (var i=0; i<names.length; i++)
            this.filters.get(names[i]).reset_filter();
    },

    // render all filters
    draw: function() {
        var names = this.filters.keys();
        for (var i=0; i<names.length; i++)
            this.filters.get(names[i]).draw();
    },
};
  

/* 
 * Informer Data Object
 * ------------------------------------------------------- */

Informer.Data = Class.create();
Informer.Data.prototype = {
    
    /* Initialization constructor */
    initialize: function(name, row_1, row_2, filter_list) {
        this.name = name;
        this.row_1 = row_1;
        this.row_2 = row_2;
        this.entries = $(this.name + "_entries");
        this.fm = new Informer.FilterManger();
        this.data = undefined;
        this.url = undefined;
        this._sorter = undefined;
        this._reversed = false;
      
        if (filter_list) {
            filter_list.each(function(x) {
                this.fm.register_filter(x);
            }.bind(this)); 
        }
    },

	/* Loads Informer data */
    load_data: function(data) {
        if (this.data) return;

        if (data) {
            this.setup_data(data);
            return;
        }

        var a = new Ajax.Request(this.url, {
            method: 'GET',
            onSuccess: function(r) {
                this.setup_data(r.responseText);
            }.bind(this)
        });
    },

    /* Preprocesses data and populates table */
    setup_data: function(data) {
        if ('String' == typeof(data))
            this.data=eval(data);
        else
            this.data=data;
        
        this.pre_process();
        this.populate_table();
    },

    /* Preprocesses data */
    pre_process: function() {
        this.fm.reset_all_filters();
		  
        for (var i=0; i<this.data.length; i++) {
            var obj = this.data[i];
            this.fm.register_data(obj);
			  
            for (var j=0; j<this.row_1.length; j++) {
                var col = this.row_1[j];
                val = get_value(col.field, obj);

                if (col.parser) {
                    // use the column's parser if specified
                    val = col.parser(val);
                }

                set_value(col.field, obj, val);
            }
        }  

        this.fm.draw();
    },

    /* Resorts data given passed sorter */
    resort_table: function(which) {
        this.clear_table();	
        if (which == null) {
	        this.sort_data();

            if (this._reversed) {
                // sorting undoes the reverse order
                // reverse it again if it was previously
                this.reverse_table();
            }
        } else if (this._sorter == which) {
            this.reverse_table();
            this._reversed = !this._reversed;
	    } else {
	        this._sorter = which;
	        this.sort_data();
        }
        this.populate_table();
    },
    
	/* Populates table with data */
    populate_table: function() {
        this.data.each(function (item) {
            if (this.row_2)
                this.create_entry(item);
           else
                this.create_log_entry(item);
        }.bind(this));
    },

    /* Clears table of all data */
    clear_table: function() {
        while (this.entries.hasChildNodes())
            this.entries.removeChild(this.entries.firstChild);	
    },

    /* Sorts data given current sorter */
    sort_data: function() {
        var sorter = this.row_1.find(function(x) { 
            return x.name == this._sorter; 
        }.bind(this));

        if (sorter) {
            if (sorter.sorter) {
                this.data = this.data.sortBy(function (x) {
                    return sorter.sorter(get_value(sorter.field, x));
                });
            } else {
                this.data = this.data.sortBy(function (x) { 
                    return get_value(sorter.field, x); 
                });
            }
        }
    },
	
	/* Reverses given data and flips reversed flag */
    reverse_table: function() {
        this.data.reverse();
    },
    
    /* Creates table entry (note, render, or clip) given passed data item */
    create_entry: function(item) {
        var li = document.createElement('LI');
        li.id = item.pk;
        var entry = document.createElement('DIV');
        entry.className = 'entry';
        var ul = document.createElement('UL');
        ul.className = 'heading';
        this.create_entry_item(item, 'LI', ul);
        entry.appendChild(ul);
      
        var content = document.createElement('DIV');
        content.className = 'content';
        var val = get_value(this.row_2, item);
        if (val.tagName) {
            content.appendChild(val);
        } else {
            var p = document.createElement('P');
            p.appendChild(document.createTextNode(val));
            content.appendChild(p);
        }
        entry.appendChild(content);
        li.appendChild(entry);
        this.entries.appendChild(li);
    },
    
    /* Creates a log entry given passed data item */
    create_log_entry: function(item) {
        var li = document.createElement('LI');
        this.create_entry_item(item, 'SPAN', li);
        this.entries.appendChild(li);
    },
    
	/* Creates entry item */ 
    create_entry_item: function(item, tag, parent) {
        for (var i=0; i<this.row_1.length; i++) {
            var info = this.row_1[i];
            var elem;
            if (info.create_func) {
                elem = info.create_func(item);
            } else {
                var value = get_value(info, item);
                elem = document.createElement(tag);
                elem.className = info.name;
                elem.appendChild(document.createTextNode(value));
            }
            parent.appendChild(elem);
            
        }

        // Adds the close button if we're on the notes page
        var button = document.createElement('li');
        if(document.body.id == "notes" || document.body.id == "elements") {
            button.appendChild(document.createTextNode('Close'));
            button.className = "close";
        } else if(document.body.id == "renders") {
            button.appendChild(document.createTextNode('Update'));
            button.className = "update";
        }
        parent.appendChild(button);
    },

    /* Turns off given filter and repopulates table */
    turn_off: function(which) {
        this.fm.filters.get(which).use = false;
        this.populate_table();
    },

    /* Sets the filter value and repopulates the table */
    set_value: function(which, value) {
        var f = this.fm.filters.get(which);
        f.use = true;
        f.value = value;
        this.populate_table();
    },

    /* Adds given data object, preprocesses it, and repopulates table */     
    add_data: function(d) {
        this.data.push(d);
        this.pre_process();
    }
};
  
/* 
 * Helper Functions
 * ------------------------------------------------------- */

function sort_by_date(d) {
    return d.getTime();
}

function sort_by_string(s) {
    return (s+"").toLowerCase();
}

function sort_by_assigned(u) {
    return sort_by_string(format_assigned(u));
}
  
function get_value(field, object) {
    var filter, f;
    if (field.pop) {
        f = field[0];
        filter = field[1];
    }
    else if (field.field) {
        f = field.field;
        filter = field.formatter;
    }
    else {
        f = field;
    }
    var tmp = object;
    var fields = f.split('.');
    for (var i=0; i<fields.length; i++)
        tmp = tmp[fields[i]];
    if (filter)
        tmp = filter(tmp);
    return tmp;
}
  
function set_value(field, object, value) {
    var fields = field.split('.');
    fields.reverse();
    return _set_value(fields, object, value);
}
  
function _set_value(fields, object, value) {
    var f = fields.pop();
    tmp = object[f];
   
    if (!fields.length)
        object[f] = value;
    else
        object[f] = _set_value(fields, tmp, value);
    return object;
}
  
function parse_date(strDate) {
    if (!strDate || strDate.getMonth)
        return strDate;
    var nums = strDate.match(/(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2})/);
    var d = new Date();
    d.setFullYear(nums[1], nums[2]-1, nums[3]);
    d.setHours(nums[4]);
    d.setMinutes(nums[5]);
    d.setSeconds(nums[6]);
    return d;
}

// slight hack to get around the json sometimes
// evaluating boolean false to the string 'false'
function parse_boolean(b) {
    if (b && 'false' == b) {
        return false;
    } else if (b) {
        return true;
    } else {
        return false;
    }
}
  
/* accepts a Date object, returns pretty stringified Date */
function format_date(d) {	
    month = (d.getMonth()+1) < 10?'0'+(d.getMonth()+1):(d.getMonth()+1);
    day = d.getDate() < 10?'0'+d.getDate():d.getDate();
    return month+'/'+day+'/'+d.getFullYear();
}

/* accepts a boolean, returns string describing bool for Note status */
function format_pending(p) {
    return parse_boolean(p) ? 'Completed' : 'Pending';
}

/* accepts string, returns pretty formatted username. Defaults to 'Anyone' */
function format_assigned(u) {
    return u ? format_author(u) : 'Anyone';
}

/* accepts string, capitalizes first letter */
function format_author(u) {
     return u.substring(0,1).toUpperCase() + u.substring(1,u.length);	
}

function format_nl2br(txt) {
    var lines = txt.strip().split('\n');
    var p = document.createElement('p');
    for (var i=0; i<lines.length; i++) {
        p.appendChild(document.createTextNode(lines[i]));
        if (i>0)
            p.appendChild(document.createElement('br'));
    }
	return p;
}

function create_log(info) {
    var span = document.createElement('span');
    span.className = 'activity ' +  get_value('fields.type', info).toLowerCase() + " " + get_value('fields.msg_prefix', info).split(/\s/)[0].toLowerCase(); //TODO action+type
    var obj = get_value('fields.object_repr', info);
    var prefix = get_value('fields.msg_prefix', info);
    var suffix = get_value('fields.msg_suffix', info);
    var t = get_value('fields.type', info);
  
    if (t == 'Note') {
        var q = document.createElement('q');
        var parts = obj.split(/\s/);
        if (parts.length > 10) {
            parts = parts.slice(0,10);
            parts.push('...');
		}
		obj = parts.join(' ');
        q.appendChild(document.createTextNode(obj));
        obj = q;
    } else
    obj = document.createTextNode(obj);
  
    if (prefix)
        span.appendChild(document.createTextNode(prefix + ' '));
    if (obj)
        span.appendChild(obj);
    if (suffix && t!='Shot')
        span.appendChild(document.createTextNode(' ' + suffix));
    
    return span;
}

function resort_table(th, which, data_obj) {
    Event.observe(th, 'click', function(x) { data_obj.resort_table(which); });
}
