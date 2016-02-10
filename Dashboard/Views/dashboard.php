<?php

/*

Heatpump Monitor web dashboard code is released under the: 
GNU Affero General Public License and is based on emoncms.
Part of the OpenEnergyMonitor project:
http://openenergymonitor.org

*/

?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
  <head>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
  
  <script type="text/javascript" src="jquery-1.9.1.min.js"></script>
  <!--[if lte IE 8]><script language="javascript" type="text/javascript" src="flot/excanvas.min.js"></script><![endif]-->
  <script language="javascript" type="text/javascript" src="flot/jquery.js"></script>
  <script language="javascript" type="text/javascript" src="flot/jquery.flot.js"></script>
  <script language="javascript" type="text/javascript" src="flot/jquery.flot.time.min.js"></script>
  <script language="javascript" type="text/javascript" src="flot/jquery.flot.selection.min.js"></script>
  <script language="javascript" type="text/javascript" src="vis.helper.js"></script>

  <link rel="stylesheet" type="text/css" href="style.css" />


  <title>Heatpump Dashboard</title>

  </head>
  <body>
    <div id="bound">
    
      <div id="header">
        <div id="logout-bound" style="float:right; margin:5px; display:none">
            <span id="username-info"></span> 
            <button id="logout">Logout</button>
        </div>
        <div id="title">Heatpump Dashboard</div>
        <div id="maintext">
        </div>
      </div>
      
<div id="box">
  <div id="maintext" style="font-size:16px">

    <h2>System info</h2>
    <table style="width:100%">
    
    
    
    <tr>
    
    <td style="width:33%;" valign="top">
    <b>Heat pump type:</b><br>Air-source<br><br>
    <b>Heat pump model:</b><br>Mitsubushi EcoDan 5kW<br><br>
    <b>Location:</b><br>Snowdonia, North Wales, UK<br><br>
    </td>
    
    <td style="width:33%;" valign="top">
    <div style="width:200px">
    <b>REALTIME</b><br>
    <div>Power: <span style="float:right"><span id="heatpump_elec"></span>W</span></div>
    <div>Heat: <span style="float:right"><span id="heatpump_heat"></span>W</span></div>
    <div>Flow: <span style="float:right"><span id="heatpump_flowT"></span>&deg;C</span></div>
    <div>Return: <span style="float:right"><span id="heatpump_returnT"></span>&deg;C</span></div>
    <div>Delta T: <span style="float:right"><span id="heatpump_deltaT"></span>K</span></div>
    <div>Flowrate: <span style="float:right"><span id="heatpump_flowrate"></span>L/h</span></div>
    <div>Outside: <span style="float:right"><span id="outside"></span>&deg;C</span></div>
    </div>
    </td><td style="width:33%;" valign="top">
    <div style="width:200px">
    <b>COP</b><br>
    <div>30 mins: <span style="float:right"><span id="COP_30m"></span></span></div>
    <div>60 mins: <span style="float:right"><span id="COP_60m"></span></span></div>
    <div>120 mins: <span style="float:right"><span id="COP_120m"></span></span></div>
    <br>
    <b>Totals</b><br>
    <div>Electricity in: <span style="float:right"><span id="total_elec"></span> kWh</span></div>
    <div>Heat output: <span style="float:right"><span id="total_heat"></span> kWh</span></div>
    <div>All-time COP: <span style="float:right"><span id="total_cop"></span></span></div>
    </div>
    </td>
    
    </tr>
    </table>
  </div>
</div>

<div id="box">
  <div id="maintext">
  
    <h2>Heating period explorer</h2>
    <p>Select a heating period to see the measured vs theoretical carnot COP for that heating period. The theoretical carnot COP is dependent on the condencing and evaporator temperature of the heat pump, for an air source heatpump the condencing temperature is about 4K higher than the flow temperature and the evaporator 6K less than the outside temperature. In practice a heat pump usually achieves around half the ideal carnot COP.</p>
    
    <div style="height:35px;">
    <div class='btn-group' style="float:left; padding-right:10px">
        <button class='btn time visnav' type='button' time='1'>D</button>
        <button class='btn time' type='button' time='7'>W</button>
        <button class='btn time' type='button' time='30'>M</button>
        <button class='btn time' type='button' time='365'>Y</button>
    </div>
    <div class='btn-group' style="float:left">
        <button id='zoomin' class='btn' >+</button>
        <button id='zoomout' class='btn' >-</button>
        <button id='left' class='btn' ><</button>
        <button id='right' class='btn' >></button>
    </div>
    <div style="clear:both"></div>
    </div>
    
    <div id="placeholder_bound">
          <div id="placeholder"></div>
    </div>
  </div>
</div>

<div id="box">
  <div id="maintext">
    <br>
    <table style="width:100%">
        <tr>
        <th style='text-align:left'>Name</th>
        <th style='text-align:center'>Min</th>
        <th style='text-align:center'>Max</th>
        <th style='text-align:center'>Diff</th>
        <th style='text-align:center'>Mean</th>
        <th style='text-align:center'>Stdev</th>
        </tr>
        <tbody id="stats"></tbody>
    </table>
    <br>
    <table style="width:100%">
      <tr>
        <th style='text-align:center'>Measured COP</th>
        <th style='text-align:center'>Theoretical Carnot COP</th>
        <th style='text-align:center'>Ratio (0.4-0.5 is good)</th>
      </tr>
      <tr>
        <td style='text-align:center'><span id="COP"></span></td>
        <td style='text-align:center'><span id="carnot"></span></td>
        <td style='text-align:center'><span id="ratio"></span></td>
      </tr>
    </table>
    <br>
    <p><i>Carnot COP = (Condencing + 273) / ((Condencing+273) - (Evaporator+273))</i>
  </div>
</div>

<div id="box">
  <div id="maintext">
  
    <h2>Daily energy input/output and COP</h2>
  
    <div id="placeholder_bound_history">
          <div id="placeholder_history"></div>
    </div>
    
  </div>
</div>

<div id="box">
  <div id="maintext">
  

  </div>
</div>

    </div>
  </body>
</html>

<script>

var path = "SITE DOMAIN";
var session = JSON.parse('<?php echo json_encode($session); ?>');
var apikey = session.apikey_read;

if (session.valid) {
  $("#logout-bound").show();
  $("#username-info").html(session.username);
}

$('#placeholder').width($('#placeholder_bound').width());
$('#placeholder_bound').height($('#placeholder_bound').width()*0.6);
$('#placeholder').height($('#placeholder_bound').width()*0.6);

$('#placeholder_history').width($('#placeholder_bound_history').width());
$('#placeholder_bound_history').height($('#placeholder_bound_history').width()*0.6);
$('#placeholder_history').height($('#placeholder_bound_history').width()*0.6);

var feeds = {};
fast_update();
slow_update();
setInterval(fast_update,5000);
setInterval(slow_update,30000);



function fast_update() 
{
    feeds = getlist();
    
    if (feeds["heatpump_elec"]!=undefined) 
        $("#heatpump_elec").html(feeds["heatpump_elec"].value);
        
    if (feeds["heatpump_heat"]!=undefined) 
        $("#heatpump_heat").html(feeds["heatpump_heat"].value);
        
    if (feeds["heatpump_flowT"]!=undefined) 
        $("#heatpump_flowT").html(feeds["heatpump_flowT"].value.toFixed(1));
        
    if (feeds["heatpump_returnT"]!=undefined) 
        $("#heatpump_returnT").html(feeds["heatpump_returnT"].value.toFixed(1));
        
    if (feeds["heatpump_deltaT"]!=undefined) 
        $("#heatpump_deltaT").html(feeds["heatpump_deltaT"].value.toFixed(1));
    
    if (feeds["heatpump_flowrate"]!=undefined) 
        $("#heatpump_flowrate").html(feeds["heatpump_flowrate"].value);
        
    if (feeds["outside"]!=undefined) 
        $("#outside").html(feeds["outside"].value.toFixed(1));
    
    if (feeds["heatpump_elec_kwh"]!=undefined) 
        $("#total_elec").html(Math.round(feeds["heatpump_elec_kwh"].value));
        
    if (feeds["heatpump_heat_kwh"]!=undefined) 
        $("#total_heat").html(Math.round(feeds["heatpump_heat_kwh"].value));
    
    if (feeds["heatpump_heat_kwh"]!=undefined && feeds["heatpump_elec_kwh"]!=undefined) {
        $("#total_cop").html((feeds["heatpump_heat_kwh"].value/feeds["heatpump_elec_kwh"].value).toFixed(2));
    }
}

function slow_update() 
{
    var now = (new Date()).getTime();
    if (feeds["heatpump_heat_kwh"]!=undefined && feeds["heatpump_elec_kwh"]!=undefined) {
    
        var heatpump_E_now = feeds["heatpump_elec_kwh"].value;
        var heatpump_H_now = feeds["heatpump_heat_kwh"].value;
        
        // LAST 30 mins
        var heatpump_E_start = getvalue(feeds["heatpump_elec_kwh"].id, now-(3600*1000*0.5))[1];
        var heatpump_H_start = getvalue(feeds["heatpump_heat_kwh"].id, now-(3600*1000*0.5))[1];
        var COP = (heatpump_H_now - heatpump_H_start) / (heatpump_E_now - heatpump_E_start);
        $("#COP_30m").html(COP.toFixed(2));
        
        // LAST 60 mins
        var heatpump_E_start = getvalue(feeds["heatpump_elec_kwh"].id, now-(3600*1000*1.0))[1];
        var heatpump_H_start = getvalue(feeds["heatpump_heat_kwh"].id, now-(3600*1000*1.0))[1];
        var COP = (heatpump_H_now - heatpump_H_start) / (heatpump_E_now - heatpump_E_start);
        $("#COP_60m").html(COP.toFixed(2));
        
        // LAST 120 mins
        var heatpump_E_start = getvalue(feeds["heatpump_elec_kwh"].id, now-(3600*1000*2.0))[1];
        var heatpump_H_start = getvalue(feeds["heatpump_heat_kwh"].id, now-(3600*1000*2.0))[1];
        var COP = (heatpump_H_now - heatpump_H_start) / (heatpump_E_now - heatpump_E_start);
        $("#COP_120m").html(COP.toFixed(2));
        
    }
}

function getlist()
{
    var list = {};
    $.ajax({                                      
        url: path+"feed/list.json?apikey="+apikey,
        async: false, dataType: "json",
        success: function(data_in) {
            for (z in data_in) {
                var name = data_in[z].name;
                list[name] = data_in[z];
                list[name].value = list[name].value * 1.0;
            }
        }
    });
    return list;
}

function getvalue(feedid,time)
{
    var result = getdata(feedid,time,time+1000,1);
    if (result.length>0) return result[0];
    return false;
}

function getdata(feedid,start,end,interval,skipmissing,limitinterval)
{
    var data = [];
    $.ajax({                                      
        url: path+"feed/data.json",                         
        data: "id="+feedid+"&start="+start+"&end="+end+"&interval="+interval+"&skipmissing="+skipmissing+"&limitinterval="+limitinterval+"&apikey="+apikey,
        dataType: 'json',
        async: false,                      
        success: function(data_in) { data = data_in; } 
    });
    return data;
}

$("#zoomout").click(function () {view.zoomout(); draw();});
$("#zoomin").click(function () {view.zoomin(); draw();});
$('#right').click(function () {view.panright(); draw();});
$('#left').click(function () {view.panleft(); draw();});
$('.time').click(function () {view.timewindow($(this).attr("time")); draw();});

$('#placeholder').bind("plotselected", function (event, ranges) {
    view.start = ranges.xaxis.from;
    view.end = ranges.xaxis.to;
    draw();
});

var timeWindow = 3600*24;
var timenow = (new Date()).getTime();
view.end = timenow;
view.start = view.end - (timeWindow*1000);
var interval = parseInt(timeWindow / 800);

var options = {
    series: { lines: { show: true } },
    xaxis: { min: view.start, max: view.end, mode: "time", timezone: "browser" },
    selection: { mode: "x" }
};

draw();
    
function draw() 
{
    options.xaxis.min = view.start;
    options.xaxis.max = view.end;
    timeWindow = (view.end - view.start)*0.001;
    interval = parseInt(timeWindow / 800);
    
    var data = [];
    var series = [];
    
    data = getdata(feeds["heatpump_elec"].id,view.start,view.end,interval,1,1);
    feeds["heatpump_elec"].stats = stats(data);
    series.push({label:"heatpump_elec", data:data,yaxis:1});

    data = getdata(feeds["heatpump_heat"].id,view.start,view.end,interval,1,1);
    feeds["heatpump_heat"].stats = stats(data);
    series.push({label:"heatpump_heat", data:data,yaxis:1});
    
    data = getdata(feeds["heatpump_flowT"].id,view.start,view.end,interval,1,1);
    feeds["heatpump_flowT"].stats = stats(data);
    series.push({label:"heatpump_flowT", data:data,yaxis:2});
    
    data = getdata(feeds["heatpump_returnT"].id,view.start,view.end,interval,1,1);
    feeds["heatpump_returnT"].stats = stats(data);
    series.push({label:"heatpump_returnT", data:data,yaxis:2});

    if (feeds["outside"]!=undefined) {
        data = getdata(feeds["outside"].id,view.start,view.end,interval,1,1);
        feeds["outside"].stats = stats(data);
        series.push({label:"outside", data:data,yaxis:2});
    }
    
    $.plot("#placeholder",series, options);
    
    var out = "";
    for (z in feeds) {
        if (feeds[z].stats!=undefined) {
            out += "<tr>";
            out += "<td style='text-align:left'>"+z+"</td>";
            out += "<td style='text-align:center'>"+feeds[z].stats.minval.toFixed(2)+"</td>";
            out += "<td style='text-align:center'>"+feeds[z].stats.maxval.toFixed(2)+"</td>";
            out += "<td style='text-align:center'>"+feeds[z].stats.diff.toFixed(2)+"</td>";
            out += "<td style='text-align:center'>"+feeds[z].stats.mean.toFixed(2)+"</td>";
            out += "<td style='text-align:center'>"+feeds[z].stats.stdev.toFixed(2)+"</td>";
            out += "</tr>";
        }
    }
    $("#stats").html(out);
    
    var COP = feeds["heatpump_heat"].stats.mean / feeds["heatpump_elec"].stats.mean;

    $("#heat").html(Math.round(feeds["heatpump_heat"].stats.mean));
    $("#elec").html(Math.round(feeds["heatpump_elec"].stats.mean));
    $("#COP").html(COP.toFixed(2));
    $("#flow").html(feeds["heatpump_flowT"].stats.mean.toFixed(2));
    
    if (feeds["heatpump_flowT"]!=undefined && feeds["outside"]!=undefined) {
        var Th = feeds["heatpump_flowT"].stats.mean + 4
        var Tc = feeds["outside"].stats.mean - 6;
        var Carnot = (Th + 273) / ((Th+273) - (Tc+273));
        var ratio = COP / Carnot;
        $("#outside").html(feeds["outside"].stats.mean.toFixed(2));
        $("#carnot").html(Carnot.toFixed(2));
        $("#ratio").html(ratio.toFixed(2));
    }
}

function stats(data)
{
    var sum = 0;
    var i=0;
    var minval = 0;
    var maxval = 0;
    for (z in data)
    {
        var val = data[z][1];
        if (val!=null) 
        {
            if (i==0) {
                maxval = val;
                minval = val;
            }
            if (val>maxval) maxval = val;
            if (val<minval) minval = val;
            sum += val;
            i++;
        }
    }
    var mean = sum / i;
    sum = 0, i=0;
    for (z in data)
    {
        sum += (data[z][1] - mean) * (data[z][1] - mean);
        i++;
    }
    var stdev = Math.sqrt(sum / i);
    
    return {
        "minval":minval,
        "maxval":maxval,
        "diff":maxval-minval,
        "mean":mean,
        "stdev":stdev
    }
}

// ---------------------------------------------
// Bargraph
// ---------------------------------------------

var d = new Date()
var n = d.getTimezoneOffset();
var offset = n / -60;

var viewend = d.getTime();
var viewstart = viewend - (3600*24*1000*30);

var interval = 3600*24;
var intervalms = interval*1000;

var datastart = Math.floor(viewstart / intervalms) * intervalms;
var dataend = Math.ceil(viewend / intervalms) * intervalms;
datastart -= offset * 3600000;
dataend -= offset * 3600000;

var scale = 1;
var series = [];            

// 1. Heat
var heat_data = getdata(feeds["heatpump_heat_kwh"].id,datastart,dataend,interval,0,1);
for (z in heat_data) {
  if (heat_data[z][0]==dataend && heat_data[z][1]==null) heat_data[z][1] = feeds["heatpump_heat_kwh"].value;
}
var heat_out = [];
for (var z=1; z<heat_data.length; z++) {
    if (heat_data[z][1]!=null && heat_data[z-1][1]!=null) {
        var val = (heat_data[z][1] - heat_data[z-1][1]) * scale;
        heat_out.push([heat_data[z-1][0],val]);
    }
}
var heatkwh = heat_out;
// 2. Elec
var elec_data = getdata(feeds["heatpump_elec_kwh"].id,datastart,dataend,interval,0,1);
for (z in elec_data) {
  if (elec_data[z][0]==dataend && elec_data[z][1]==null) elec_data[z][1] = feeds["heatpump_elec_kwh"].value;
}
var elec_out = [];
for (var z=1; z<elec_data.length; z++) {
    if (elec_data[z][1]!=null && elec_data[z-1][1]!=null) {
        var val = (elec_data[z][1] - elec_data[z-1][1]) * scale;
        elec_out.push([elec_data[z-1][0],val]);
    }
}
var eleckwh = elec_out;
var dailycop = []
for (z in heatkwh) {
  var cop = heatkwh[z][1]/eleckwh[z][1];
  dailycop.push([heatkwh[z][0],cop]);
}

series.push({
  label:"COP", data:dailycop, yaxis:2, color:2,
  lines: { show: true, steps:true}
});
series.push({
  label:"heatpump_heat_kwh", data:heatkwh,yaxis:1, color:0, 
  bars: { show: true, align: "left", barWidth: 0.75*interval*1000, fill: true}
});
series.push({
  label:"heatpump_elec_kwh", data:eleckwh, yaxis:1, color:1, 
  bars: { show: true, align: "left", barWidth: 0.75*interval*1000, fill: true}
});

var options = {
  xaxis: { min: datastart, max: dataend, mode: "time", timezone: "browser" },
  selection: { mode: "x" }
};

$.plot("#placeholder_history",series, options);

$("#logout").click(function(){
  $.ajax({url: path+"user/logout", success:function(){window.location=path}});
});
</script>

