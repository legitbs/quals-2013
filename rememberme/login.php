<html>
<title>Remember Me</title>
<?php
if ($_SERVER['REQUEST_METHOD'] == "POST") {
	echo "<font color='red'> ACCESS DENIED!</font>";
}
?>
<body><form id='login' action='login.php' method='post' accept-charset='UTF-8'>
<fieldset >
<legend>Login</legend>
<label for='username' >UserName:</label>
<input type='text' name='username' id='username'  maxlength="50" /><br>
 
<label for='password' >Password:</label>
<input type='password' name='password' id='password' maxlength="50" /><br>
 
<input type='submit' name='Submit' value='Submit' />
 
</fieldset>
</form>
</body>
</html>

