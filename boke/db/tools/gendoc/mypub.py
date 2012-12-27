#!/usr/bin/python
# -*- coding: utf-8 -*-
import md5 
#import hashlib 
import re 
class fielddesc:
	#字段说明
	def __init__(self):
		pass
	def init(self,configfile):
		#从文件中得到配置
		self.field_desc_conf={};
		pf=open(configfile,"r")				
		for line in pf.readlines(): 
			m=re.match("\s*^([\w.]+)\s+(.*)",line)
			if(m):
				self.field_desc_conf[m.group(1)]=m.group(2)
		pass

	def getDesc(self, key ):
		#得到说明
		#key=key.split(".",1)[1];
		while True:
			if ( self. field_desc_conf .has_key(key)):	
				if self.field_desc_conf[key]:
					return self.field_desc_conf[key]
			key_arr=key.split(".",1);
			if len(key_arr)==2: 
				key=key_arr[1]
			else:
				return  key_arr[0]


def get_md5( str ):
	import hashlib
	m1 = hashlib.md5()    
	m1.update(str)  
	return m1.hexdigest().lower()


hexchars="0123456789ABCDEF"
import string

# Compare this to its 'c' equivalent in bin2hex.c...
def dit2hex(ch):
	i=ord(ch) # get its integral value.
	leftnib=i>>4
	rightnib=i & 0xf
	leftchar=hexchars[leftnib]
	rightchar=hexchars[rightnib]
	return leftchar+rightchar
	
# compare this to its 'c' equivalent in bin2hex.c...
def bin2hex(str):
	retvalue=""  # return value is empty until we add to it...
	for s in str:
		hexchars=dit2hex(s)
		retvalue=retvalue+hexchars
		pass
	return retvalue


# this is similar to the 'c' equivalent. 
def hex2dit(ch):
	if (ch >= '0') and (ch <= '9'):
		return ord(ch)-ord('0')
	if (ch >= 'a') and (ch <= 'f'):
		return ord(ch)-ord('a')+10
	if (ch >= 'A') and (ch <= 'F'):
		return ord(ch)-ord('A')+10
	hexval=bin2hex(ch)
	print "Invalid hex digit '%s' (hex value %s)." % (ch,hexval)
	raise "INVALID_HEX_DIGIT"

# turn a big-endian hex value into a string of binary values. put a leading
# zero on this guy if it doesn't have one so that we always have an even
# number of digits. Also strip any white space off the front & back first!
def hex2bin(str):
	###DEBUG###
	# print str
	str=string.strip(str) # zap white space. 
	if (not str):
		return None #agh!!!! NADA THERE!!!
	i=len(str)
	if (i % 2) != 0:
		str="0"+str  # make it an even number of digits.
		pass
	rightnib=None
	leftnib=None
	retvalue=""
	for s in str:
		if leftnib == None:
			leftnib=hex2dit(s)
		elif rightnib == None:
			rightnib=leftnib << 4
			leftnib=hex2dit(s)
		else:
			i=rightnib | leftnib
			retvalue=retvalue+chr(i)
			leftnib=hex2dit(s)
			rightnib=None
			pass
		pass
	i = rightnib | leftnib # we assured that it was even up above, so ...
	retvalue=retvalue+chr(i)

	# we should now have a return value!
	return retvalue
 
 
def get_chr( hex_re ):
	hex=hex_re.group();
	value=eval ("0x%s" % (hex)) ;
	return chr(value); 
def get_chr_ex( hex_re ):
	hex=hex_re.group()	
	if (hex==""): return " "; 
	value=eval ("0x%s" % (hex)) ;
	if (value>= 32 and value <=126 ):return chr(value); 
	else: return  "." ;
 

def get_hex( chr_re ):
	a_chr=chr_re.group();
	return "%02X "% ord(a_chr); 

def bin2hex_2(chrlist):
	sl=len(chrlist)
	phex=""
	for i in range(sl):
		phex=phex+"%02X"% ord(chrlist[i]); 
	return phex		

def print_hex_16( msg ,startid=0 ):
	msg=bin2hex_2(msg)
	pri_len=len(msg)/2
	i=0
	print "len[%3d]=========================================================================="% (pri_len)
	phex=re.compile(r'(\w\w)')
	while(i<pri_len):
		hex_msg=phex.sub(r"\1 ", msg[i*2:(i+16)*2]);
		chr_msg=phex.sub(get_chr_ex , msg[i*2:(i+16)*2]);
		print "\t%d\t %-24s %-24s  %-8s %-8s"	%(startid+ i, 
				hex_msg[0:24], hex_msg[24:48], 
				chr_msg[0:8], chr_msg[8:16] )
		i=i+16;
	print "end:=================================================================================="
def get_html_table_header(): 
	return """
<style type="text/css">
/* CSS Document */

body {
font: normal 12px auto "Trebuchet MS", Verdana, Arial, Helvetica, sans-serif;
color: #4f6b72;
background: #E6EAE9;
}

a{
color: #c75f3e;
}

#mytable {
width: 700px;
padding: 0;
margin: 0;
}

#mytable_1 {
width: 750px;
padding: 0;
margin: 0;
}



caption {
padding: 0 0 5px 0;
width: 700px;
font: bold  15px "Trebuchet MS", Verdana, Arial, Helvetica, sans-serif;
text-align: left;
}

th {
font: bold 12px "Trebuchet MS", Verdana, Arial, Helvetica, sans-serif;
color: #4f6b72;
border-right: 1px solid #C1DAD7;
border-bottom: 1px solid #C1DAD7;
border-top: 1px solid #C1DAD7;
letter-spacing: 2px;
text-align: left;
padding: 6px 6px 6px 12px;
background: #CAE8EA url(images/bg_header.jpg) no-repeat;
}
th.title_def {
font: bold 12px "Trebuchet MS", Verdana, Arial, Helvetica, sans-serif;
color: #4f6b72;
border-right: 1px solid #FFD8C0;
border-bottom: 1px solid #FFD8C0;
border-top: 1px solid #FFD8C0 ;
letter-spacing: 2px;
text-align: left;
padding: 6px 6px 6px 12px;
background: #FFD8C0 url(images/bg_header.jpg) no-repeat;
}



th.nobg {
border-top: 0;
border-left: 0;
border-right: 1px solid #C1DAD7;
background: none;
}

td {
border-right: 1px solid #C1DAD7;
border-bottom: 1px solid #C1DAD7;
background: #fff;
width:200;
font-size:12px;
padding: 6px 6px 6px 12px;
color: #4f6b72;
}


td.alt {
background: #F5FAFA;
color: #797268;
}
td.spec {
border-left: 1px solid #C1DAD7;
border-top: 0;
background: #fff url(images/bullet1.gif) no-repeat;
font: bold 10px "Trebuchet MS", Verdana, Arial, Helvetica, sans-serif;
}


th.spec {
border-left: 1px solid #C1DAD7;
border-top: 0;
background: #fff url(images/bullet1.gif) no-repeat;
font: bold 10px "Trebuchet MS", Verdana, Arial, Helvetica, sans-serif;
}

th.specalt {
border-left: 1px solid #C1DAD7;
border-top: 0;
background: #f5fafa url(images/bullet2.gif) no-repeat;
font: bold 10px "Trebuchet MS", Verdana, Arial, Helvetica, sans-serif;
color: #797268;
}
</style>

<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<head>
<link id="themestyle" rel="stylesheet" type="text/css" href="./css.css">
<SCRIPT LANGUAGE="JavaScript" src="../pub/pub.js"></SCRIPT>
</head>
<body  >
<div align=center>
	"""


