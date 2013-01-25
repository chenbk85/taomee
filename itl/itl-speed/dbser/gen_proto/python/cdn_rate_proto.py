# -*- coding: utf-8 -*-

hexchars="0123456789ABCDEF"
def dit2hex(ch):
	i=ord(ch) # get its integral value.
	leftnib=i>>4
	rightnib=i & 0xf
	leftchar=hexchars[leftnib]
	rightchar=hexchars[rightnib]
	return leftchar+rightchar+" "
	
def bin2hex(str):
	retvalue=""  # return value is empty until we add to it...
	for s in str:
		hexchars=dit2hex(s)
		retvalue=retvalue+hexchars
		pass
	return retvalue

		
class cdnrate_ip_isp_in :
	"""
参数列表:
	/* 项目编号 */
	#类型:uint32
	public prjid;

	/* 上报时间戳 */
	#类型:uint32
	public logtime;

	/* cdn服务器ip */
	#类型:uint32
	public cdnip;

	/* 省编号 */
	#类型:uint32
	public provid;

	/* 城市编号 */
	#类型:uint32
	public cityid;

	/* 运营商名称 */
	#定长数组,长度:64, 类型:char 
	public comp ;

	/* cdn下载速度和 */
	#类型:uint32
	public value;

	/* 下载次数 */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.prjid=ba.read_uint32();
		if ( self.prjid== None): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.cdnip=ba.read_uint32();
		if ( self.cdnip== None): return False;
		self.provid=ba.read_uint32();
		if ( self.provid== None): return False;
		self.cityid=ba.read_uint32();
		if ( self.cityid== None): return False;
		self.comp=ba.read_buf(64);
		if( self.comp==None ): return False;
		self.value=ba.read_uint32();
		if ( self.value== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.prjid)):return False;
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.cdnip)):return False;
		if ( not ba.write_uint32(self.provid)):return False;
		if ( not ba.write_uint32(self.cityid)):return False;
		ba.write_buf(self.comp,64);
		if ( not ba.write_uint32(self.value)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 项目编号 */"""
		print tabs+"[prjid]=%s"%(str(self.prjid))
		print tabs+"""/* 上报时间戳 */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/* cdn服务器ip */"""
		print tabs+"[cdnip]=%s"%(str(self.cdnip))
		print tabs+"""/* 省编号 */"""
		print tabs+"[provid]=%s"%(str(self.provid))
		print tabs+"""/* 城市编号 */"""
		print tabs+"[cityid]=%s"%(str(self.cityid))
		print tabs+"""/* 运营商名称 */"""
		tmp_str=str(self.comp);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[comp]=%s"%(tmp_str)
		print tabs+"""/* cdn下载速度和 */"""
		print tabs+"[value]=%s"%(str(self.value))
		print tabs+"""/* 下载次数 */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class cdn_report_ip_in :
	"""
参数列表:
	/* 项目编号 */
	#类型:uint32
	public prjid;

	/* ip地址 */
	#类型:uint32
	public ip;

	/* 上报时间戳 */
	#类型:uint32
	public logtime;

	/* cdn下载速度和 */
	#类型:uint32
	public value;

	/* 下载次数 */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.prjid=ba.read_uint32();
		if ( self.prjid== None): return False;
		self.ip=ba.read_uint32();
		if ( self.ip== None): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.value=ba.read_uint32();
		if ( self.value== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.prjid)):return False;
		if ( not ba.write_uint32(self.ip)):return False;
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.value)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 项目编号 */"""
		print tabs+"[prjid]=%s"%(str(self.prjid))
		print tabs+"""/* ip地址 */"""
		print tabs+"[ip]=%s"%(str(self.ip))
		print tabs+"""/* 上报时间戳 */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/* cdn下载速度和 */"""
		print tabs+"[value]=%s"%(str(self.value))
		print tabs+"""/* 下载次数 */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class cdn_report_prov_in :
	"""
参数列表:
	/* 项目编号 */
	#类型:uint32
	public prjid;

	/* 省编号 */
	#类型:uint32
	public provid;

	/* 城市编号 */
	#类型:uint32
	public cityid;

	/* 运营商名称 */
	#定长数组,长度:64, 类型:char 
	public comp ;

	/* 下载时间 */
	#类型:uint32
	public logtime;

	/* 下载速度和 */
	#类型:uint32
	public value;

	/* 下载次数 */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.prjid=ba.read_uint32();
		if ( self.prjid== None): return False;
		self.provid=ba.read_uint32();
		if ( self.provid== None): return False;
		self.cityid=ba.read_uint32();
		if ( self.cityid== None): return False;
		self.comp=ba.read_buf(64);
		if( self.comp==None ): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.value=ba.read_uint32();
		if ( self.value== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.prjid)):return False;
		if ( not ba.write_uint32(self.provid)):return False;
		if ( not ba.write_uint32(self.cityid)):return False;
		ba.write_buf(self.comp,64);
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.value)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 项目编号 */"""
		print tabs+"[prjid]=%s"%(str(self.prjid))
		print tabs+"""/* 省编号 */"""
		print tabs+"[provid]=%s"%(str(self.provid))
		print tabs+"""/* 城市编号 */"""
		print tabs+"[cityid]=%s"%(str(self.cityid))
		print tabs+"""/* 运营商名称 */"""
		tmp_str=str(self.comp);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[comp]=%s"%(tmp_str)
		print tabs+"""/* 下载时间 */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/* 下载速度和 */"""
		print tabs+"[value]=%s"%(str(self.value))
		print tabs+"""/* 下载次数 */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class net_stat_ip_report_in :
	"""
参数列表:
	/* 机房id */
	#类型:uint32
	public idcid;

	/* ip地址 */
	#类型:uint32
	public ip;

	/* 上报时间戳 */
	#类型:uint32
	public logtime;

	/* 延迟 */
	#类型:uint32
	public delay_v;

	/* 丢包率 */
	#类型:uint32
	public lost_v;

	/* 跳数 */
	#类型:uint32
	public hop_v;

	/* 探测次数 */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.idcid=ba.read_uint32();
		if ( self.idcid== None): return False;
		self.ip=ba.read_uint32();
		if ( self.ip== None): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.delay_v=ba.read_uint32();
		if ( self.delay_v== None): return False;
		self.lost_v=ba.read_uint32();
		if ( self.lost_v== None): return False;
		self.hop_v=ba.read_uint32();
		if ( self.hop_v== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.idcid)):return False;
		if ( not ba.write_uint32(self.ip)):return False;
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.delay_v)):return False;
		if ( not ba.write_uint32(self.lost_v)):return False;
		if ( not ba.write_uint32(self.hop_v)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 机房id */"""
		print tabs+"[idcid]=%s"%(str(self.idcid))
		print tabs+"""/* ip地址 */"""
		print tabs+"[ip]=%s"%(str(self.ip))
		print tabs+"""/* 上报时间戳 */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/* 延迟 */"""
		print tabs+"[delay_v]=%s"%(str(self.delay_v))
		print tabs+"""/* 丢包率 */"""
		print tabs+"[lost_v]=%s"%(str(self.lost_v))
		print tabs+"""/* 跳数 */"""
		print tabs+"[hop_v]=%s"%(str(self.hop_v))
		print tabs+"""/* 探测次数 */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class net_stat_link_report_in :
	"""
参数列表:
	/* 链路编号 */
	#类型:uint32
	public link;

	/* 上报时间戳 */
	#类型:uint32
	public logtime;

	/* 延迟 */
	#类型:uint32
	public delay_v;

	/* 丢包率 */
	#类型:uint32
	public lost_v;

	/* 跳数 */
	#类型:uint32
	public hop_v;

	/* 探测次数 */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.link=ba.read_uint32();
		if ( self.link== None): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.delay_v=ba.read_uint32();
		if ( self.delay_v== None): return False;
		self.lost_v=ba.read_uint32();
		if ( self.lost_v== None): return False;
		self.hop_v=ba.read_uint32();
		if ( self.hop_v== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.link)):return False;
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.delay_v)):return False;
		if ( not ba.write_uint32(self.lost_v)):return False;
		if ( not ba.write_uint32(self.hop_v)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 链路编号 */"""
		print tabs+"[link]=%s"%(str(self.link))
		print tabs+"""/* 上报时间戳 */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/* 延迟 */"""
		print tabs+"[delay_v]=%s"%(str(self.delay_v))
		print tabs+"""/* 丢包率 */"""
		print tabs+"[lost_v]=%s"%(str(self.lost_v))
		print tabs+"""/* 跳数 */"""
		print tabs+"[hop_v]=%s"%(str(self.hop_v))
		print tabs+"""/* 探测次数 */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class net_stat_prov_report_in :
	"""
参数列表:
	/* 机房id */
	#类型:uint32
	public idcid;

	/* 省编号 */
	#类型:uint32
	public provid;

	/* 城市编号 */
	#类型:uint32
	public cityid;

	/* 运营商名称 */
	#定长数组,长度:64, 类型:char 
	public comp ;

	/* 上报时间戳 */
	#类型:uint32
	public logtime;

	/* 延迟 */
	#类型:uint32
	public delay_v;

	/* 丢包率 */
	#类型:uint32
	public lost_v;

	/* 跳数 */
	#类型:uint32
	public hop_v;

	/* 探测次数 */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.idcid=ba.read_uint32();
		if ( self.idcid== None): return False;
		self.provid=ba.read_uint32();
		if ( self.provid== None): return False;
		self.cityid=ba.read_uint32();
		if ( self.cityid== None): return False;
		self.comp=ba.read_buf(64);
		if( self.comp==None ): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.delay_v=ba.read_uint32();
		if ( self.delay_v== None): return False;
		self.lost_v=ba.read_uint32();
		if ( self.lost_v== None): return False;
		self.hop_v=ba.read_uint32();
		if ( self.hop_v== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.idcid)):return False;
		if ( not ba.write_uint32(self.provid)):return False;
		if ( not ba.write_uint32(self.cityid)):return False;
		ba.write_buf(self.comp,64);
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.delay_v)):return False;
		if ( not ba.write_uint32(self.lost_v)):return False;
		if ( not ba.write_uint32(self.hop_v)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 机房id */"""
		print tabs+"[idcid]=%s"%(str(self.idcid))
		print tabs+"""/* 省编号 */"""
		print tabs+"[provid]=%s"%(str(self.provid))
		print tabs+"""/* 城市编号 */"""
		print tabs+"[cityid]=%s"%(str(self.cityid))
		print tabs+"""/* 运营商名称 */"""
		tmp_str=str(self.comp);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[comp]=%s"%(tmp_str)
		print tabs+"""/* 上报时间戳 */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/* 延迟 */"""
		print tabs+"[delay_v]=%s"%(str(self.delay_v))
		print tabs+"""/* 丢包率 */"""
		print tabs+"[lost_v]=%s"%(str(self.lost_v))
		print tabs+"""/* 跳数 */"""
		print tabs+"[hop_v]=%s"%(str(self.hop_v))
		print tabs+"""/* 探测次数 */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class net_stat_report_all_idc_delay_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public logtime;

	/*  */
	#类型:uint32
	public value;

	/*  */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.value=ba.read_uint32();
		if ( self.value== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.value)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/*  */"""
		print tabs+"[value]=%s"%(str(self.value))
		print tabs+"""/*  */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class net_stat_report_all_idc_hop_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public logtime;

	/*  */
	#类型:uint32
	public value;

	/*  */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.value=ba.read_uint32();
		if ( self.value== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.value)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/*  */"""
		print tabs+"[value]=%s"%(str(self.value))
		print tabs+"""/*  */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class net_stat_report_all_idc_lost_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public logtime;

	/*  */
	#类型:uint32
	public value;

	/*  */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.value=ba.read_uint32();
		if ( self.value== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.value)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/*  */"""
		print tabs+"[value]=%s"%(str(self.value))
		print tabs+"""/*  */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class net_stat_report_idc_delay_in :
	"""
参数列表:
	/* idc编号 */
	#类型:uint32
	public idc;

	/*  */
	#类型:uint32
	public logtime;

	/*  */
	#类型:uint32
	public value;

	/*  */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.idc=ba.read_uint32();
		if ( self.idc== None): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.value=ba.read_uint32();
		if ( self.value== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.idc)):return False;
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.value)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* idc编号 */"""
		print tabs+"[idc]=%s"%(str(self.idc))
		print tabs+"""/*  */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/*  */"""
		print tabs+"[value]=%s"%(str(self.value))
		print tabs+"""/*  */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class net_stat_report_idc_hop_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public idc;

	/*  */
	#类型:uint32
	public logtime;

	/*  */
	#类型:uint32
	public value;

	/*  */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.idc=ba.read_uint32();
		if ( self.idc== None): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.value=ba.read_uint32();
		if ( self.value== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.idc)):return False;
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.value)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[idc]=%s"%(str(self.idc))
		print tabs+"""/*  */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/*  */"""
		print tabs+"[value]=%s"%(str(self.value))
		print tabs+"""/*  */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class net_stat_report_idc_lost_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public idc;

	/*  */
	#类型:uint32
	public logtime;

	/*  */
	#类型:uint32
	public value;

	/*  */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.idc=ba.read_uint32();
		if ( self.idc== None): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.value=ba.read_uint32();
		if ( self.value== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.idc)):return False;
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.value)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[idc]=%s"%(str(self.idc))
		print tabs+"""/*  */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/*  */"""
		print tabs+"[value]=%s"%(str(self.value))
		print tabs+"""/*  */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class net_stat_report_ip_delay_in :
	"""
参数列表:
	/* 机房id */
	#类型:uint32
	public idcid;

	/*  */
	#类型:uint32
	public ip;

	/* 探测时间 */
	#类型:uint32
	public logtime;

	/* 时延和 */
	#类型:uint32
	public value;

	/* 探测的次数 */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.idcid=ba.read_uint32();
		if ( self.idcid== None): return False;
		self.ip=ba.read_uint32();
		if ( self.ip== None): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.value=ba.read_uint32();
		if ( self.value== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.idcid)):return False;
		if ( not ba.write_uint32(self.ip)):return False;
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.value)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 机房id */"""
		print tabs+"[idcid]=%s"%(str(self.idcid))
		print tabs+"""/*  */"""
		print tabs+"[ip]=%s"%(str(self.ip))
		print tabs+"""/* 探测时间 */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/* 时延和 */"""
		print tabs+"[value]=%s"%(str(self.value))
		print tabs+"""/* 探测的次数 */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class net_stat_report_ip_hop_in :
	"""
参数列表:
	/* 机房id */
	#类型:uint32
	public idcid;

	/*  */
	#类型:uint32
	public ip;

	/*  */
	#类型:uint32
	public logtime;

	/* 一分钟内ttl和 */
	#类型:uint32
	public value;

	/* 一分钟内探测次数 */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.idcid=ba.read_uint32();
		if ( self.idcid== None): return False;
		self.ip=ba.read_uint32();
		if ( self.ip== None): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.value=ba.read_uint32();
		if ( self.value== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.idcid)):return False;
		if ( not ba.write_uint32(self.ip)):return False;
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.value)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 机房id */"""
		print tabs+"[idcid]=%s"%(str(self.idcid))
		print tabs+"""/*  */"""
		print tabs+"[ip]=%s"%(str(self.ip))
		print tabs+"""/*  */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/* 一分钟内ttl和 */"""
		print tabs+"[value]=%s"%(str(self.value))
		print tabs+"""/* 一分钟内探测次数 */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class net_stat_report_ip_lost_in :
	"""
参数列表:
	/* 机房id */
	#类型:uint32
	public idcid;

	/*  */
	#类型:uint32
	public ip;

	/*  */
	#类型:uint32
	public logtime;

	/* 一分钟内丢包率和 */
	#类型:uint32
	public value;

	/* 探测次数 */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.idcid=ba.read_uint32();
		if ( self.idcid== None): return False;
		self.ip=ba.read_uint32();
		if ( self.ip== None): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.value=ba.read_uint32();
		if ( self.value== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.idcid)):return False;
		if ( not ba.write_uint32(self.ip)):return False;
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.value)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 机房id */"""
		print tabs+"[idcid]=%s"%(str(self.idcid))
		print tabs+"""/*  */"""
		print tabs+"[ip]=%s"%(str(self.ip))
		print tabs+"""/*  */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/* 一分钟内丢包率和 */"""
		print tabs+"[value]=%s"%(str(self.value))
		print tabs+"""/* 探测次数 */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class net_stat_report_link_delay_in :
	"""
参数列表:
	/* 链路编号 */
	#类型:uint32
	public link;

	/*  */
	#类型:uint32
	public logtime;

	/*  */
	#类型:uint32
	public value;

	/*  */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.link=ba.read_uint32();
		if ( self.link== None): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.value=ba.read_uint32();
		if ( self.value== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.link)):return False;
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.value)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 链路编号 */"""
		print tabs+"[link]=%s"%(str(self.link))
		print tabs+"""/*  */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/*  */"""
		print tabs+"[value]=%s"%(str(self.value))
		print tabs+"""/*  */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class net_stat_report_link_hop_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public link;

	/*  */
	#类型:uint32
	public logtime;

	/*  */
	#类型:uint32
	public value;

	/*  */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.link=ba.read_uint32();
		if ( self.link== None): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.value=ba.read_uint32();
		if ( self.value== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.link)):return False;
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.value)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[link]=%s"%(str(self.link))
		print tabs+"""/*  */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/*  */"""
		print tabs+"[value]=%s"%(str(self.value))
		print tabs+"""/*  */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class net_stat_report_link_lost_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public link;

	/*  */
	#类型:uint32
	public logtime;

	/*  */
	#类型:uint32
	public value;

	/*  */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.link=ba.read_uint32();
		if ( self.link== None): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.value=ba.read_uint32();
		if ( self.value== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.link)):return False;
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.value)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[link]=%s"%(str(self.link))
		print tabs+"""/*  */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/*  */"""
		print tabs+"[value]=%s"%(str(self.value))
		print tabs+"""/*  */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class net_stat_report_prov_delay_in :
	"""
参数列表:
	/* 机房id */
	#类型:uint32
	public idcid;

	/*  */
	#类型:uint32
	public provid;

	/*  */
	#类型:uint32
	public cityid;

	/*  */
	#定长数组,长度:64, 类型:char 
	public comp ;

	/*  */
	#类型:uint32
	public logtime;

	/* 一分钟内时延和 */
	#类型:uint32
	public value;

	/* 一分钟内探测次数 */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.idcid=ba.read_uint32();
		if ( self.idcid== None): return False;
		self.provid=ba.read_uint32();
		if ( self.provid== None): return False;
		self.cityid=ba.read_uint32();
		if ( self.cityid== None): return False;
		self.comp=ba.read_buf(64);
		if( self.comp==None ): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.value=ba.read_uint32();
		if ( self.value== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.idcid)):return False;
		if ( not ba.write_uint32(self.provid)):return False;
		if ( not ba.write_uint32(self.cityid)):return False;
		ba.write_buf(self.comp,64);
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.value)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 机房id */"""
		print tabs+"[idcid]=%s"%(str(self.idcid))
		print tabs+"""/*  */"""
		print tabs+"[provid]=%s"%(str(self.provid))
		print tabs+"""/*  */"""
		print tabs+"[cityid]=%s"%(str(self.cityid))
		print tabs+"""/*  */"""
		tmp_str=str(self.comp);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[comp]=%s"%(tmp_str)
		print tabs+"""/*  */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/* 一分钟内时延和 */"""
		print tabs+"[value]=%s"%(str(self.value))
		print tabs+"""/* 一分钟内探测次数 */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class net_stat_report_prov_hop_in :
	"""
参数列表:
	/* 机房id */
	#类型:uint32
	public idcid;

	/*  */
	#类型:uint32
	public provid;

	/*  */
	#类型:uint32
	public cityid;

	/*  */
	#定长数组,长度:64, 类型:char 
	public comp ;

	/*  */
	#类型:uint32
	public logtime;

	/*  */
	#类型:uint32
	public value;

	/*  */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.idcid=ba.read_uint32();
		if ( self.idcid== None): return False;
		self.provid=ba.read_uint32();
		if ( self.provid== None): return False;
		self.cityid=ba.read_uint32();
		if ( self.cityid== None): return False;
		self.comp=ba.read_buf(64);
		if( self.comp==None ): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.value=ba.read_uint32();
		if ( self.value== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.idcid)):return False;
		if ( not ba.write_uint32(self.provid)):return False;
		if ( not ba.write_uint32(self.cityid)):return False;
		ba.write_buf(self.comp,64);
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.value)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 机房id */"""
		print tabs+"[idcid]=%s"%(str(self.idcid))
		print tabs+"""/*  */"""
		print tabs+"[provid]=%s"%(str(self.provid))
		print tabs+"""/*  */"""
		print tabs+"[cityid]=%s"%(str(self.cityid))
		print tabs+"""/*  */"""
		tmp_str=str(self.comp);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[comp]=%s"%(tmp_str)
		print tabs+"""/*  */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/*  */"""
		print tabs+"[value]=%s"%(str(self.value))
		print tabs+"""/*  */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class net_stat_report_prov_lost_in :
	"""
参数列表:
	/* 机房id */
	#类型:uint32
	public idcid;

	/*  */
	#类型:uint32
	public provid;

	/*  */
	#类型:uint32
	public cityid;

	/*  */
	#定长数组,长度:64, 类型:char 
	public comp ;

	/*  */
	#类型:uint32
	public logtime;

	/* 一分钟内丢包率和 */
	#类型:uint32
	public value;

	/* 一分钟内探测次数 */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.idcid=ba.read_uint32();
		if ( self.idcid== None): return False;
		self.provid=ba.read_uint32();
		if ( self.provid== None): return False;
		self.cityid=ba.read_uint32();
		if ( self.cityid== None): return False;
		self.comp=ba.read_buf(64);
		if( self.comp==None ): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.value=ba.read_uint32();
		if ( self.value== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.idcid)):return False;
		if ( not ba.write_uint32(self.provid)):return False;
		if ( not ba.write_uint32(self.cityid)):return False;
		ba.write_buf(self.comp,64);
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.value)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 机房id */"""
		print tabs+"[idcid]=%s"%(str(self.idcid))
		print tabs+"""/*  */"""
		print tabs+"[provid]=%s"%(str(self.provid))
		print tabs+"""/*  */"""
		print tabs+"[cityid]=%s"%(str(self.cityid))
		print tabs+"""/*  */"""
		tmp_str=str(self.comp);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[comp]=%s"%(tmp_str)
		print tabs+"""/*  */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/* 一分钟内丢包率和 */"""
		print tabs+"[value]=%s"%(str(self.value))
		print tabs+"""/* 一分钟内探测次数 */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class url_rate_record_in :
	"""
参数列表:
	/* 项目编号 */
	#类型:uint32
	public prjid;

	/* 用户id */
	#类型:uint32
	public userid;

	/* 客户端ip */
	#类型:uint32
	public clientip;

	/* 时间戳 */
	#类型:uint32
	public time;

	/* 页面id */
	#类型:uint32
	public pageid;

	/*起点到每个点的耗时，最多10个点*/
	#变长数组,最大长度:20, 类型:uint32_t
	public vlist =array();


	"""

	def read_from_buf(self, ba ):
		self.prjid=ba.read_uint32();
		if ( self.prjid== None): return False;
		self.userid=ba.read_uint32();
		if ( self.userid== None): return False;
		self.clientip=ba.read_uint32();
		if ( self.clientip== None): return False;
		self.time=ba.read_uint32();
		if ( self.time== None): return False;
		self.pageid=ba.read_uint32();
		if ( self.pageid== None): return False;

		vlist_count= ba.read_uint32();
		if ( vlist_count==None ): return False;
		if (vlist_count>20):return False;
		self.vlist=[];
		for i in range(vlist_count):
			self.vlist.append(ba.read_uint32());
			if ( self.vlist[i]== None): return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.prjid)):return False;
		if ( not ba.write_uint32(self.userid)):return False;
		if ( not ba.write_uint32(self.clientip)):return False;
		if ( not ba.write_uint32(self.time)):return False;
		if ( not ba.write_uint32(self.pageid)):return False;
		vlist_count=len(self.vlist);
		if (vlist_count>20 ): return False; 
		ba.write_uint32(vlist_count);
		for i in range(vlist_count):
			if ( not ba.write_uint32(self.vlist[i])):return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 项目编号 */"""
		print tabs+"[prjid]=%s"%(str(self.prjid))
		print tabs+"""/* 用户id */"""
		print tabs+"[userid]=%s"%(str(self.userid))
		print tabs+"""/* 客户端ip */"""
		print tabs+"[clientip]=%s"%(str(self.clientip))
		print tabs+"""/* 时间戳 */"""
		print tabs+"[time]=%s"%(str(self.time))
		print tabs+"""/* 页面id */"""
		print tabs+"[pageid]=%s"%(str(self.pageid))
		print tabs+"#起点到每个点的耗时，最多10个点"
		print tabs+"[vlist]=>{";
		for i in range(len(self.vlist)):
			print tabs+"    [%d]="%(i)+str(self.vlist[i]); 

		print tabs+'}'
		
		pass;
	




class url_stat_report_point_time_in :
	"""
参数列表:
	/* 项目id */
	#类型:uint32
	public prjid;

	/* 页面id */
	#类型:uint32
	public pageid;

	/*  */
	#类型:uint32
	public provid;

	/*  */
	#类型:uint32
	public cityid;

	/* ISP编号 */
	#类型:uint32
	public ISPID;

	/* 运营商名称 */
	#定长数组,长度:64, 类型:char 
	public comp ;

	/* 上报时间戳 */
	#类型:uint32
	public logtime;

	/* 上报的数量 */
	#类型:uint32
	public repot_cnt;

	/**/
	#变长数组,最大长度:20, 类型:uint32_t
	public vlist =array();


	"""

	def read_from_buf(self, ba ):
		self.prjid=ba.read_uint32();
		if ( self.prjid== None): return False;
		self.pageid=ba.read_uint32();
		if ( self.pageid== None): return False;
		self.provid=ba.read_uint32();
		if ( self.provid== None): return False;
		self.cityid=ba.read_uint32();
		if ( self.cityid== None): return False;
		self.ISPID=ba.read_uint32();
		if ( self.ISPID== None): return False;
		self.comp=ba.read_buf(64);
		if( self.comp==None ): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.repot_cnt=ba.read_uint32();
		if ( self.repot_cnt== None): return False;

		vlist_count= ba.read_uint32();
		if ( vlist_count==None ): return False;
		if (vlist_count>20):return False;
		self.vlist=[];
		for i in range(vlist_count):
			self.vlist.append(ba.read_uint32());
			if ( self.vlist[i]== None): return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.prjid)):return False;
		if ( not ba.write_uint32(self.pageid)):return False;
		if ( not ba.write_uint32(self.provid)):return False;
		if ( not ba.write_uint32(self.cityid)):return False;
		if ( not ba.write_uint32(self.ISPID)):return False;
		ba.write_buf(self.comp,64);
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.repot_cnt)):return False;
		vlist_count=len(self.vlist);
		if (vlist_count>20 ): return False; 
		ba.write_uint32(vlist_count);
		for i in range(vlist_count):
			if ( not ba.write_uint32(self.vlist[i])):return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 项目id */"""
		print tabs+"[prjid]=%s"%(str(self.prjid))
		print tabs+"""/* 页面id */"""
		print tabs+"[pageid]=%s"%(str(self.pageid))
		print tabs+"""/*  */"""
		print tabs+"[provid]=%s"%(str(self.provid))
		print tabs+"""/*  */"""
		print tabs+"[cityid]=%s"%(str(self.cityid))
		print tabs+"""/* ISP编号 */"""
		print tabs+"[ISPID]=%s"%(str(self.ISPID))
		print tabs+"""/* 运营商名称 */"""
		tmp_str=str(self.comp);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[comp]=%s"%(tmp_str)
		print tabs+"""/* 上报时间戳 */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/* 上报的数量 */"""
		print tabs+"[repot_cnt]=%s"%(str(self.repot_cnt))
		print tabs+"#"
		print tabs+"[vlist]=>{";
		for i in range(len(self.vlist)):
			print tabs+"    [%d]="%(i)+str(self.vlist[i]); 

		print tabs+'}'
		
		pass;
	




create_time='2011年08月30日 星期2 13:44:24'
cmd_map={ }
cmd_map[0x1001]=(0x1001,'cdn_report_ip',cdn_report_ip_in,None,0x55dbed83)
cmd_map[0x1002]=(0x1002,'cdn_report_prov',cdn_report_prov_in,None,0x433d49cc)
cmd_map[0x1003]=(0x1003,'net_stat_report_ip_delay',net_stat_report_ip_delay_in,None,0x004382d1)
cmd_map[0x1004]=(0x1004,'net_stat_report_prov_delay',net_stat_report_prov_delay_in,None,0xdcf31efa)
cmd_map[0x1005]=(0x1005,'net_stat_report_ip_lost',net_stat_report_ip_lost_in,None,0x62125738)
cmd_map[0x1006]=(0x1006,'net_stat_report_prov_lost',net_stat_report_prov_lost_in,None,0x0427d85e)
cmd_map[0x1007]=(0x1007,'net_stat_report_ip_hop',net_stat_report_ip_hop_in,None,0x3e65b476)
cmd_map[0x1008]=(0x1008,'net_stat_report_prov_hop',net_stat_report_prov_hop_in,None,0x88f1a8cd)
cmd_map[0x1009]=(0x1009,'net_stat_report_link_delay',net_stat_report_link_delay_in,None,0xa11782a9)
cmd_map[0x1010]=(0x1010,'net_stat_report_link_lost',net_stat_report_link_lost_in,None,0xac44711e)
cmd_map[0x1011]=(0x1011,'net_stat_report_link_hop',net_stat_report_link_hop_in,None,0x27390bdb)
cmd_map[0x1012]=(0x1012,'net_stat_report_idc_delay',net_stat_report_idc_delay_in,None,0x4a8f5029)
cmd_map[0x1013]=(0x1013,'net_stat_report_idc_lost',net_stat_report_idc_lost_in,None,0x46353d7b)
cmd_map[0x1014]=(0x1014,'net_stat_report_idc_hop',net_stat_report_idc_hop_in,None,0xe5acfd47)
cmd_map[0x1015]=(0x1015,'net_stat_report_all_idc_delay',net_stat_report_all_idc_delay_in,None,0xedc0dda3)
cmd_map[0x1016]=(0x1016,'net_stat_report_all_idc_lost',net_stat_report_all_idc_lost_in,None,0x96b2c1ba)
cmd_map[0x1017]=(0x1017,'net_stat_report_all_idc_hop',net_stat_report_all_idc_hop_in,None,0x135e812a)
cmd_map[0x1018]=(0x1018,'url_stat_report_point_time',url_stat_report_point_time_in,None,0x0e074eb4)
cmd_map[0x1019]=(0x1019,'url_rate_record',url_rate_record_in,None,0xd0e624e9)
cmd_map[0x1201]=(0x1201,'cdnrate_ip_isp',cdnrate_ip_isp_in,None,0x8b99a5d5)
cmd_map[0x1211]=(0x1211,'net_stat_ip_report',net_stat_ip_report_in,None,0x20e4ac55)
cmd_map[0x1212]=(0x1212,'net_stat_prov_report',net_stat_prov_report_in,None,0x044b54b8)
cmd_map[0x1213]=(0x1213,'net_stat_link_report',net_stat_link_report_in,None,0x28b4687a)
	
def init_cmd_map():
	pass

try:
	import cdn_rate_proto_base
	base_type=cdn_rate_proto_base.Ccdn_rate_proto_base
except:
	base_type=object
	pass


class Ccdn_rate_proto (base_type):
	def __init__(self,ip,port):
		cdn_rate_proto_base.Ccdn_rate_proto_base.__init__(self,ip,port)

	def cdn_report_ip(self ,prjid,ip,logtime,value,count ):
		"""
上报相同ip的分钟表数据:
	/* 下载次数 */
	#类型:uint32
	public count;


	"""
		pri_in=cdn_report_ip_in();

		pri_in.prjid=prjid
		pri_in.ip=ip
		pri_in.logtime=logtime
		pri_in.value=value
		pri_in.count=count
		self.sendmsg(0x1001, pri_in );
		pass

	def cdn_report_prov(self ,prjid,provid,cityid,comp,logtime,value,count ):
		"""
按省市运营商上报cdn速度:
	/* 下载次数 */
	#类型:uint32
	public count;


	"""
		pri_in=cdn_report_prov_in();

		pri_in.prjid=prjid
		pri_in.provid=provid
		pri_in.cityid=cityid
		pri_in.comp=comp
		pri_in.logtime=logtime
		pri_in.value=value
		pri_in.count=count
		self.sendmsg(0x1002, pri_in );
		pass

	def net_stat_report_ip_delay(self ,idcid,ip,logtime,value,count ):
		"""
按ip上报全网测速的时延:
	/* 探测的次数 */
	#类型:uint32
	public count;


	"""
		pri_in=net_stat_report_ip_delay_in();

		pri_in.idcid=idcid
		pri_in.ip=ip
		pri_in.logtime=logtime
		pri_in.value=value
		pri_in.count=count
		self.sendmsg(0x1003, pri_in );
		pass

	def net_stat_report_prov_delay(self ,idcid,provid,cityid,comp,logtime,value,count ):
		"""
:
	/* 一分钟内探测次数 */
	#类型:uint32
	public count;


	"""
		pri_in=net_stat_report_prov_delay_in();

		pri_in.idcid=idcid
		pri_in.provid=provid
		pri_in.cityid=cityid
		pri_in.comp=comp
		pri_in.logtime=logtime
		pri_in.value=value
		pri_in.count=count
		self.sendmsg(0x1004, pri_in );
		pass

	def net_stat_report_ip_lost(self ,idcid,ip,logtime,value,count ):
		"""
:
	/* 探测次数 */
	#类型:uint32
	public count;


	"""
		pri_in=net_stat_report_ip_lost_in();

		pri_in.idcid=idcid
		pri_in.ip=ip
		pri_in.logtime=logtime
		pri_in.value=value
		pri_in.count=count
		self.sendmsg(0x1005, pri_in );
		pass

	def net_stat_report_prov_lost(self ,idcid,provid,cityid,comp,logtime,value,count ):
		"""
:
	/* 一分钟内探测次数 */
	#类型:uint32
	public count;


	"""
		pri_in=net_stat_report_prov_lost_in();

		pri_in.idcid=idcid
		pri_in.provid=provid
		pri_in.cityid=cityid
		pri_in.comp=comp
		pri_in.logtime=logtime
		pri_in.value=value
		pri_in.count=count
		self.sendmsg(0x1006, pri_in );
		pass

	def net_stat_report_ip_hop(self ,idcid,ip,logtime,value,count ):
		"""
:
	/* 一分钟内探测次数 */
	#类型:uint32
	public count;


	"""
		pri_in=net_stat_report_ip_hop_in();

		pri_in.idcid=idcid
		pri_in.ip=ip
		pri_in.logtime=logtime
		pri_in.value=value
		pri_in.count=count
		self.sendmsg(0x1007, pri_in );
		pass

	def net_stat_report_prov_hop(self ,idcid,provid,cityid,comp,logtime,value,count ):
		"""
:
	/*  */
	#类型:uint32
	public count;


	"""
		pri_in=net_stat_report_prov_hop_in();

		pri_in.idcid=idcid
		pri_in.provid=provid
		pri_in.cityid=cityid
		pri_in.comp=comp
		pri_in.logtime=logtime
		pri_in.value=value
		pri_in.count=count
		self.sendmsg(0x1008, pri_in );
		pass

	def net_stat_report_link_delay(self ,link,logtime,value,count ):
		"""
单条链路的时延:
	/*  */
	#类型:uint32
	public count;


	"""
		pri_in=net_stat_report_link_delay_in();

		pri_in.link=link
		pri_in.logtime=logtime
		pri_in.value=value
		pri_in.count=count
		self.sendmsg(0x1009, pri_in );
		pass

	def net_stat_report_link_lost(self ,link,logtime,value,count ):
		"""
:
	/*  */
	#类型:uint32
	public count;


	"""
		pri_in=net_stat_report_link_lost_in();

		pri_in.link=link
		pri_in.logtime=logtime
		pri_in.value=value
		pri_in.count=count
		self.sendmsg(0x1010, pri_in );
		pass

	def net_stat_report_link_hop(self ,link,logtime,value,count ):
		"""
:
	/*  */
	#类型:uint32
	public count;


	"""
		pri_in=net_stat_report_link_hop_in();

		pri_in.link=link
		pri_in.logtime=logtime
		pri_in.value=value
		pri_in.count=count
		self.sendmsg(0x1011, pri_in );
		pass

	def net_stat_report_idc_delay(self ,idc,logtime,value,count ):
		"""
:
	/*  */
	#类型:uint32
	public count;


	"""
		pri_in=net_stat_report_idc_delay_in();

		pri_in.idc=idc
		pri_in.logtime=logtime
		pri_in.value=value
		pri_in.count=count
		self.sendmsg(0x1012, pri_in );
		pass

	def net_stat_report_idc_lost(self ,idc,logtime,value,count ):
		"""
:
	/*  */
	#类型:uint32
	public count;


	"""
		pri_in=net_stat_report_idc_lost_in();

		pri_in.idc=idc
		pri_in.logtime=logtime
		pri_in.value=value
		pri_in.count=count
		self.sendmsg(0x1013, pri_in );
		pass

	def net_stat_report_idc_hop(self ,idc,logtime,value,count ):
		"""
:
	/*  */
	#类型:uint32
	public count;


	"""
		pri_in=net_stat_report_idc_hop_in();

		pri_in.idc=idc
		pri_in.logtime=logtime
		pri_in.value=value
		pri_in.count=count
		self.sendmsg(0x1014, pri_in );
		pass

	def net_stat_report_all_idc_delay(self ,logtime,value,count ):
		"""
:
	/*  */
	#类型:uint32
	public count;


	"""
		pri_in=net_stat_report_all_idc_delay_in();

		pri_in.logtime=logtime
		pri_in.value=value
		pri_in.count=count
		self.sendmsg(0x1015, pri_in );
		pass

	def net_stat_report_all_idc_lost(self ,logtime,value,count ):
		"""
:
	/*  */
	#类型:uint32
	public count;


	"""
		pri_in=net_stat_report_all_idc_lost_in();

		pri_in.logtime=logtime
		pri_in.value=value
		pri_in.count=count
		self.sendmsg(0x1016, pri_in );
		pass

	def net_stat_report_all_idc_hop(self ,logtime,value,count ):
		"""
:
	/*  */
	#类型:uint32
	public count;


	"""
		pri_in=net_stat_report_all_idc_hop_in();

		pri_in.logtime=logtime
		pri_in.value=value
		pri_in.count=count
		self.sendmsg(0x1017, pri_in );
		pass

	def url_stat_report_point_time(self ,prjid,pageid,provid,cityid,ISPID,comp,logtime,repot_cnt,vlist ):
		"""
:
	/**/
	#变长数组,最大长度:20, 类型:uint32_t
	public vlist =array();


	"""
		pri_in=url_stat_report_point_time_in();

		pri_in.prjid=prjid
		pri_in.pageid=pageid
		pri_in.provid=provid
		pri_in.cityid=cityid
		pri_in.ISPID=ISPID
		pri_in.comp=comp
		pri_in.logtime=logtime
		pri_in.repot_cnt=repot_cnt
		pri_in.vlist=vlist
		self.sendmsg(0x1018, pri_in );
		pass

	def url_rate_record(self ,prjid,userid,clientip,time,pageid,vlist ):
		"""
:
	/*起点到每个点的耗时，最多10个点*/
	#变长数组,最大长度:20, 类型:uint32_t
	public vlist =array();


	"""
		pri_in=url_rate_record_in();

		pri_in.prjid=prjid
		pri_in.userid=userid
		pri_in.clientip=clientip
		pri_in.time=time
		pri_in.pageid=pageid
		pri_in.vlist=vlist
		self.sendmsg(0x1019, pri_in );
		pass

	def cdnrate_ip_isp(self ,prjid,logtime,cdnip,provid,cityid,comp,value,count ):
		"""
:
	/* 下载次数 */
	#类型:uint32
	public count;


	"""
		pri_in=cdnrate_ip_isp_in();

		pri_in.prjid=prjid
		pri_in.logtime=logtime
		pri_in.cdnip=cdnip
		pri_in.provid=provid
		pri_in.cityid=cityid
		pri_in.comp=comp
		pri_in.value=value
		pri_in.count=count
		self.sendmsg(0x1201, pri_in );
		pass

	def net_stat_ip_report(self ,idcid,ip,logtime,delay_v,lost_v,hop_v,count ):
		"""
该协议替换1003,1005,1007:
	/* 探测次数 */
	#类型:uint32
	public count;


	"""
		pri_in=net_stat_ip_report_in();

		pri_in.idcid=idcid
		pri_in.ip=ip
		pri_in.logtime=logtime
		pri_in.delay_v=delay_v
		pri_in.lost_v=lost_v
		pri_in.hop_v=hop_v
		pri_in.count=count
		self.sendmsg(0x1211, pri_in );
		pass

	def net_stat_prov_report(self ,idcid,provid,cityid,comp,logtime,delay_v,lost_v,hop_v,count ):
		"""
该协议替换1004,1006,1008:
	/* 探测次数 */
	#类型:uint32
	public count;


	"""
		pri_in=net_stat_prov_report_in();

		pri_in.idcid=idcid
		pri_in.provid=provid
		pri_in.cityid=cityid
		pri_in.comp=comp
		pri_in.logtime=logtime
		pri_in.delay_v=delay_v
		pri_in.lost_v=lost_v
		pri_in.hop_v=hop_v
		pri_in.count=count
		self.sendmsg(0x1212, pri_in );
		pass

	def net_stat_link_report(self ,link,logtime,delay_v,lost_v,hop_v,count ):
		"""
该协议替换1009,1010,1011:
	/* 探测次数 */
	#类型:uint32
	public count;


	"""
		pri_in=net_stat_link_report_in();

		pri_in.link=link
		pri_in.logtime=logtime
		pri_in.delay_v=delay_v
		pri_in.lost_v=lost_v
		pri_in.hop_v=hop_v
		pri_in.count=count
		self.sendmsg(0x1213, pri_in );
		pass


