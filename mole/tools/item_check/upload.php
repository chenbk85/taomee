<?php
$uploaddir = '/var/www/';
$uploadfile = $uploaddir . basename($_FILES['userfile']['name']);

echo '<pre>';
if (move_uploaded_file($_FILES['userfile']['tmp_name'], $uploadfile)) {
    echo "File is valid, and was successfully uploaded.\n";
	echo "Upload file name: ".$_FILES['userfile']['name'];
} else {
    echo "Possible file upload attack!\n";
}


print "</pre>";
echo <<<back
<a href="items.php">返回</a>。 
back;
?> 

