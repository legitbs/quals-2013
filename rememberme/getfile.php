<html>
<title>Remember Me</title>
<body>
<?php
$value = time();
$filename = $_GET["filename"];
$accesscode = $_GET["accesscode"];
if (md5($filename) == $accesscode){
	echo "Acces granted to $filename!<br><br>";
	srand($value);
	if (in_array($filename, array('getfile.php', 'index.html', 'key.txt', 'login.php', 'passwords.txt', 'usernames.txt'))==TRUE){
		$data = file_get_contents($filename);
		if ($data !== FALSE) {
			if ($filename == "key.txt") {
				$key = rand();
				$cyphertext = mcrypt_encrypt(MCRYPT_RIJNDAEL_128, $key, $data, MCRYPT_MODE_CBC);
				echo  base64_encode($cyphertext);
			}
			else{
				echo nl2br($data);
			}

		}
		else{
			echo "File does not exist";
		}
	}
	else{
		echo "File does not exist";
	}
	
}
else{
	echo "Invalid access code";
}
?>
</body>
</html>
