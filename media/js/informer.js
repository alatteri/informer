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
Informer.Filter = Class.create();
Informer.Filter.prototype = {
    initialize: function(name, field, label, filter) {
        this.name = name
        this.field = field
        this.value = '';
        this.use = false;

        this.label_field = label;
        this.label_field_filter = filter;
    },

    run_for_list: function(obj) {
        return [get_value(this.field, obj), get_value(this.label_field, obj)];
    },

    get_label: function(label_value) {
        return this.label_field_filter ? this.label_field_filter(label_value) : label_value;
    },
  
    execute: function(objects) {
        if (!this.use)
            return objects;
        else {
            var tr = [];
            for (var o=0; o<objects.length; o++) {
                var tmp = get_value(this.field, objects[o]);
                if (tmp == this.value)
                    tr.push(objects[o]);
            }
            return tr;
        }
    }
};
  

/* 
 * Informer Filter List
 * ------------------------------------------------------- */

Informer.FilterList = Class.create();
Informer.FilterList.prototype = {
    initialize: function() {
        this._filters = $H();
        this._list_data;
    },
  
    add: function(filter) {
        this._filters.set(filter.name, filter);
    },
  
    execute: function(objects) {
        var tr = objects;
        var names = this._filters.keys();
        for (var i=0; i<names.length; i++)
            tr = this._filters.get(names[i]).execute(tr);
        return tr;
    },
  
    reset_list_data: function() {
        this._list_data = $H();
        this._filters.keys().each(function(x){
            this._list_data.set(x, $H());
        }.bind(this));
    },

    process_list_data: function(obj) {
        var names = this._filters.keys();
        for (var i=0; i<names.length; i++) {
            var f = this._filters.get(names[i]);
            var data = this._list_data.get(names[i]);
            var info = f.run_for_list(obj);
            var tmp = data.get(info[1]);
            
            if (tmp)
                tmp.count++;
            else
                tmp = {'count':1, 'value':info[0]};
            
            data.set(info[1], tmp);
            this._list_data.set(names[i], data);
        }
    },

    get_list_data: function() {
        var tmp = $H();
        var keys = this._list_data.keys();
        for (var i=0; i<keys.length; i++) {
            var key = keys[i];
            var filter = this._filters.get(key);
            var sub = this._list_data.get(key);
            var sub_keys = sub.keys();
            var tmp2 = $H();
            for (var j=0; j<sub_keys.length; j++) {
                var sub_key = sub_keys[j];
                var label = filter.get_label(sub_key);
                tmp2.set(label, sub.get(sub_key));
            }
            tmp.set(key, tmp2);
        }
        return tmp;
    }
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
        this.entries = undefined;
        this.filters = new Informer.FilterList();;
        this.data = undefined;
        this.url = undefined;
        this._sorter = undefined;
        this._reversed = false;
      
        if (filter_list) {
            filter_list.each(function(x) {
                this.filters.add(x);
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
        this.filters.reset_list_data();
		  
        if (!this.entries)
            this.entries = $(this.name + "_entries");
        
        for (var i=0; i<this.data.length; i++) {
            var obj = this.data[i];
            this.filters.process_list_data(obj);
			  
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
        if (!this.entries)
            this.entries = $(this.name + '_entries');

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
                elem = info.create_func(info, item);
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
        this.filters._filters.get(which).use = false;
        this.populate_table();
    },

    /* Sets the filter value and repopulates the table */
    set_value: function(which, value) {
        var f = this.filters._filters.get(which);
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
  
function create_filter_ul(values, data_obj, which, ul_element) {
    while (ul_element.hasChildNodes())
        ul_element.removeChild(ul_element.firstChild);

	var names = values.keys();
	names.sort();
	ul_element.appendChild(create_li(
		'All', function(x) { data_obj.turn_off(which); return false; }));
	names.each(function(each) {
		var txt = each + ' (' + values.get(each).count + ')';
		var val = values.get(each).value;
		var func = function(x) { data_obj.set_value(which, val); return false; };
		ul_element.appendChild(create_li(txt, func));
	});
}

function create_li(text, func) {
    var li = document.createElement('li');
    li.appendChild(document.createTextNode(text));
    Event.observe(li, 'click', func);
    return li;
}

function format_date(d) {	
    month = (d.getMonth()+1) < 10?'0'+(d.getMonth()+1):(d.getMonth()+1);
    day = d.getDate() < 10?'0'+d.getDate():d.getDate();
    return month+'/'+day+'/'+d.getFullYear();
}

function format_pending(p) {
    return p ? 'Completed' : 'Pending';
}

function format_assigned(u) {
    return u ? format_author(u) : 'Anyone';
}

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
