# coding=utf-8

import os
import send_package
from mod_python import Session

sess = None

def interface():
    return """\
<html><body>
<form enctype="multipart/form-data" action="./upload" method="post">
<p>请选择配置文件: </p>
<p><input type="file" name="file"></p>
<p><input type="submit" value="上传文件"></p>
</form>
</body></html>
"""

def upload(req):
    try: # Windows needs stdio set for binary mode.
        import msvcrt
        msvcrt.setmode (0, os.O_BINARY) # stdin  = 0
        msvcrt.setmode (1, os.O_BINARY) # stdout = 1
    except ImportError:
        pass

    # A nested FieldStorage instance holds the file
    file_item = req.form['file']
#    basename = os.path.basename(file_item.filename)
#    return file_item.file
    if not file_item.filename:
        msg="没有文件被上传!"
    else:
        # return file_item.file
        basename=file_item.filename.split("\\")[-1]
#        return basename
        f1 = file_item.file
        #                f1 = file("/var/www/test.py", "rb")
      # build absolute path to files directory
        dir_path = "/tmp/"
	    
	global sess
	sess = Session.Session(req)
	sess['configfile'] = dir_path + basename
	sess.save()
		
        f2 = file((dir_path+basename), "wb")
        while 1:
            data = f1.read(4096)
            if not len(data):
                break
            f2.write(data)
        f2.close()
        msg = '配置文件 %s 已经成功上传' % basename
		
	    #Excute the config file now
														 	
#<p><a href="./interface">Upload and excute another configure file</a></p>
    if msg=="没有文件被上传!":
        return """\
<html><body>
<p>%s</p>
<form>
<p><a href="./interface">点击此处上传文件</a></p>
</form> 
</body></html>
""" % msg

    return """\
<html><body>
<p>%s</p>
<form action="./send_main" post="post">
<p><input type="submit" value="测试协议"></p>   
</form> 
</body></html>
""" % msg

def send_main(req):
    global sess
    sess = Session.Session(req)
    sess.load()
    configfile = sess['configfile']
   
    return send_package.do_send_package(configfile)
