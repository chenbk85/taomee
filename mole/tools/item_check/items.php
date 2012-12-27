<?php
echo <<<html
<form action="items_process.php" method=POST>
<select name=type>
        <option>不可买卖物品
        <option>不可买物品
        <option>不可卖物品
        <option>打折物品
        <option>VIP物品
        <option selected>全部
</select><p>
<input type=submit>
</form>

<form enctype="multipart/form-data" action="upload.php" method="post"> 
　　　　<input type="hidden" name="max_file_size" value="100000"> 
<p>请选择文件： 
　　　　<input name="userfile" type="file">　　 
　　　　<input type="submit" value="上传文件">  
</form> 

html;
?>
