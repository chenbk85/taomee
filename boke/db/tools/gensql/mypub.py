#!/usr/bin/python
# -*- coding: utf-8 -*-
import md5 
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

	def getDesc(self, key , default_value="" ):
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
				if not default_value:
					return  key_arr[0]
				else:
					return default_value  
				

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


def get_xls_header(): 
	return """

<?xml version="1.0" encoding="UTF-8"?>
<?mso-application progid="Excel.Sheet"?>
<Workbook xmlns="urn:schemas-microsoft-com:office:spreadsheet"
 xmlns:o="urn:schemas-microsoft-com:office:office"
 xmlns:x="urn:schemas-microsoft-com:office:excel"
 xmlns:ss="urn:schemas-microsoft-com:office:spreadsheet"
 xmlns:html="http://www.w3.org/TR/REC-html40">
 <DocumentProperties xmlns="urn:schemas-microsoft-com:office:office">
  <Created>1996-12-17T01:32:42Z</Created>
  <LastSaved>2009-11-30T04:27:58Z</LastSaved>
  <Version>11.5606</Version>
 </DocumentProperties>
 <OfficeDocumentSettings xmlns="urn:schemas-microsoft-com:office:office">
  <RemovePersonalInformation/>
 </OfficeDocumentSettings>
 <ExcelWorkbook xmlns="urn:schemas-microsoft-com:office:excel">
  <WindowHeight>3750</WindowHeight>
  <WindowWidth>15480</WindowWidth>
  <WindowTopX>0</WindowTopX>
  <WindowTopY>7560</WindowTopY>
  <AcceptLabelsInFormulas/>
  <ProtectStructure>False</ProtectStructure>
  <ProtectWindows>False</ProtectWindows>
 </ExcelWorkbook>
 <Styles>
  <Style ss:ID="Default" ss:Name="Normal">
   <Alignment ss:Vertical="Bottom"/>
   <Borders/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="12"/>
   <Interior/>
   <NumberFormat/>
   <Protection/>
  </Style>
  <Style ss:ID="s23">
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9"/>
  </Style>
  <Style ss:ID="s24">
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9" ss:Bold="1"/>
   <Interior/>
  </Style>
  <Style ss:ID="s25">
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9" ss:Bold="1"/>
  </Style>
  <Style ss:ID="s26">
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9" ss:Color="#FF0000"/>
  </Style>
  <Style ss:ID="s27">
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9"/>
   <NumberFormat ss:Format="m&quot;月&quot;d&quot;日&quot;"/>
  </Style>
  <Style ss:ID="s28">
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9"/>
   <Interior/>
  </Style>
  <Style ss:ID="s29">
   <Alignment ss:Horizontal="Right" ss:Vertical="Bottom"/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9"/>
  </Style>
  <Style ss:ID="s30">
   <Alignment ss:Horizontal="Right" ss:Vertical="Bottom"/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9" ss:Bold="1"/>
   <Interior/>
  </Style>
  <Style ss:ID="s31">
   <Alignment ss:Horizontal="Right" ss:Vertical="Bottom"/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9" ss:Bold="1"/>
  </Style>
  <Style ss:ID="s32">
   <Alignment ss:Horizontal="Right" ss:Vertical="Bottom"/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9"/>
   <Interior/>
  </Style>
  <Style ss:ID="s33">
   <Alignment ss:Horizontal="Left" ss:Vertical="Bottom"/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9" ss:Bold="1"/>
   <Interior ss:Color="#3366FF" ss:Pattern="Solid"/>
  </Style>
  <Style ss:ID="s34">
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9"/>
   <Interior ss:Color="#99CC00" ss:Pattern="Solid"/>
  </Style>
  <Style ss:ID="s35">
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9" ss:Bold="1"/>
   <Interior ss:Color="#3366FF" ss:Pattern="Solid"/>
  </Style>
  <Style ss:ID="s36">
   <Alignment ss:Horizontal="Left" ss:Vertical="Bottom"/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9" ss:Color="#FF0000"
    ss:Bold="1"/>
  </Style>
  <Style ss:ID="s37">
   <Alignment ss:Horizontal="Left" ss:Vertical="Bottom"/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9"/>
  </Style>
  <Style ss:ID="s38">
   <Alignment ss:Horizontal="Left" ss:Vertical="Bottom"/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9"/>
   <Interior/>
  </Style>
  <Style ss:ID="s39">
   <Alignment ss:Horizontal="Left" ss:Vertical="Bottom"/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9" ss:Bold="1"/>
  </Style>
  <Style ss:ID="s40">
   <Alignment ss:Horizontal="Left" ss:Vertical="Bottom"/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9"/>
   <Interior ss:Color="#FF0000" ss:Pattern="Solid"/>
  </Style>
  <Style ss:ID="s41">
   <Alignment ss:Horizontal="Left" ss:Vertical="Bottom"/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9"/>
   <NumberFormat/>
  </Style>
  <Style ss:ID="s42">
   <Alignment ss:Horizontal="Left" ss:Vertical="Bottom"/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9" ss:Color="#FF0000"/>
  </Style>
  <Style ss:ID="s43">
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9" ss:Color="#008000"/>
  </Style>
  <Style ss:ID="s44">
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9" ss:Color="#008000"/>
   <Interior/>
  </Style>
  <Style ss:ID="s45">
   <Alignment ss:Horizontal="Right" ss:Vertical="Bottom"/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9" ss:Color="#FF0000"/>
  </Style>
  <Style ss:ID="s46">
   <Alignment ss:Horizontal="Left" ss:Vertical="Bottom"/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9" ss:Color="#FF0000"/>
   <Interior/>
  </Style>
  <Style ss:ID="s47">
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9" ss:Color="#339966"/>
   <Interior/>
  </Style>
  <Style ss:ID="s48">
   <Alignment ss:Horizontal="Left" ss:Vertical="Bottom"/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="14" ss:Color="#008000"
    ss:Bold="1"/>
  </Style>
  <Style ss:ID="s49">
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="14" ss:Color="#008000"
    ss:Bold="1"/>
  </Style>
  <Style ss:ID="s50">
   <Alignment ss:Horizontal="Left" ss:Vertical="Bottom"/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="14" ss:Color="#008000"
    ss:Bold="1"/>
   <Interior/>
  </Style>
  <Style ss:ID="s51">
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="14" ss:Color="#008000"
    ss:Bold="1"/>
   <Interior/>
  </Style>
  <Style ss:ID="s52">
   <Alignment ss:Horizontal="Left" ss:Vertical="Bottom"/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9"/>
   <Interior ss:Color="#99CC00" ss:Pattern="Solid"/>
  </Style>
  <Style ss:ID="s53">
   <Alignment ss:Horizontal="Right" ss:Vertical="Bottom"/>
   <Font ss:FontName="宋体" x:CharSet="134" ss:Size="9"/>
   <Interior ss:Color="#99CC00" ss:Pattern="Solid"/>
  </Style>
 </Styles>
 <Worksheet ss:Name="Sheet1">
  <Table ss:ExpandedColumnCount="10" ss:ExpandedRowCount="50000" x:FullColumns="1"
   x:FullRows="1" ss:StyleID="s23" ss:DefaultColumnWidth="54"
   ss:DefaultRowHeight="11.25">
   <Column ss:StyleID="s37" ss:Width="83"/>
   <Column ss:StyleID="s23" ss:Width="100"/>
   <Column ss:StyleID="s23" ss:Width="150"/>
   <Column ss:StyleID="s23" ss:Width="28"/>
   <Column ss:StyleID="s23" ss:Width="78"/>
   <Column ss:StyleID="s29" ss:Width="93"/>
   <Column ss:StyleID="s23" ss:Width="150"/>
"""

def get_xls_end(): 
	return """
  </Table>
  <WorksheetOptions xmlns="urn:schemas-microsoft-com:office:excel">
   <Print>
    <ValidPrinterInfo/>
    <PaperSizeIndex>9</PaperSizeIndex>
    <HorizontalResolution>300</HorizontalResolution>
    <VerticalResolution>300</VerticalResolution>
   </Print>
   <Selected/>
   <TopRowVisible>39</TopRowVisible>
   <Panes>
    <Pane>
     <Number>3</Number>
     <ActiveRow>81</ActiveRow>
     <ActiveCol>8</ActiveCol>
    </Pane>
   </Panes>
   <ProtectObjects>False</ProtectObjects>
   <ProtectScenarios>False</ProtectScenarios>
  </WorksheetOptions>
 </Worksheet>
 <Worksheet ss:Name="Sheet2">
  <Table ss:ExpandedColumnCount="0" ss:ExpandedRowCount="0" x:FullColumns="1"
   x:FullRows="1" ss:DefaultColumnWidth="54" ss:DefaultRowHeight="14.25"/>
  <WorksheetOptions xmlns="urn:schemas-microsoft-com:office:excel">
   <ProtectObjects>False</ProtectObjects>
   <ProtectScenarios>False</ProtectScenarios>
  </WorksheetOptions>
 </Worksheet>
 <Worksheet ss:Name="Sheet3">
  <Table ss:ExpandedColumnCount="0" ss:ExpandedRowCount="0" x:FullColumns="1"
   x:FullRows="1" ss:DefaultColumnWidth="54" ss:DefaultRowHeight="14.25"/>
  <WorksheetOptions xmlns="urn:schemas-microsoft-com:office:excel">
   <ProtectObjects>False</ProtectObjects>
   <ProtectScenarios>False</ProtectScenarios>
  </WorksheetOptions>
 </Worksheet>
</Workbook>
"""
