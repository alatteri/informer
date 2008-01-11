  var Informer = {};
  Informer.Filter = Class.create();
  Informer.Filter.prototype = {
    initialize: function(field)
    {
      this.field = field
      this.value = '';
      this.use = false;

			this.label_field;
			this.label_field_filter;
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
      this._to_run = [];
      this._filters = $H();

			this._list_data;
    },
    
    add: function(name, filter)
    {
      this._filters.set(name, filter);
      this._to_run.push(name);
    },
    
    execute: function(objects)
    {
      var tr = objects;
      for (var i=0; i<this._to_run.length; i++)
      {
        tr = this._filters.get(this._to_run[i]).execute(tr);
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
    initialize: function(table_name, row_1, row_2, url, filters)
    {
      this.table_name = table_name;
      this.row_1 = row_1;
      this.row_2 = row_2;
      this.table = undefined;
      this.filters = filters;
      this.data = undefined;
      this.url = url;
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
    },
    
    populate_table: function()
    {
      if (!this.data)
      {
        this.load_data();
        return;
      }

      if (!this.table)
        this.table = $(this.table_name);
      // clear rows
      var tbody = this.table.tBodies[0];
      while (tbody.hasChildNodes())
        tbody.removeChild(tbody.firstChild);
      
      var data_copy = this.data;
      if (this.filters)
        data_copy = this.filters.execute(this.data);
      
      data_copy.each(function (item) {
        var tr1 = document.createElement('TR');
        for (var i=0; i<this.row_1.length; i++)
        {
          var value = get_value(this.row_1[i], item);
          var td = document.createElement('TD');
          td.appendChild(document.createTextNode(value));
          tr1.appendChild(td);
        }
        tbody.appendChild(tr1);
      
        var tr2 = document.createElement('TR');
        var td = document.createElement('TD');
        td.colSpan = this.row_1.length;
        var value = get_value(this.row_2, item);
        td.appendChild(document.createTextNode(value));
        tr2.appendChild(td);
        tbody.appendChild(tr2);
      
      }.bind(this));
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
		}

  };
  
  function get_value(field, object)
  {
    var filter, f;
    if (field.pop)
    {
      f = field[0];
      filter = field[1];
    }
    else
    {
      f = field;
    }
		var tmp = object.eval(f);
    if (filter)
      tmp = filter(tmp);
    return tmp;
  }
  function parseDate(strDate) {
      if (strDate.getMonth)
        return strDate;
      var nums = strDate.match(/(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2})/);
      var d = new Date();
      d.setFullYear(nums[1], nums[2]-1, nums[3]);
      d.setHours(nums[4]);
      d.setMinutes(nums[5]);
      d.setSeconds(nums[6]);
      return d;
  }
  var NOTE_FILTERS = new Informer.FilterList();
	var _NF1 = new Informer.Filter('fields.is_checked');
	_NF1.label_field = 'fields.is_checked';
	_NF1.label_field_filter = function(x) { return x=='true' ? 'Completed' : 'Pending'};
  NOTE_FILTERS.add('status', _NF1);
	var _NF2 = new Informer.Filter('fields.created_by.pk');
	_NF2.label_field = 'fields.created_by.username';
  NOTE_FILTERS.add('author', _NF2);
  
  var Notes = new Informer.Data('notes_table', 
    [['fields.created_on', function(x) { return (x.getMonth()+1)+'/'+x.getDate()+'/'+x.getFullYear(); }],
      'fields.created_by.username', 
      ['fields.is_checked', function(x) { return x ? 'Completed' : 'Pending'}]],
      'fields.text', '', NOTE_FILTERS);
  Notes.pre_process = function()
  {
		this.filters.reset_list_data();

    for (var i=0; i<this.data.length; i++)
    {
		  var note = this.data[i];
			this.filters.process_list_data(note);

      this.data[i].fields.created_on = parseDate(this.data[i].fields.created_on);
      this.data[i].fields.modified_on = parseDate(this.data[i].fields.modified_on);
    }
		var ld = this.filters.get_list_data();

    create_filter_ul(ld.get('author'), Notes, 'author', $('notes_author_filter'));
		create_filter_ul(ld.get('status'), Notes, 'status', $('notes_status_filter'));
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
