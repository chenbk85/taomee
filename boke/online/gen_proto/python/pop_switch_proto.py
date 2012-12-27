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

		
class sw_get_ranged_svrlist_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public start_id;

	/*  */
	#类型:uint32
	public end_id;

	/**/
	#变长数组,最大长度:1000, 类型:uint32_t
	public friend_list =array();


	"""

	def read_from_buf(self, ba ):
		self.start_id=ba.read_uint32();
		if ( self.start_id== None): return False;
		self.end_id=ba.read_uint32();
		if ( self.end_id== None): return False;

		friend_list_count= ba.read_uint32();
		if ( friend_list_count==None ): return False;
		self.friend_list=[];
		for i in range(friend_list_count):
			self.friend_list.append(ba.read_uint32());
			if ( self.friend_list[i]== None): return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.start_id)):return False;
		if ( not ba.write_uint32(self.end_id)):return False;
		friend_list_count=len(self.friend_list);
		ba.write_uint32(friend_list_count);
		for i in range(friend_list_count):
			if ( not ba.write_uint32(self.friend_list[i])):return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[start_id]=%s"%(str(self.start_id))
		print tabs+"""/*  */"""
		print tabs+"[end_id]=%s"%(str(self.end_id))
		print tabs+"#"
		print tabs+"[friend_list]=>{";
		for i in range(len(self.friend_list)):
			print tabs+"    [%d]="%(i)+str(self.friend_list[i]); 

		print tabs+'}'
		
		pass;
	




class online_item_t :
	"""
参数列表:
	/*  */
	#类型:uint32
	public online_id;

	/*  */
	#类型:uint32
	public user_num;

	/*  */
	#定长数组,长度:16, 类型:char 
	public online_ip ;

	/*  */
	#类型:uint16
	public online_port;

	/*  */
	#类型:uint32
	public friend_count;


	"""

	def read_from_buf(self, ba ):
		self.online_id=ba.read_uint32();
		if ( self.online_id== None): return False;
		self.user_num=ba.read_uint32();
		if ( self.user_num== None): return False;
		self.online_ip=ba.read_buf(16);
		if( self.online_ip==None ): return False;
		self.online_port=ba.read_uint16();
		if ( self.online_port== None): return False;
		self.friend_count=ba.read_uint32();
		if ( self.friend_count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.online_id)):return False;
		if ( not ba.write_uint32(self.user_num)):return False;
		ba.write_buf(self.online_ip,16);
		if ( not ba.write_uint16(self.online_port)):return False;
		if ( not ba.write_uint32(self.friend_count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[online_id]=%s"%(str(self.online_id))
		print tabs+"""/*  */"""
		print tabs+"[user_num]=%s"%(str(self.user_num))
		print tabs+"""/*  */"""
		tmp_str=str(self.online_ip);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[online_ip]=%s"%(tmp_str)
		print tabs+"""/*  */"""
		print tabs+"[online_port]=%s"%(str(self.online_port))
		print tabs+"""/*  */"""
		print tabs+"[friend_count]=%s"%(str(self.friend_count))
		
		pass;
	




class sw_get_ranged_svrlist_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public max_online_id;

	/**/
	#变长数组,最大长度:1000, 类型:online_item_t
	public online_list =array();


	"""

	def read_from_buf(self, ba ):
		self.max_online_id=ba.read_uint32();
		if ( self.max_online_id== None): return False;

		online_list_count= ba.read_uint32();
		if ( online_list_count==None ): return False;
		self.online_list=[];
		for i in range(online_list_count):
			self.online_list.append(online_item_t());
			if ( not self.online_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.max_online_id)):return False;
		online_list_count=len(self.online_list);
		ba.write_uint32(online_list_count);
		for i in range(online_list_count):
			if (  not  isinstance(self.online_list[i] ,online_item_t) ):return False; 
			if ( not self.online_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[max_online_id]=%s"%(str(self.max_online_id))
		print tabs+"#"
		print tabs+"[online_list]=>{";
		for i in range(len(self.online_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.online_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class sw_get_recommend_svr_list_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public max_online_id;

	/**/
	#变长数组,最大长度:1000, 类型:online_item_t
	public online_list =array();


	"""

	def read_from_buf(self, ba ):
		self.max_online_id=ba.read_uint32();
		if ( self.max_online_id== None): return False;

		online_list_count= ba.read_uint32();
		if ( online_list_count==None ): return False;
		self.online_list=[];
		for i in range(online_list_count):
			self.online_list.append(online_item_t());
			if ( not self.online_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.max_online_id)):return False;
		online_list_count=len(self.online_list);
		ba.write_uint32(online_list_count);
		for i in range(online_list_count):
			if (  not  isinstance(self.online_list[i] ,online_item_t) ):return False; 
			if ( not self.online_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[max_online_id]=%s"%(str(self.max_online_id))
		print tabs+"#"
		print tabs+"[online_list]=>{";
		for i in range(len(self.online_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.online_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class sw_get_user_count_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public online_count;


	"""

	def read_from_buf(self, ba ):
		self.online_count=ba.read_uint32();
		if ( self.online_count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.online_count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[online_count]=%s"%(str(self.online_count))
		
		pass;
	




class sw_noti_across_svr_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public cmdid;

	/**/
	#变长数组,最大长度:999, 类型:char
	public buflist ;


	"""

	def read_from_buf(self, ba ):
		self.cmdid=ba.read_uint32();
		if ( self.cmdid== None): return False;

		buflist_count= ba.read_uint32();
		if ( buflist_count==None ): return False;
		self.buflist=ba.read_buf(buflist_count);
		if( self.buflist==None ):return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.cmdid)):return False;
		buflist_count=len(self.buflist);
		ba.write_uint32(buflist_count);
		ba.write_buf(self.buflist,buflist_count);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[cmdid]=%s"%(str(self.cmdid))
		print tabs+"""/*  */"""
		print tabs+"[buflist]=%s"%(str(self.buflist))
		
		pass;
	




class sw_noti_across_svr_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public cmdid;

	/**/
	#变长数组,最大长度:999, 类型:char
	public buflist ;


	"""

	def read_from_buf(self, ba ):
		self.cmdid=ba.read_uint32();
		if ( self.cmdid== None): return False;

		buflist_count= ba.read_uint32();
		if ( buflist_count==None ): return False;
		self.buflist=ba.read_buf(buflist_count);
		if( self.buflist==None ):return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.cmdid)):return False;
		buflist_count=len(self.buflist);
		ba.write_uint32(buflist_count);
		ba.write_buf(self.buflist,buflist_count);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[cmdid]=%s"%(str(self.cmdid))
		print tabs+"""/*  */"""
		print tabs+"[buflist]=%s"%(str(self.buflist))
		
		pass;
	




class sw_report_online_info_in :
	"""
参数列表:
	/*  */
	#类型:uint16
	public domain_id;

	/*  */
	#类型:uint32
	public online_id;

	/*  */
	#定长数组,长度:16, 类型:char 
	public online_name ;

	/*  */
	#定长数组,长度:16, 类型:uint16_t 
	public online_ip =array();

	/*  */
	#类型:uint16
	public online_port;

	/*  */
	#类型:uint32
	public user_num;

	/*  */
	#类型:uint32
	public seqno;

	/*在线的用户列表*/
	#变长数组,最大长度:999999, 类型:uint32_t
	public userid_list =array();


	"""

	def read_from_buf(self, ba ):
		self.domain_id=ba.read_uint16();
		if ( self.domain_id== None): return False;
		self.online_id=ba.read_uint32();
		if ( self.online_id== None): return False;
		self.online_name=ba.read_buf(16);
		if( self.online_name==None ): return False;
		self.online_ip=[];
		for i in range(16):
			self.online_ip.append(ba.read_uint16());
			if ( self.online_ip[i]== None): return False;

		self.online_port=ba.read_uint16();
		if ( self.online_port== None): return False;
		self.user_num=ba.read_uint32();
		if ( self.user_num== None): return False;
		self.seqno=ba.read_uint32();
		if ( self.seqno== None): return False;

		userid_list_count= ba.read_uint32();
		if ( userid_list_count==None ): return False;
		self.userid_list=[];
		for i in range(userid_list_count):
			self.userid_list.append(ba.read_uint32());
			if ( self.userid_list[i]== None): return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint16(self.domain_id)):return False;
		if ( not ba.write_uint32(self.online_id)):return False;
		ba.write_buf(self.online_name,16);
		if(len(self.online_ip)<>16):return False; 
		for i in range(16):
			if ( not ba.write_uint16(self.online_ip[i])):return False;

		if ( not ba.write_uint16(self.online_port)):return False;
		if ( not ba.write_uint32(self.user_num)):return False;
		if ( not ba.write_uint32(self.seqno)):return False;
		userid_list_count=len(self.userid_list);
		ba.write_uint32(userid_list_count);
		for i in range(userid_list_count):
			if ( not ba.write_uint32(self.userid_list[i])):return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[domain_id]=%s"%(str(self.domain_id))
		print tabs+"""/*  */"""
		print tabs+"[online_id]=%s"%(str(self.online_id))
		print tabs+"""/*  */"""
		tmp_str=str(self.online_name);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[online_name]=%s"%(tmp_str)
		print tabs+"""#"""
		print tabs+"[online_ip]=>{";
		for i in range(16):
			print tabs+"    [%d]="%(i)+str(self.online_ip[i]); 

		print tabs+"}"
		print tabs+"""/*  */"""
		print tabs+"[online_port]=%s"%(str(self.online_port))
		print tabs+"""/*  */"""
		print tabs+"[user_num]=%s"%(str(self.user_num))
		print tabs+"""/*  */"""
		print tabs+"[seqno]=%s"%(str(self.seqno))
		print tabs+"#在线的用户列表"
		print tabs+"[userid_list]=>{";
		for i in range(len(self.userid_list)):
			print tabs+"    [%d]="%(i)+str(self.userid_list[i]); 

		print tabs+'}'
		
		pass;
	




class sw_report_user_onoff_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public is_on_online;


	"""

	def read_from_buf(self, ba ):
		self.is_on_online=ba.read_uint32();
		if ( self.is_on_online== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.is_on_online)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[is_on_online]=%s"%(str(self.is_on_online))
		
		pass;
	




class find_map_t :
	"""
参数列表:
	/*  */
	#类型:uint32
	public islandid;

	/*  */
	#类型:uint32
	public mapid;


	"""

	def read_from_buf(self, ba ):
		self.islandid=ba.read_uint32();
		if ( self.islandid== None): return False;
		self.mapid=ba.read_uint32();
		if ( self.mapid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.islandid)):return False;
		if ( not ba.write_uint32(self.mapid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[islandid]=%s"%(str(self.islandid))
		print tabs+"""/*  */"""
		print tabs+"[mapid]=%s"%(str(self.mapid))
		
		pass;
	




class game_info_t :
	"""
参数列表:
	/* 项目编号 */
	#类型:uint32
	public gameid;

	/* 胜利次数 */
	#类型:uint32
	public win_time;

	/* 输的次数 */
	#类型:uint32
	public lose_time;

	/* 平的次数 */
	#类型:uint32
	public draw_time;


	"""

	def read_from_buf(self, ba ):
		self.gameid=ba.read_uint32();
		if ( self.gameid== None): return False;
		self.win_time=ba.read_uint32();
		if ( self.win_time== None): return False;
		self.lose_time=ba.read_uint32();
		if ( self.lose_time== None): return False;
		self.draw_time=ba.read_uint32();
		if ( self.draw_time== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.gameid)):return False;
		if ( not ba.write_uint32(self.win_time)):return False;
		if ( not ba.write_uint32(self.lose_time)):return False;
		if ( not ba.write_uint32(self.draw_time)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 项目编号 */"""
		print tabs+"[gameid]=%s"%(str(self.gameid))
		print tabs+"""/* 胜利次数 */"""
		print tabs+"[win_time]=%s"%(str(self.win_time))
		print tabs+"""/* 输的次数 */"""
		print tabs+"[lose_time]=%s"%(str(self.lose_time))
		print tabs+"""/* 平的次数 */"""
		print tabs+"[draw_time]=%s"%(str(self.draw_time))
		
		pass;
	




class item_day_limit_t :
	"""
参数列表:
	/* 物品ID（3战斗积分4在线时长） */
	#类型:uint32
	public itemid;

	/* 总数 */
	#类型:uint32
	public total;

	/* 当日获得次数 */
	#类型:uint32
	public daycnt;


	"""

	def read_from_buf(self, ba ):
		self.itemid=ba.read_uint32();
		if ( self.itemid== None): return False;
		self.total=ba.read_uint32();
		if ( self.total== None): return False;
		self.daycnt=ba.read_uint32();
		if ( self.daycnt== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.itemid)):return False;
		if ( not ba.write_uint32(self.total)):return False;
		if ( not ba.write_uint32(self.daycnt)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 物品ID（3战斗积分4在线时长） */"""
		print tabs+"[itemid]=%s"%(str(self.itemid))
		print tabs+"""/* 总数 */"""
		print tabs+"[total]=%s"%(str(self.total))
		print tabs+"""/* 当日获得次数 */"""
		print tabs+"[daycnt]=%s"%(str(self.daycnt))
		
		pass;
	




class item_exchange_t :
	"""
参数列表:
	/* 1:任务，2:xiaomee */
	#类型:uint32
	public itemid;

	/*  */
	#类型:uint32
	public count;

	/*  */
	#类型:uint32
	public max_count;

	/* 今日增加数量 */
	#类型:uint32
	public day_add_cnt;


	"""

	def read_from_buf(self, ba ):
		self.itemid=ba.read_uint32();
		if ( self.itemid== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;
		self.max_count=ba.read_uint32();
		if ( self.max_count== None): return False;
		self.day_add_cnt=ba.read_uint32();
		if ( self.day_add_cnt== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.itemid)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		if ( not ba.write_uint32(self.max_count)):return False;
		if ( not ba.write_uint32(self.day_add_cnt)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 1:任务，2:xiaomee */"""
		print tabs+"[itemid]=%s"%(str(self.itemid))
		print tabs+"""/*  */"""
		print tabs+"[count]=%s"%(str(self.count))
		print tabs+"""/*  */"""
		print tabs+"[max_count]=%s"%(str(self.max_count))
		print tabs+"""/* 今日增加数量 */"""
		print tabs+"[day_add_cnt]=%s"%(str(self.day_add_cnt))
		
		pass;
	




class item_ex_t :
	"""
参数列表:
	/*  */
	#类型:uint32
	public itemid;

	/*  */
	#类型:uint32
	public use_count;

	/*  */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.itemid=ba.read_uint32();
		if ( self.itemid== None): return False;
		self.use_count=ba.read_uint32();
		if ( self.use_count== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.itemid)):return False;
		if ( not ba.write_uint32(self.use_count)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[itemid]=%s"%(str(self.itemid))
		print tabs+"""/*  */"""
		print tabs+"[use_count]=%s"%(str(self.use_count))
		print tabs+"""/*  */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class item_t :
	"""
参数列表:
	/*  */
	#类型:uint32
	public itemid;

	/*  */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.itemid=ba.read_uint32();
		if ( self.itemid== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.itemid)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[itemid]=%s"%(str(self.itemid))
		print tabs+"""/*  */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class task_t :
	"""
参数列表:
	/*  */
	#类型:uint32
	public taskid;

	/*  */
	#类型:uint32
	public task_nodeid;


	"""

	def read_from_buf(self, ba ):
		self.taskid=ba.read_uint32();
		if ( self.taskid== None): return False;
		self.task_nodeid=ba.read_uint32();
		if ( self.task_nodeid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.taskid)):return False;
		if ( not ba.write_uint32(self.task_nodeid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[taskid]=%s"%(str(self.taskid))
		print tabs+"""/*  */"""
		print tabs+"[task_nodeid]=%s"%(str(self.task_nodeid))
		
		pass;
	




class user_log_t :
	"""
参数列表:
	/* 记录时间 */
	#类型:uint32
	public logtime;

	/* 1:注册 ，2：完成岛:(v2:岛id) */
	#类型:uint32
	public v1;

	/*  */
	#类型:uint32
	public v2;


	"""

	def read_from_buf(self, ba ):
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.v1=ba.read_uint32();
		if ( self.v1== None): return False;
		self.v2=ba.read_uint32();
		if ( self.v2== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.v1)):return False;
		if ( not ba.write_uint32(self.v2)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 记录时间 */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/* 1:注册 ，2：完成岛:(v2:岛id) */"""
		print tabs+"[v1]=%s"%(str(self.v1))
		print tabs+"""/*  */"""
		print tabs+"[v2]=%s"%(str(self.v2))
		
		pass;
	




create_time='2012年12月27日 星期4 12:04:27'
cmd_map={ }
cmd_map[61001]=(61001,'sw_get_user_count',None,sw_get_user_count_out,0x945b505b)
cmd_map[62001]=(62001,'sw_report_online_info',sw_report_online_info_in,None,0x9d002133)
cmd_map[62002]=(62002,'sw_report_user_onoff',sw_report_user_onoff_in,None,0xa23fe386)
cmd_map[62003]=(62003,'sw_set_user_offline',None,None,0xe2948cb2)
cmd_map[62004]=(62004,'sw_noti_across_svr',sw_noti_across_svr_in,sw_noti_across_svr_out,0xf55ca111)
cmd_map[62006]=(62006,'sw_get_recommend_svr_list',None,sw_get_recommend_svr_list_out,0x4940b505)
cmd_map[64002]=(64002,'sw_get_ranged_svrlist',sw_get_ranged_svrlist_in,sw_get_ranged_svrlist_out,0xe107fe35)
	
def init_cmd_map():
	pass

try:
	import pop_switch_proto_base
	base_type=pop_switch_proto_base.Cpop_switch_proto_base
except:
	base_type=object
	pass


class Cpop_switch_proto (base_type):
	def __init__(self,ip,port):
		pop_switch_proto_base.Cpop_switch_proto_base.__init__(self,ip,port)

	def sw_get_user_count(self  ):
		"""
得到在线人数:

	"""
		pri_in=None;

		self.sendmsg(61001, pri_in );
		pass

	def sw_report_online_info(self ,domain_id,online_id,online_name,online_ip,online_port,user_num,seqno,userid_list ):
		"""
online 上报启动信息:
	/*在线的用户列表*/
	#变长数组,最大长度:999999, 类型:uint32_t
	public userid_list =array();


	"""
		pri_in=sw_report_online_info_in();

		pri_in.domain_id=domain_id
		pri_in.online_id=online_id
		pri_in.online_name=online_name
		pri_in.online_ip=online_ip
		pri_in.online_port=online_port
		pri_in.user_num=user_num
		pri_in.seqno=seqno
		pri_in.userid_list=userid_list
		self.sendmsg(62001, pri_in );
		pass

	def sw_report_user_onoff(self ,is_on_online ):
		"""
上报用户上下线:
	/*  */
	#类型:uint32
	public is_on_online;


	"""
		pri_in=sw_report_user_onoff_in();

		pri_in.is_on_online=is_on_online
		self.sendmsg(62002, pri_in );
		pass

	def sw_set_user_offline(self  ):
		"""
请求用户离线:

	"""
		pri_in=None;

		self.sendmsg(62003, pri_in );
		pass

	def sw_noti_across_svr(self ,cmdid,buflist ):
		"""
:
	/**/
	#变长数组,最大长度:999, 类型:char
	public buflist ;


	"""
		pri_in=sw_noti_across_svr_in();

		pri_in.cmdid=cmdid
		pri_in.buflist=buflist
		self.sendmsg(62004, pri_in );
		pass

	def sw_get_recommend_svr_list(self  ):
		"""
得到推荐服务器列表:

	"""
		pri_in=None;

		self.sendmsg(62006, pri_in );
		pass

	def sw_get_ranged_svrlist(self ,start_id,end_id,friend_list ):
		"""
得到范围列表:
	/**/
	#变长数组,最大长度:1000, 类型:uint32_t
	public friend_list =array();


	"""
		pri_in=sw_get_ranged_svrlist_in();

		pri_in.start_id=start_id
		pri_in.end_id=end_id
		pri_in.friend_list=friend_list
		self.sendmsg(64002, pri_in );
		pass


