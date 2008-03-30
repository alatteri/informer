function LOG(x) {
    if (1) {
        return;
    } else if (window.console) {
        window.console.log(x);
    } else {
        alert(x);
    }
}

/*
 * GLOBALS
 * ------------------------------------------------------- */
// the id of the currently selected render
var CURRENT_RENDER;

/* 
 * Informer Object
 * ------------------------------------------------------- */
var Informer = {};

/* 
 * Informer Column: represents a table column
 * ------------------------------------------------------- */
Informer.BaseColumn = Class.create({
    initialize: function(name, field) {
        this.name = name;
        this.field = field;
        this.is_default = false;

        this.parser    = function (x) { return x };
        this.sorter    = function (x) { return x };
        this.formatter = function (x) { return x };
        LOG("BaseColumn initialize called: " + this.name);
    },
});

Informer.DateColumn = Class.create(Informer.BaseColumn, {
    initialize: function($super, name, field) {
        $super(name, field);
        LOG("DateColumn initialize called");
        this.sorter = sort_by_date;
        this.formatter = format_date;
        this.parser = parse_date;
    },
});

Informer.DateTimeColumn = Class.create(Informer.BaseColumn, {
    initialize: function($super, name, field) {
        $super(name, field);
        LOG("DetailedDateColumn initialize called");
        this.sorter = sort_by_date;
        this.formatter = format_datetime;
        this.parser = parse_date;
    },
});

Informer.UserColumn = Class.create(Informer.BaseColumn, {
    initialize: function($super, name, field) {
        $super(name, field);
        LOG("UserColumn initialize called");
        this.sorter = sort_by_string;
        this.formatter = format_author;
    },
});

Informer.AssignedUserColumn = Class.create(Informer.BaseColumn, {
    initialize: function($super, name, field) {
        $super(name, field);
        LOG("AssignedUserColumn initialize called");
        this.sorter = sort_by_assigned;
        this.formatter = format_assigned;
    },
});

Informer.NoteStatusColumn = Class.create(Informer.BaseColumn, {
    initialize: function($super, name, field) {
        $super(name, field);
        LOG("NoteStatusColumn initialize called");
        this.sorter = sort_by_string;
        this.formatter = format_pending;
        this.parser = parse_boolean;
    },
});

Informer.ParagraphColumn = Class.create(Informer.BaseColumn, {
    initialize: function($super, name, field) {
        $super(name, field);
        LOG("ParagraphColumn initialize called");
        this.sorter = sort_by_string;
        this.formatter = format_nl2br;
    },
});

Informer.TextColumn = Class.create(Informer.BaseColumn, {
    initialize: function($super, name, field) {
        $super(name, field);
        LOG("TextColumn initialize called");
        this.sorter = sort_by_string;
        this.formatter = format_string;
    },
});

Informer.ActivityColumn = Class.create(Informer.BaseColumn, {
    initialize: function($super, name, field) {
        $super(name, field);
        LOG("ActivityColumn initialize called");
        this.sorter = sort_by_string;
        this.formatter = format_string;
        this.create_func = create_log;
    },
});

/* 
 * Informer Filter: represents one filtered category
 * ------------------------------------------------------- */
Informer.Filter = Class.create();
Informer.Filter.prototype = {
    // initialize: create a new Filter object
    //      col: Informer column object
    initialize: function(col) {
        this.name = 'filter_' + col.name;
        this.field = col.field;

        // the format function is optional
        this.format = col.formatter;

        this.value = null;
        this.matches = $H();
        this.on_click = null;
    },

    // specify the value the object will filter on
    // pass null to turn off filter
    filter_by: function(value) {
        this.value = value;
    },

    // returns true if the data would be filtered
    is_filtered: function(data) {
        if (null == this.value) return false;

        // the data is filtered if the value doesn't
        // match the filter's value
        var val = this.format(getattr(data, this.field));
        return val != this.value;
    },

    // clears the match count for the Filter object
    reset_filter: function() {
        this.matches = $H();
    },

    // accepts a data object, increments internal counts for fields
    observe_data: function(data) {
        var val = this.format(getattr(data, this.field));
        if (val == null) val = 'None'

        var count = this.matches.get(val);
        if (count) {
            this.matches.set(val, count + 1);
        } else {
            this.matches.set(val, 1);
        }
    },

    // create a list item entry: accepts text label to display and value
    // to use in the on_click handler
    create_li: function(text, value) {
        var li = document.createElement('li');

        // if the value is the highlighted value set the class
        if (value == this.value)
            li.className = 'highlight';

        // I'm eval'ing fn inside of the callback. It seems gross
        // it'll work until there's something better. Since it's an
        // eval make sure to not quote null or it becomes string 'null'
        if (value != null)
            value = "'" + value + "'";

        var a = document.createElement('a');
        a.appendChild(document.createTextNode(text));
        var fn = this.on_click + "('" + this.name + "'," + value + ")";
        a.href = 'javascript:' + fn;
        li.appendChild(a);

        return li;
    },

    // render the Filter object to the screen
    draw: function() {
        var ul = $(this.name);
        while (ul.hasChildNodes())
            ul.removeChild(ul.firstChild);

        var keys = this.matches.keys().sort();
        if (keys.length) {
            // display the show all link first
            ul.appendChild(this.create_li('Show All', null));

            // followed by the counts for all rows
            for (var i=0; i<keys.length; i++) {
                var key = keys[i];
                var count = this.matches.get(key);

                // Provide defaults to make sure something is displayed
                // XXX is this always right? -- NOPE. Too late at this point. TODO
                if (key == '') key = 'None';
                if (text == '') text = 'None';

                var text = key + ' (' + count + ')';
                ul.appendChild(this.create_li(text, key));
            }
        } else {
            ul.appendChild(this.create_li('None', null));
        }
    },
};


/* 
 * Informer Filter Manager: manages several Filter objects
 * ------------------------------------------------------- */
Informer.FilterManger = Class.create();
Informer.FilterManger.prototype = {
    // initialize: create a new Filter Manager object
    initialize: function(on_click) {
        this.filters = $H();
        this.on_click = on_click;
    },
  
    // associate a Filter object with the Manger
    register_filter: function(filter) {
        filter.on_click = this.on_click;
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

    // returns true if any of the filters would filter the data
    is_filtered: function(data) {
        var names = this.filters.keys();
        for (var i=0; i<names.length; i++) {
            if (this.filters.get(names[i]).is_filtered(data)) {
                return true;
            }
        }
        return false;
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

        // the Filter Manager accepts the name of a function
        // for callback when a filter is clicked
        var on_click = this.name + '.filter';
        this.fm = new Informer.FilterManger(on_click);

        // default to sorting by the first column in the table
        if (this.row_1)
            this._sorter = this.row_1[0]['name'];

        this.data = undefined;
        this._reversed = false;
        this.generate_row = function() { return null };
      
        if (filter_list) {
            filter_list.each(function(x) {
                this.fm.register_filter(x);
            }.bind(this)); 
        }
    },

    /* Preprocesses data and populates table */
    load_data: function(data) {
        if ('String' == typeof(data)) {
            this.data=eval(data);
        } else {
            this.data=data;
        }
        
        this.pre_process();
        this.draw();
    },

    /* Preprocesses data */
    pre_process: function() {
        this.fm.reset_all_filters();
		  
        for (var i=0; i<this.data.length; i++) {
            var obj = this.data[i];
            this.fm.register_data(obj);
			  
            if (this.row_1) {
                for (var j=0; j<this.row_1.length; j++) {
                    var col = this.row_1[j];
                    val = getattr(obj, col.field);
                    if (col.parser) {
                        // use the column's parser if specified
                        val = col.parser(val);
                    }

                    setattr(obj, col.field, val);
                }
            }
        }  
    },

    /* Resorts data given passed sorter */
    resort_table: function(which) {
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

        this.draw();
    },

    /* Sorts the table in decending order by column specified */
    sort_decending: function(which) {
        this._sorter = which;
        this._reversed = true;
        this.sort_data();
        this.reverse_table();
        this.draw();
    },
    
	/* Renders the table item */
    draw: function() {
        // Clear out previous entries (if any)
        while (this.entries.hasChildNodes())
            this.entries.removeChild(this.entries.firstChild);	

        var number_drawn = 0;

        // Draw the non-filtered data
        for (var i=0; i<this.data.length; i++) {
            if (false == this.fm.is_filtered(this.data[i])) {
                number_drawn += 1;
                var new_entry = this.generate_row(this.data[i]);
                if (new_entry) this.entries.appendChild(new_entry);
            }
        }

        if (0 == number_drawn) {
            var msg = document.createElement('P');
            msg.style.textAlign = 'center';
            msg.style.fontSize = '12px';
            msg.style.margin = '10px 0 0 0';
            var text = 'No ' + this.name + ' to display';
            msg.appendChild(document.createTextNode(text));
            this.entries.appendChild(msg);
        }

        // TODO: It would be nice to make this into an object method
        if (this.row_1) highlightColumn(this._sorter);

        // Finally, draw the filters
        this.fm.draw();
		addIEOverviewHoverStyles();
		addLogItemLinks();
    },

    /* Sorts data given current sorter */
    sort_data: function() {
        // search through all columns, find the sorter of the currently
        // highlighted column
        var column = this.row_1.find(function(x) { 
            return x.name == this._sorter; 
        }.bind(this));

        // make sure we found the column and it had a sorter defined
        if (column && column.sorter) {
            this.data = this.data.sortBy(function (x) {
                return column.sorter(getattr(x, column.field));
            });
        }
    },
	
	/* Reverses given data and flips reversed flag */
    reverse_table: function() {
        this.data.reverse();
    },
    
    /* Callback triggered when user clicks on a "filter by" link */
    filter: function(filter_name, filter_val) {
        var f = this.fm.filters.get(filter_name);
        f.filter_by(filter_val);
        this.draw();
    },

    /* Adds given data object, preprocesses it, and repopulates table */     
    add_data: function(d) {
        this.data.push(d);
        this.pre_process();
    },

    /* Updates the data with a particular pk with new entry */
    update_data: function(d) {
        var pk = getattr(d, 'pk');
        var done = false;
        for (var i=0; i<this.data.length && !done; i++) {
            if (pk == getattr(this.data[i], 'pk')) {
                this.data[i] = d;
                done = true;
            }
        }
        this.pre_process();
    },

    /* Deletes the row associated with the data element */
    delete_data: function(d) {
        var pk = getattr(d, 'pk');
        var new_data = [];
        for (var i=0; i<this.data.length; i++) {
            if (pk != getattr(this.data[i], 'pk')) {
                new_data.push(this.data[i]);
            }
        }
        this.data = new_data;
        this.pre_process();
    },
};
  
/* 
 * Helper Functions
 * ------------------------------------------------------- */
function sort_table(table, filter_by) {
    $('sort_by_' + table._sorter).className = null;
    table.resort_table(filter_by);
    $('sort_by_' + table._sorter).className = 'highlight';
}

function sort_by_date(d) {
    if (d != null) {
        return d.getTime();
    } else {
        // 0 sorts to the top
        return 9999999999999;
    }
}

function sort_by_string(s) {
    return (s+"").toLowerCase();
}

function sort_by_assigned(u) {
    return sort_by_string(format_assigned(u));
}
  
function getattr(object, field) {
    var parser, formatter, f;
    if (field.pop) {
        f = field[0];
        formatter = field[1];
    } else if (field.field) {
        f = field.field;
        formatter = field.formatter;
        parser = field.parser;
    } else {
        f = field;
    }
    var tmp = object;
    var fields = f.split('.');
    for (var i=0; i<fields.length; i++) {
        tmp = tmp[fields[i]];
    }

    if (parser) tmp = parser(tmp);
    if (formatter) tmp = formatter(tmp);

    return tmp;
}
  
function setattr(object, field, value) {
    var fields = field.split('.');
    fields.reverse();
    return _setattr(object, fields, value);
}
  
function _setattr(object, fields, value) {
    var f = fields.pop();
    tmp = object[f];
   
    if (!fields.length)
        object[f] = value;
    else
        object[f] = _setattr(tmp, fields, value);
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
    if (d != null) {
        month = (d.getMonth()+1) < 10?'0'+(d.getMonth()+1):(d.getMonth()+1);
        day = d.getDate() < 10?'0'+d.getDate():d.getDate();
        return month+'/'+day+'/'+d.getFullYear();
    } else {
        return 'None';
    }
}

/* accepts a Date object, returns stringified Date with time */
function format_datetime(d) {
    if (d != null && d.getMonth) {
        month = (d.getMonth()+1).toPaddedString(2);
        day = d.getDate().toPaddedString(2)
        year = d.getYear();
        if (year > 100) year -= 100;
        if (year < 10) year = '0' + year;

        ampm = 'am';
        hours = d.getHours();
        if (hours == 0) {
            hours = 12;
        } else if (hours > 12) {
            ampm = 'pm';
            hours -= 12;
        }
        mins = d.getMinutes().toPaddedString(2);
        return month + '/' + day + '/' + year + ' at ' + hours + ':' + mins + ' ' + ampm;
    } else {
        return 'None';
    }
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
     return u ? u.substring(0,1).toUpperCase() + u.substring(1,u.length) : 'None';
}

/* accepts string, returns None if null */
function format_string(s) {
    return s == null || s == '' ? 'None' : s;
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

function resort_table(th, which, data_obj) {
    Event.observe(th, 'click', function(x) { data_obj.resort_table(which); });
}
