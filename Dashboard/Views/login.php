<?php

/*

Heatpump Monitor web dashboard code is released under the: 
GNU Affero General Public License and is based on emoncms.
Part of the OpenEnergyMonitor project:
http://openenergymonitor.org

*/

?>

<div id="header">
    <button id="logout" style="float:right; margin:5px; display:none">Logout</button>
    <div id="title">Heatpump Dashboard</div>
    <div id="maintext"></div>
    
    
</div>

<div id="box">
    <div id="maintext">
        <div id="login-form" style="display:none">
            <h2>Login</h2>
            
            <p>Username:<br>
                <input id="username" type="text" autocomplete="on" name="username"  />
            </p>
            
            <p>Password:<br>
                <input id="password" type="password" name="password" />
            </p>
            
            <button id="login">Login</button>
        </div>
        <div id="output"></div>
    </div>    
</div>

<script>
var path = "SITE DOMAIN";
var session = JSON.parse('<?php echo json_encode($session); ?>');

if (!session.valid) {
  $("#login-form").show();
} else {
  $("#login-form").hide(); $("#logout").show();
  $("#output").html("<br><b>Welcome:</b> "+session.username);
}

$("#login").click(function(){
  var username = $("#username").val();
  var password = $("#password").val();

  $.ajax({
    type: "POST", url: path+"user/login", data: "username="+username+"&password="+password,
    async: true, dataType: "json",
    success: function(result) {
        if (result!=false) {
            $("#login-form").hide(); $("#logout").show();
            $("#output").html("<br><b>Welcome:</b> "+result.username);
            window.location=path+"dashboard";
        }
    }
  });
});

$("#logout").click(function(){
  $.ajax({url: path+"user/logout", success:function(){location.reload()}});
});

</script>
