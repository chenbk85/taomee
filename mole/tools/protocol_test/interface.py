#!/usr/bin/env python
# -*- coding: utf-8 -*-
def interface(req):
    return """\
<html><body>
<form name="oForm" action="upload.py/upload" enctype="multpart/form-data" method="post"> 
     <p>File:  <input type="file" name="oFile1" ></p>
     <p><input type="submit" value="开始上传"></p>
</form>
</body></html>
"""

