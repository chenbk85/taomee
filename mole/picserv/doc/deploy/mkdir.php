
<?php
$dir1 = -1;
while ($dir1++ < 99) {
   $dir2 = -1;
   while($dir2++ < 99)
   mkdir('/opt/taomee/pic_server_root/mole_pictures/'.str_pad($dir1, 2, '0', STR_PAD_LEFT).'/'.str_pad($dir2, 2, '0', STR_PAD_LEFT), 0755, true);
}

?>
