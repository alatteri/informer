  var Informer = {};
  Informer.Filter = Class.create();
  Informer.Filter.prototype = {
    initialize: function(name, field, label, filter)
    {
      this.name = name
      this.field = field
      this.value = '';
      this.use = false;

			this.label_field = label;
			this.label_field_filter = filter;
    },

		run_for_list: function(obj)
		{
			return [get_value(this.field, obj), get_value(this.label_field, obj)];
		},

		get_label: function(label_value)
		{
			return this.label_field_filter ? this.label_field_filter(label_value) : label_value;
		},
    
    execute: function(objects)
    {
      if (!this.use)
      {
        return objects;
      }
      else
      {
        var tr = [];
        for (var o=0; o<objects.length; o++)
        {
          var tmp = get_value(this.field, objects[o]);
          if (tmp == this.value)
            tr.push(objects[o]);
        }
        return tr;
      }
    }
  };
  
  Informer.FilterList = Class.create();
  Informer.FilterList.prototype = {
    initialize: function()
    {
      this._filters = $H();

			this._list_data;
    },
    
    add: function(filter)
    {
      this._filters.set(filter.name, filter);
    },
    
    execute: function(objects)
    {
      var tr = objects;
      var names = this._filters.keys();
      for (var i=0; i<names.length; i++)
      {
        tr = this._filters.get(names[i]).execute(tr);
      }
      return tr;
    },
    
		reset_list_data: function()
		{
			this._list_data = $H();
			this._filters.keys().each(function(x){
				this._list_data.set(x, $H());
			}.bind(this));
		},

		process_list_data: function(obj)
		{
		  var names = this._filters.keys();
			for (var i=0; i<names.length; i++)
			{
				var f = this._filters.get(names[i]);
				var data = this._list_data.get(names[i]);
				var info = f.run_for_list(obj);
				var tmp = data.get(info[1]);
				if (tmp)
				{
					tmp.count++;
				}
				else
				{
					tmp = {'count':1, 'value':info[0]};
				}
				data.set(info[1], tmp);
				this._list_data.set(names[i], data);
			}
		},

		get_list_data: function()
		{
			var tmp = $H();
			var keys = this._list_data.keys();
			for (var i=0; i<keys.length; i++)
			{
				var key = keys[i];
				var filter = this._filters.get(key);
				var sub = this._list_data.get(key);
				var sub_keys = sub.keys();
				var tmp2 = $H();
				for (var j=0; j<sub_keys.length; j++)
				{
					var sub_key = sub_keys[j];
					var label = filter.get_label(sub_key);
					tmp2.set(label, sub.get(sub_key));
				}
				tmp.set(key, tmp2);
			}
			return tmp;
		}

  }
  
  Informer.Data = Class.create();
  Informer.Data.prototype = {
    initialize: function(name, row_1, row_2, filter_list)
    {
      this.name = name;
      this.row_1 = row_1;
      this.row_2 = row_2;
      this.entries = undefined;
      this.filters = new Informer.FilterList();;
      this.data = undefined;
      this.url = undefined;
      this._sorter = undefined;
      this._reversed = false;
      
      if (filter_list)
      {
        filter_list.each(function(x) {
          this.filters.add(x);
        }.bind(this)); 
      }
    },
    
    set_sorter: function(which)
    {
      if (this._sorter == which)
      {
        this._reversed = !this._reversed;
      }
      else
      {
        this._sorter = which;
        this._reversed = false;
      }
      this.populate_table();
    },

    load_data: function()
    {
      if (this.data) return;

      var a = new Ajax.Request(this.url, {
        method: 'GET',
        onSuccess: function(r) { 
          this.data=eval(r.responseText);
          this.pre_process();
          this.populate_table()
        }.bind(this)
      });

    },

    setup_data: function(resp)
    {
      this.data = eval(resp.responseText);
      this.pre_process(); 
      this.populate_table();
    },
    
    pre_process: function()
    {
		  this.filters.reset_list_data();
		  
		  if (!this.entries)
		  {
		    this.entries = $(this.name + "_entries");
		  }
      for (var i=0; i<this.data.length; i++)
      {
		    var obj = this.data[i];
			  this.filters.process_list_data(obj);
			  
			  for (var j=0; j<this.row_1.length; j++)
			  {
			    var col = this.row_1[j];
			    
			    if (col.parser)
			    {
			      val = get_value(col.field, obj);
			      val = col.parser(val);
			      set_value(col.field, obj, val);
			    }
			  }
      }
		  
    },
    
    populate_table: function()
    {
      if (!this.data)
      {
        this.load_data();
        return;
      }

      if (!this.entries)
        this.entries = $(this.name + '_entries');
      // clear rows
      while (this.entries.hasChildNodes())
        this.entries.removeChild(this.entries.firstChild);
      
      var data_copy = this.data.clone();
      if (this.filters)
        data_copy = this.filters.execute(this.data);
        
      // sort what remains
      var sorter = this.row_1.find(function(x) { return x.name==this._sorter; }.bind(this));
      if (sorter)
      {
        if (sorter.sorter)
        {
          data_copy.sortBy(function (x) {
            return sorter.sorter(get_value(sorter.field, x));
          });
        }
        else
        {
         data_copy.sortBy(function (x) { get_value(sorter.field, x); });
        }
      }
      
      if (this._reversed)
        data_copy.reverse();
      
      data_copy.each(function (item) {
        if (this.row_2)
        {
        this.handle_two_rows(item);
        }
        else
        {
		this.handle_single_row(item);
        }
      }.bind(this));
    },
    
    handle_two_rows: function(item)
    {
      var li = document.createElement('LI');
      var entry = document.createElement('DIV');
      entry.className = 'entry';
      var ul = document.createElement('UL');
      ul.className = 'heading';
      this.create_entry_item(item, 'LI', ul);
      entry.appendChild(ul);
      
      var content = document.createElement('DIV');
      content.className = 'entry_content';
			var val = get_value(this.row_2, item);
			if (val.tagName)
			{
			  content.appendChild(val);
		  }
			else
			{
			  var p = document.createElement('P');
        p.appendChild(document.createTextNode(val));
        content.appendChild(p);
			}
      entry.appendChild(content);
      
      li.appendChild(entry);
      this.entries.appendChild(li);
    },
    
    handle_single_row: function(item)
    {
      var li = document.createElement('LI');
      this.create_entry_item(item, 'SPAN', li);
      this.entries.appendChild(li);
    },
    
    create_entry_item: function(item, tag, parent)
    {
      for (var i=0; i<this.row_1.length; i++)
      {
        var info = this.row_1[i];
        var elem;
        if (info.create_func)
        {
          elem = info.create_func(item);
        }
        else
        {
          var value = get_value(info, item);
          elem = document.createElement(tag);
          elem.className = info.name;
          elem.appendChild(document.createTextNode(value));
        }
        parent.appendChild(elem);
      }
    },

		turn_off: function(which)
		{
			this.filters._filters.get(which).use = false;
			this.populate_table();
		},

		set_value: function(which, value)
		{
			var f = this.filters._filters.get(which);
			f.use = true;
			f.value = value;
			this.populate_table();
		},
    
		add_data: function(d)
		{
      this.data.push(d);
			this.pre_process();
			this.populate_table();
		}
  };
  
  function sort_by_date(d)
  {
    return d.getTime();
  }
  
  function sort_by_string(s)
  {
    return s.toLowerCase();
  }
  
  function get_value(field, object)
  {
    var filter, f;
    if (field.pop)
    {
      f = field[0];
      filter = field[1];
    }
    else if (field.field)
    {
      f = field.field;
      filter = field.formatter;
    }
    else
    {
      f = field;
    }
		var tmp = object;
		var fields = f.split('.');
		for (var i=0; i<fields.length; i++)
		{
			tmp = tmp[fields[i]];
		}
    if (filter)
      tmp = filter(tmp);
    return tmp;
  }
  
  function set_value(field, object, value)
  {
    var fields = field.split('.');
    fields.reverse();
    return _set_value(fields, object, value);
  }
  
  function _set_value(fields, object, value)
  {
    var f = fields.pop();
    tmp = object[f];
    
    if (!fields.length)
    {
      object[f] = value;
    }
    else
    {
      
      object[f] = _set_value(fields, tmp, value);
    }
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
  
function create_filter_ul(values, data_obj, which, ul_element)
{
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

function create_li(text, func)
{
  var li = document.createElement('li');
  li.appendChild(document.createTextNode(text));
  Event.observe(li, 'click', func);
  return li;
}

function format_date(d)
{
  return (d.getMonth()+1)+'/'+d.getDate()+'/'+d.getFullYear();
}

function format_pending(p)
{
  return p ? 'Completed' : 'Pending';
}

function format_assigned(u)
{
	if (!u || !u.pk)
		return 'Anyone';
	else
		return u.username;
}

function format_nl2br(txt)
{
  var lines = txt.strip().split('\n');
	var p = document.createElement('p');
  for (var i=0; i<lines.length; i++)
	{
    p.appendChild(document.createTextNode(lines[i]));
		if (i>0)
		  p.appendChild(document.createElement('br'));
	}
	return p;
}

function create_log(info)
{
  var span = document.createElement('span');
  span.className = 'activity'; //TODO action+type
  var obj = get_value('fields.object_repr', info);
  var prefix = get_value('fields.msg_prefix', info);
  var suffix = get_value('fields.msg_suffix', info);
  var t = get_value('fields.type', info);
  
  if (t == 'Note')
  {
    var q = document.createElement('q');
		var parts = obj.split(/\s/);
		if (parts.length > 10)
		{
      parts = parts.slice(0,10);
			parts.push('...');
		}
		obj = parts.join(' ');
    q.appendChild(document.createTextNode(obj));
    obj = q;
  }
  else
  {
    obj = document.createTextNode(obj);
  }
  
  if (prefix)
    span.appendChild(document.createTextNode(prefix + ' '));
  if (obj)
    span.appendChild(obj);
  if (suffix && t!='Shot')
    span.appendChild(document.createTextNode(' ' + suffix));
    
  return span;
}

function set_sorter(th, which, data_obj)
{
  Event.observe(th, 'click', function(x) { data_obj.set_sorter(which); });
}
  
  var Notes = new Informer.Data(
    'notes', // name
    [ // row_1
      {name: 'date',
       field: 'fields.created_on',
       sorter: sort_by_date,
       formatter: format_date,
       parser: parse_date,
       is_default: true},
      {name: 'author',
       field:'fields.created_by.username',
       sorter: sort_by_string},
       {name: 'assignedTo',
        field:'fields.assigned_to',
        formatter: format_assigned},
       {name: 'status',
        field: 'fields.is_checked',
       formatter: format_pending}
    ],
    { //row_2
     field: 'fields.text',
		 formatter: format_nl2br
    }, 
    [ // filter_list
      new Informer.Filter('status', 'fields.is_checked', 'fields.is_checked',
                          format_pending),
      new Informer.Filter('author', 'fields.created_by.pk', 
			                    'fields.created_by.username')
    ]);
    
  var Logs = new Informer.Data(
    'overview', // name
    [ // row_1
      {name: 'date',
       field: 'fields.when',
       sorter: sort_by_date,
       formatter: format_date,
       parser: parse_date,
       is_default: true},
      {name: 'author',
       field:'fields.who.username',
       sorter: sort_by_string},
      {name: 'activity',
       field: 'fields.msg_prefix',
       create_func: create_log,
       sorter: sort_by_string}
    ],
    null, //row_2
    [ // filter_list
      new Informer.Filter('date', 'fields.when', 'fields.when', format_date),
      new Informer.Filter('author', 'fields.who.pk', 'fields.who.username')
    ]);
