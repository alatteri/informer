  var Informer = {};
  Informer.Filter = Class.create();
  Informer.Filter.prototype = {
    initialize: function(field)
    {
      this.field = field
      this.value = '';
      this.use = false;
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
      this._filters = {};
    },
    
    add: function(name, filter)
    {
      this._filters[name] = filter;
      this._to_run.push(name);
    },
    
    execute: function(objects)
    {
      var tr = objects;
      for (var i=0; i<this._to_run.length; i++)
      {
        tr = this._filters[this._to_run[i]].execute(tr);
      }
      return tr;
    },
    
    turn_off: function(which)
    {
      this._filters[which].use = false;
    },
    
    set_value: function(which, value)
    {
      this._filters[which].use = true;
      this._filters[which].value = value;
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
    var f = f.split('.');
    var tmp = object
    for (var i=0; i<f.length; i++)
    {
      tmp = tmp[f[i]];
    }
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
  NOTE_FILTERS.add('status', new Informer.Filter('fields.is_checked'));
  NOTE_FILTERS.add('author', new Informer.Filter('fields.created_by.pk'));
  
  var Notes = new Informer.Data('notes_table', 
    [['fields.created_on', function(x) { return (x.getMonth()+1)+'/'+x.getDate()+'/'+x.getFullYear(); }],
      'fields.created_by.username', 
      ['fields.is_checked', function(x) { return x ? 'Completed' : 'Pending'}]],
      'fields.text', '', NOTE_FILTERS);
  Notes.pre_process = function()
  {
    var authors = [];
    var a_data = {}
    var a_filter = $('notes_author_filter');

    for (var i=0; i<this.data.length; i++)
    {
      var note = this.data[i];
      var name = note.fields.created_by.username;
      if (!a_data[name])
      {
        a_data[name] = {id: note.fields.created_by.pk, count: 0};
        authors.push(name);
      }
      a_data[name].count++;

      this.data[i].fields.created_on = parseDate(this.data[i].fields.created_on);
      this.data[i].fields.modified_on = parseDate(this.data[i].fields.modified_on);
    }

    while(a_filter.hasChildNodes())
      a_filter.removeChild(a_filter.firstChild);

    a_filter.appendChild(create_li('All', function(x) { Notes.filters.turn_off('author'); return false; }));
    authors.sort();
    authors.each(function(each){
      var txt = each + ' (' + a_data[each].count + ')';
      var func = function(x) { Notes.filters.set_value('author', a_data[each].id); return false; };
      a_filter.appendChild(create_li(txt, func));
    });
  }

function create_li(text, func)
{
  var li = document.createElement('li');
  var a = document.createElement('a');
  a.href = '#';
  a.appendChild(document.createTextNode(text));
  Event.observe(a, 'click', func);
  li.appendChild(a);
  return li;
}
