import md5 
import re 
def get_md5( str ):
	m1 = md5.new()   
	m1.update(str)  
	return m1.hexdigest().lower()
 
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

def hex2bin(hexlist):
	phex=re.compile(r'(\w\w)')
	return phex.sub(get_chr , hexlist );

def bin2hex(chrlist):
	sl=len(chrlist)
	phex=""
	for i in range(sl):
		phex=phex+"%02X "% ord(chrlist[i]); 
	return phex		
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
