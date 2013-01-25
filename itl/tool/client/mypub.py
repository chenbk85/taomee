#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
        key=key.split(".",1)[1];
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
    try:
        import hashlib 
        m1 =  hashlib.md5()   
    except  msg:
        try:
            import md5 
            m1 = md5.new()   
        except  msg:
            pass
        pass

    m1.update(str)  
    return m1.hexdigest().lower()

def get_file_md5(file):
    import hashlib;
    m = hashlib.md5();
    f = open(file, 'r');
    bytes = f.read();
    m.update(bytes);
    f.close()
    return m.hexdigest().lower()

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

def stripslashes(str):
    tmp_str=str.replace('\\n','\n' );
    tmp_str=tmp_str.replace('\\r','\r' );
    tmp_str=tmp_str.replace('\\0','\0' );
    tmp_str=tmp_str.replace('\\\'','\'' );
    tmp_str=tmp_str.replace('\\\"','\"' );
    return tmp_str;


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

