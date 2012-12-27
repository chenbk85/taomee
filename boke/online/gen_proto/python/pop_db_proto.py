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

		
class get_server_version_out :
	"""
参数列表:
	/*  */
	#定长数组,长度:255, 类型:char 
	public version ;


	"""

	def read_from_buf(self, ba ):
		self.version=ba.read_buf(255);
		if( self.version==None ): return False;

		return True;


	def write_to_buf(self,ba):
		ba.write_buf(self.version,255);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		tmp_str=str(self.version);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[version]=%s"%(tmp_str)
		
		pass;
	




class pop_add_game_info_in :
	"""
参数列表:
	/* 项目编号 */
	#类型:uint32
	public gameid;

	/* 胜负标志0：负 1：胜 2：平 */
	#类型:uint32
	public win_flag;


	"""

	def read_from_buf(self, ba ):
		self.gameid=ba.read_uint32();
		if ( self.gameid== None): return False;
		self.win_flag=ba.read_uint32();
		if ( self.win_flag== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.gameid)):return False;
		if ( not ba.write_uint32(self.win_flag)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 项目编号 */"""
		print tabs+"[gameid]=%s"%(str(self.gameid))
		print tabs+"""/* 胜负标志0：负 1：胜 2：平 */"""
		print tabs+"[win_flag]=%s"%(str(self.win_flag))
		
		pass;
	




class pop_copy_user_in :
	"""
参数列表:
	/* 目标userid */
	#类型:uint32
	public dsc_userid;


	"""

	def read_from_buf(self, ba ):
		self.dsc_userid=ba.read_uint32();
		if ( self.dsc_userid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.dsc_userid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 目标userid */"""
		print tabs+"[dsc_userid]=%s"%(str(self.dsc_userid))
		
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
	




class pop_exchange_in :
	"""
参数列表:
	/* 未使用 */
	#类型:uint32
	public opt_flag;

	/**/
	#变长数组,最大长度:9999, 类型:item_exchange_t
	public add_item_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:item_exchange_t
	public del_item_list =array();


	"""

	def read_from_buf(self, ba ):
		self.opt_flag=ba.read_uint32();
		if ( self.opt_flag== None): return False;

		add_item_list_count= ba.read_uint32();
		if ( add_item_list_count==None ): return False;
		self.add_item_list=[];
		for i in range(add_item_list_count):
			self.add_item_list.append(item_exchange_t());
			if ( not self.add_item_list[i].read_from_buf(ba)):return False;


		del_item_list_count= ba.read_uint32();
		if ( del_item_list_count==None ): return False;
		self.del_item_list=[];
		for i in range(del_item_list_count):
			self.del_item_list.append(item_exchange_t());
			if ( not self.del_item_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.opt_flag)):return False;
		add_item_list_count=len(self.add_item_list);
		ba.write_uint32(add_item_list_count);
		for i in range(add_item_list_count):
			if (  not  isinstance(self.add_item_list[i] ,item_exchange_t) ):return False; 
			if ( not self.add_item_list[i].write_to_buf(ba)): return False;

		del_item_list_count=len(self.del_item_list);
		ba.write_uint32(del_item_list_count);
		for i in range(del_item_list_count):
			if (  not  isinstance(self.del_item_list[i] ,item_exchange_t) ):return False; 
			if ( not self.del_item_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 未使用 */"""
		print tabs+"[opt_flag]=%s"%(str(self.opt_flag))
		print tabs+"#"
		print tabs+"[add_item_list]=>{";
		for i in range(len(self.add_item_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.add_item_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		print tabs+"#"
		print tabs+"[del_item_list]=>{";
		for i in range(len(self.del_item_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.del_item_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class pop_find_map_add_in :
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
	




class user_base_info_t :
	"""
参数列表:
	/*  */
	#类型:uint32
	public register_time;

	/* 0x01:用户是否注册过 */
	#类型:uint32
	public flag;

	/*  */
	#类型:uint32
	public xiaomee;

	/*  */
	#类型:uint32
	public color;

	/*  */
	#类型:uint32
	public age;

	/*  */
	#定长数组,长度:16, 类型:char 
	public nick ;

	/*  */
	#类型:uint32
	public last_islandid;

	/*  */
	#类型:uint32
	public last_mapid;

	/*  */
	#类型:uint32
	public last_x;

	/*  */
	#类型:uint32
	public last_y;

	/*  */
	#类型:uint32
	public last_login;

	/*  */
	#类型:uint32
	public online_time;


	"""

	def read_from_buf(self, ba ):
		self.register_time=ba.read_uint32();
		if ( self.register_time== None): return False;
		self.flag=ba.read_uint32();
		if ( self.flag== None): return False;
		self.xiaomee=ba.read_uint32();
		if ( self.xiaomee== None): return False;
		self.color=ba.read_uint32();
		if ( self.color== None): return False;
		self.age=ba.read_uint32();
		if ( self.age== None): return False;
		self.nick=ba.read_buf(16);
		if( self.nick==None ): return False;
		self.last_islandid=ba.read_uint32();
		if ( self.last_islandid== None): return False;
		self.last_mapid=ba.read_uint32();
		if ( self.last_mapid== None): return False;
		self.last_x=ba.read_uint32();
		if ( self.last_x== None): return False;
		self.last_y=ba.read_uint32();
		if ( self.last_y== None): return False;
		self.last_login=ba.read_uint32();
		if ( self.last_login== None): return False;
		self.online_time=ba.read_uint32();
		if ( self.online_time== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.register_time)):return False;
		if ( not ba.write_uint32(self.flag)):return False;
		if ( not ba.write_uint32(self.xiaomee)):return False;
		if ( not ba.write_uint32(self.color)):return False;
		if ( not ba.write_uint32(self.age)):return False;
		ba.write_buf(self.nick,16);
		if ( not ba.write_uint32(self.last_islandid)):return False;
		if ( not ba.write_uint32(self.last_mapid)):return False;
		if ( not ba.write_uint32(self.last_x)):return False;
		if ( not ba.write_uint32(self.last_y)):return False;
		if ( not ba.write_uint32(self.last_login)):return False;
		if ( not ba.write_uint32(self.online_time)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[register_time]=%s"%(str(self.register_time))
		print tabs+"""/* 0x01:用户是否注册过 */"""
		print tabs+"[flag]=%s"%(str(self.flag))
		print tabs+"""/*  */"""
		print tabs+"[xiaomee]=%s"%(str(self.xiaomee))
		print tabs+"""/*  */"""
		print tabs+"[color]=%s"%(str(self.color))
		print tabs+"""/*  */"""
		print tabs+"[age]=%s"%(str(self.age))
		print tabs+"""/*  */"""
		tmp_str=str(self.nick);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[nick]=%s"%(tmp_str)
		print tabs+"""/*  */"""
		print tabs+"[last_islandid]=%s"%(str(self.last_islandid))
		print tabs+"""/*  */"""
		print tabs+"[last_mapid]=%s"%(str(self.last_mapid))
		print tabs+"""/*  */"""
		print tabs+"[last_x]=%s"%(str(self.last_x))
		print tabs+"""/*  */"""
		print tabs+"[last_y]=%s"%(str(self.last_y))
		print tabs+"""/*  */"""
		print tabs+"[last_login]=%s"%(str(self.last_login))
		print tabs+"""/*  */"""
		print tabs+"[online_time]=%s"%(str(self.online_time))
		
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
	




class pop_get_all_info_out :
	"""
参数列表:
	/*  */
	#类型:user_base_info_t
	public user_base_info;

	/**/
	#变长数组,最大长度:9999, 类型:item_ex_t
	public item_list =array();

	/*每日获取物品列表*/
	#变长数组,最大长度:9999, 类型:item_day_limit_t
	public item_day_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:task_t
	public task_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:find_map_t
	public find_map_list =array();

	/*用户足迹列表*/
	#变长数组,最大长度:9999, 类型:user_log_t
	public user_log_list =array();

	/*小游戏统计信息*/
	#变长数组,最大长度:9999, 类型:game_info_t
	public game_info_list =array();


	"""

	def read_from_buf(self, ba ):
		self.user_base_info=user_base_info_t();
		if ( not self.user_base_info.read_from_buf(ba)) :return False;

		item_list_count= ba.read_uint32();
		if ( item_list_count==None ): return False;
		self.item_list=[];
		for i in range(item_list_count):
			self.item_list.append(item_ex_t());
			if ( not self.item_list[i].read_from_buf(ba)):return False;


		item_day_list_count= ba.read_uint32();
		if ( item_day_list_count==None ): return False;
		self.item_day_list=[];
		for i in range(item_day_list_count):
			self.item_day_list.append(item_day_limit_t());
			if ( not self.item_day_list[i].read_from_buf(ba)):return False;


		task_list_count= ba.read_uint32();
		if ( task_list_count==None ): return False;
		self.task_list=[];
		for i in range(task_list_count):
			self.task_list.append(task_t());
			if ( not self.task_list[i].read_from_buf(ba)):return False;


		find_map_list_count= ba.read_uint32();
		if ( find_map_list_count==None ): return False;
		self.find_map_list=[];
		for i in range(find_map_list_count):
			self.find_map_list.append(find_map_t());
			if ( not self.find_map_list[i].read_from_buf(ba)):return False;


		user_log_list_count= ba.read_uint32();
		if ( user_log_list_count==None ): return False;
		self.user_log_list=[];
		for i in range(user_log_list_count):
			self.user_log_list.append(user_log_t());
			if ( not self.user_log_list[i].read_from_buf(ba)):return False;


		game_info_list_count= ba.read_uint32();
		if ( game_info_list_count==None ): return False;
		self.game_info_list=[];
		for i in range(game_info_list_count):
			self.game_info_list.append(game_info_t());
			if ( not self.game_info_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		if ( not isinstance(self.user_base_info,user_base_info_t)): return False; 
		if ( not self.user_base_info.write_to_buf(ba)): return False;
		item_list_count=len(self.item_list);
		ba.write_uint32(item_list_count);
		for i in range(item_list_count):
			if (  not  isinstance(self.item_list[i] ,item_ex_t) ):return False; 
			if ( not self.item_list[i].write_to_buf(ba)): return False;

		item_day_list_count=len(self.item_day_list);
		ba.write_uint32(item_day_list_count);
		for i in range(item_day_list_count):
			if (  not  isinstance(self.item_day_list[i] ,item_day_limit_t) ):return False; 
			if ( not self.item_day_list[i].write_to_buf(ba)): return False;

		task_list_count=len(self.task_list);
		ba.write_uint32(task_list_count);
		for i in range(task_list_count):
			if (  not  isinstance(self.task_list[i] ,task_t) ):return False; 
			if ( not self.task_list[i].write_to_buf(ba)): return False;

		find_map_list_count=len(self.find_map_list);
		ba.write_uint32(find_map_list_count);
		for i in range(find_map_list_count):
			if (  not  isinstance(self.find_map_list[i] ,find_map_t) ):return False; 
			if ( not self.find_map_list[i].write_to_buf(ba)): return False;

		user_log_list_count=len(self.user_log_list);
		ba.write_uint32(user_log_list_count);
		for i in range(user_log_list_count):
			if (  not  isinstance(self.user_log_list[i] ,user_log_t) ):return False; 
			if ( not self.user_log_list[i].write_to_buf(ba)): return False;

		game_info_list_count=len(self.game_info_list);
		ba.write_uint32(game_info_list_count);
		for i in range(game_info_list_count):
			if (  not  isinstance(self.game_info_list[i] ,game_info_t) ):return False; 
			if ( not self.game_info_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */""";
		print tabs+"[user_base_info]=>{";
		self.user_base_info.echo(print_hex,tabs+"    ");
		print tabs+"}";
		print tabs+"#"
		print tabs+"[item_list]=>{";
		for i in range(len(self.item_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.item_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		print tabs+"#每日获取物品列表"
		print tabs+"[item_day_list]=>{";
		for i in range(len(self.item_day_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.item_day_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		print tabs+"#"
		print tabs+"[task_list]=>{";
		for i in range(len(self.task_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.task_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		print tabs+"#"
		print tabs+"[find_map_list]=>{";
		for i in range(len(self.find_map_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.find_map_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		print tabs+"#用户足迹列表"
		print tabs+"[user_log_list]=>{";
		for i in range(len(self.user_log_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.user_log_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		print tabs+"#小游戏统计信息"
		print tabs+"[game_info_list]=>{";
		for i in range(len(self.game_info_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.game_info_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class pop_get_base_info_out :
	"""
参数列表:
	/* 最后一次登录的服务器 */
	#类型:uint32
	public last_online_id;


	"""

	def read_from_buf(self, ba ):
		self.last_online_id=ba.read_uint32();
		if ( self.last_online_id== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.last_online_id)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 最后一次登录的服务器 */"""
		print tabs+"[last_online_id]=%s"%(str(self.last_online_id))
		
		pass;
	




class pop_login_out :
	"""
参数列表:
	/*  */
	#类型:user_base_info_t
	public user_base_info;

	/**/
	#变长数组,最大长度:9999, 类型:item_ex_t
	public item_list =array();

	/*每日获取物品列表*/
	#变长数组,最大长度:9999, 类型:item_day_limit_t
	public item_day_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:task_t
	public task_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:find_map_t
	public find_map_list =array();

	/*用户足迹列表*/
	#变长数组,最大长度:9999, 类型:user_log_t
	public user_log_list =array();

	/*小游戏统计信息*/
	#变长数组,最大长度:9999, 类型:game_info_t
	public game_info_list =array();


	"""

	def read_from_buf(self, ba ):
		self.user_base_info=user_base_info_t();
		if ( not self.user_base_info.read_from_buf(ba)) :return False;

		item_list_count= ba.read_uint32();
		if ( item_list_count==None ): return False;
		self.item_list=[];
		for i in range(item_list_count):
			self.item_list.append(item_ex_t());
			if ( not self.item_list[i].read_from_buf(ba)):return False;


		item_day_list_count= ba.read_uint32();
		if ( item_day_list_count==None ): return False;
		self.item_day_list=[];
		for i in range(item_day_list_count):
			self.item_day_list.append(item_day_limit_t());
			if ( not self.item_day_list[i].read_from_buf(ba)):return False;


		task_list_count= ba.read_uint32();
		if ( task_list_count==None ): return False;
		self.task_list=[];
		for i in range(task_list_count):
			self.task_list.append(task_t());
			if ( not self.task_list[i].read_from_buf(ba)):return False;


		find_map_list_count= ba.read_uint32();
		if ( find_map_list_count==None ): return False;
		self.find_map_list=[];
		for i in range(find_map_list_count):
			self.find_map_list.append(find_map_t());
			if ( not self.find_map_list[i].read_from_buf(ba)):return False;


		user_log_list_count= ba.read_uint32();
		if ( user_log_list_count==None ): return False;
		self.user_log_list=[];
		for i in range(user_log_list_count):
			self.user_log_list.append(user_log_t());
			if ( not self.user_log_list[i].read_from_buf(ba)):return False;


		game_info_list_count= ba.read_uint32();
		if ( game_info_list_count==None ): return False;
		self.game_info_list=[];
		for i in range(game_info_list_count):
			self.game_info_list.append(game_info_t());
			if ( not self.game_info_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		if ( not isinstance(self.user_base_info,user_base_info_t)): return False; 
		if ( not self.user_base_info.write_to_buf(ba)): return False;
		item_list_count=len(self.item_list);
		ba.write_uint32(item_list_count);
		for i in range(item_list_count):
			if (  not  isinstance(self.item_list[i] ,item_ex_t) ):return False; 
			if ( not self.item_list[i].write_to_buf(ba)): return False;

		item_day_list_count=len(self.item_day_list);
		ba.write_uint32(item_day_list_count);
		for i in range(item_day_list_count):
			if (  not  isinstance(self.item_day_list[i] ,item_day_limit_t) ):return False; 
			if ( not self.item_day_list[i].write_to_buf(ba)): return False;

		task_list_count=len(self.task_list);
		ba.write_uint32(task_list_count);
		for i in range(task_list_count):
			if (  not  isinstance(self.task_list[i] ,task_t) ):return False; 
			if ( not self.task_list[i].write_to_buf(ba)): return False;

		find_map_list_count=len(self.find_map_list);
		ba.write_uint32(find_map_list_count);
		for i in range(find_map_list_count):
			if (  not  isinstance(self.find_map_list[i] ,find_map_t) ):return False; 
			if ( not self.find_map_list[i].write_to_buf(ba)): return False;

		user_log_list_count=len(self.user_log_list);
		ba.write_uint32(user_log_list_count);
		for i in range(user_log_list_count):
			if (  not  isinstance(self.user_log_list[i] ,user_log_t) ):return False; 
			if ( not self.user_log_list[i].write_to_buf(ba)): return False;

		game_info_list_count=len(self.game_info_list);
		ba.write_uint32(game_info_list_count);
		for i in range(game_info_list_count):
			if (  not  isinstance(self.game_info_list[i] ,game_info_t) ):return False; 
			if ( not self.game_info_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */""";
		print tabs+"[user_base_info]=>{";
		self.user_base_info.echo(print_hex,tabs+"    ");
		print tabs+"}";
		print tabs+"#"
		print tabs+"[item_list]=>{";
		for i in range(len(self.item_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.item_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		print tabs+"#每日获取物品列表"
		print tabs+"[item_day_list]=>{";
		for i in range(len(self.item_day_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.item_day_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		print tabs+"#"
		print tabs+"[task_list]=>{";
		for i in range(len(self.task_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.task_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		print tabs+"#"
		print tabs+"[find_map_list]=>{";
		for i in range(len(self.find_map_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.find_map_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		print tabs+"#用户足迹列表"
		print tabs+"[user_log_list]=>{";
		for i in range(len(self.user_log_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.user_log_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		print tabs+"#小游戏统计信息"
		print tabs+"[game_info_list]=>{";
		for i in range(len(self.game_info_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.game_info_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class pop_logout_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public last_islandid;

	/*  */
	#类型:uint32
	public last_mapid;

	/*  */
	#类型:uint32
	public last_x;

	/*  */
	#类型:uint32
	public last_y;

	/*  */
	#类型:uint32
	public last_login;

	/*  */
	#类型:uint32
	public online_time;

	/* 最后一次登录的服务器 */
	#类型:uint32
	public last_online_id;


	"""

	def read_from_buf(self, ba ):
		self.last_islandid=ba.read_uint32();
		if ( self.last_islandid== None): return False;
		self.last_mapid=ba.read_uint32();
		if ( self.last_mapid== None): return False;
		self.last_x=ba.read_uint32();
		if ( self.last_x== None): return False;
		self.last_y=ba.read_uint32();
		if ( self.last_y== None): return False;
		self.last_login=ba.read_uint32();
		if ( self.last_login== None): return False;
		self.online_time=ba.read_uint32();
		if ( self.online_time== None): return False;
		self.last_online_id=ba.read_uint32();
		if ( self.last_online_id== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.last_islandid)):return False;
		if ( not ba.write_uint32(self.last_mapid)):return False;
		if ( not ba.write_uint32(self.last_x)):return False;
		if ( not ba.write_uint32(self.last_y)):return False;
		if ( not ba.write_uint32(self.last_login)):return False;
		if ( not ba.write_uint32(self.online_time)):return False;
		if ( not ba.write_uint32(self.last_online_id)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[last_islandid]=%s"%(str(self.last_islandid))
		print tabs+"""/*  */"""
		print tabs+"[last_mapid]=%s"%(str(self.last_mapid))
		print tabs+"""/*  */"""
		print tabs+"[last_x]=%s"%(str(self.last_x))
		print tabs+"""/*  */"""
		print tabs+"[last_y]=%s"%(str(self.last_y))
		print tabs+"""/*  */"""
		print tabs+"[last_login]=%s"%(str(self.last_login))
		print tabs+"""/*  */"""
		print tabs+"[online_time]=%s"%(str(self.online_time))
		print tabs+"""/* 最后一次登录的服务器 */"""
		print tabs+"[last_online_id]=%s"%(str(self.last_online_id))
		
		pass;
	




class pop_opt_in :
	"""
参数列表:
	/* 1:物品;2任务,3find_map */
	#类型:uint32
	public opt_groupid;

	/* 1:增加/减少,2:设置 */
	#类型:uint32
	public opt_type;

	/* 操作物的id（如ItemID） */
	#类型:uint32
	public optid;

	/* 改变的数量 */
	#类型:int32
	public count;

	/*  */
	#类型:uint32
	public v1;

	/*  */
	#类型:uint32
	public v2;


	"""

	def read_from_buf(self, ba ):
		self.opt_groupid=ba.read_uint32();
		if ( self.opt_groupid== None): return False;
		self.opt_type=ba.read_uint32();
		if ( self.opt_type== None): return False;
		self.optid=ba.read_uint32();
		if ( self.optid== None): return False;
		self.count=ba.read_int32();
		if ( self.count== None): return False;
		self.v1=ba.read_uint32();
		if ( self.v1== None): return False;
		self.v2=ba.read_uint32();
		if ( self.v2== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.opt_groupid)):return False;
		if ( not ba.write_uint32(self.opt_type)):return False;
		if ( not ba.write_uint32(self.optid)):return False;
		if ( not ba.write_int32(self.count)):return False;
		if ( not ba.write_uint32(self.v1)):return False;
		if ( not ba.write_uint32(self.v2)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 1:物品;2任务,3find_map */"""
		print tabs+"[opt_groupid]=%s"%(str(self.opt_groupid))
		print tabs+"""/* 1:增加/减少,2:设置 */"""
		print tabs+"[opt_type]=%s"%(str(self.opt_type))
		print tabs+"""/* 操作物的id（如ItemID） */"""
		print tabs+"[optid]=%s"%(str(self.optid))
		print tabs+"""/* 改变的数量 */"""
		print tabs+"[count]=%s"%(str(self.count))
		print tabs+"""/*  */"""
		print tabs+"[v1]=%s"%(str(self.v1))
		print tabs+"""/*  */"""
		print tabs+"[v2]=%s"%(str(self.v2))
		
		pass;
	




class pop_reg_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public color;

	/*  */
	#类型:uint32
	public age;

	/*  */
	#定长数组,长度:16, 类型:char 
	public nick ;

	/* 0x01:用户是否注册过 0x02是否新米米号 */
	#类型:uint32
	public flag;

	/**/
	#变长数组,最大长度:9999, 类型:item_exchange_t
	public add_item_list =array();


	"""

	def read_from_buf(self, ba ):
		self.color=ba.read_uint32();
		if ( self.color== None): return False;
		self.age=ba.read_uint32();
		if ( self.age== None): return False;
		self.nick=ba.read_buf(16);
		if( self.nick==None ): return False;
		self.flag=ba.read_uint32();
		if ( self.flag== None): return False;

		add_item_list_count= ba.read_uint32();
		if ( add_item_list_count==None ): return False;
		self.add_item_list=[];
		for i in range(add_item_list_count):
			self.add_item_list.append(item_exchange_t());
			if ( not self.add_item_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.color)):return False;
		if ( not ba.write_uint32(self.age)):return False;
		ba.write_buf(self.nick,16);
		if ( not ba.write_uint32(self.flag)):return False;
		add_item_list_count=len(self.add_item_list);
		ba.write_uint32(add_item_list_count);
		for i in range(add_item_list_count):
			if (  not  isinstance(self.add_item_list[i] ,item_exchange_t) ):return False; 
			if ( not self.add_item_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[color]=%s"%(str(self.color))
		print tabs+"""/*  */"""
		print tabs+"[age]=%s"%(str(self.age))
		print tabs+"""/*  */"""
		tmp_str=str(self.nick);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[nick]=%s"%(tmp_str)
		print tabs+"""/* 0x01:用户是否注册过 0x02是否新米米号 */"""
		print tabs+"[flag]=%s"%(str(self.flag))
		print tabs+"#"
		print tabs+"[add_item_list]=>{";
		for i in range(len(self.add_item_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.add_item_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class pop_set_color_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public color;


	"""

	def read_from_buf(self, ba ):
		self.color=ba.read_uint32();
		if ( self.color== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.color)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[color]=%s"%(str(self.color))
		
		pass;
	




class pop_set_flag_in :
	"""
参数列表:
	/* 0x01:用户是否注册过 0x02是否新米米号 */
	#类型:uint32
	public flag;

	/* 掩码 */
	#类型:uint32
	public mask;


	"""

	def read_from_buf(self, ba ):
		self.flag=ba.read_uint32();
		if ( self.flag== None): return False;
		self.mask=ba.read_uint32();
		if ( self.mask== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.flag)):return False;
		if ( not ba.write_uint32(self.mask)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 0x01:用户是否注册过 0x02是否新米米号 */"""
		print tabs+"[flag]=%s"%(str(self.flag))
		print tabs+"""/* 掩码 */"""
		print tabs+"[mask]=%s"%(str(self.mask))
		
		pass;
	




class pop_set_item_used_list_in :
	"""
参数列表:
	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public del_can_change_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public set_noused_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public add_can_change_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public set_used_list =array();


	"""

	def read_from_buf(self, ba ):

		del_can_change_list_count= ba.read_uint32();
		if ( del_can_change_list_count==None ): return False;
		self.del_can_change_list=[];
		for i in range(del_can_change_list_count):
			self.del_can_change_list.append(ba.read_uint32());
			if ( self.del_can_change_list[i]== None): return False;


		set_noused_list_count= ba.read_uint32();
		if ( set_noused_list_count==None ): return False;
		self.set_noused_list=[];
		for i in range(set_noused_list_count):
			self.set_noused_list.append(ba.read_uint32());
			if ( self.set_noused_list[i]== None): return False;


		add_can_change_list_count= ba.read_uint32();
		if ( add_can_change_list_count==None ): return False;
		self.add_can_change_list=[];
		for i in range(add_can_change_list_count):
			self.add_can_change_list.append(ba.read_uint32());
			if ( self.add_can_change_list[i]== None): return False;


		set_used_list_count= ba.read_uint32();
		if ( set_used_list_count==None ): return False;
		self.set_used_list=[];
		for i in range(set_used_list_count):
			self.set_used_list.append(ba.read_uint32());
			if ( self.set_used_list[i]== None): return False;


		return True;


	def write_to_buf(self,ba):
		del_can_change_list_count=len(self.del_can_change_list);
		ba.write_uint32(del_can_change_list_count);
		for i in range(del_can_change_list_count):
			if ( not ba.write_uint32(self.del_can_change_list[i])):return False;

		set_noused_list_count=len(self.set_noused_list);
		ba.write_uint32(set_noused_list_count);
		for i in range(set_noused_list_count):
			if ( not ba.write_uint32(self.set_noused_list[i])):return False;

		add_can_change_list_count=len(self.add_can_change_list);
		ba.write_uint32(add_can_change_list_count);
		for i in range(add_can_change_list_count):
			if ( not ba.write_uint32(self.add_can_change_list[i])):return False;

		set_used_list_count=len(self.set_used_list);
		ba.write_uint32(set_used_list_count);
		for i in range(set_used_list_count):
			if ( not ba.write_uint32(self.set_used_list[i])):return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"#"
		print tabs+"[del_can_change_list]=>{";
		for i in range(len(self.del_can_change_list)):
			print tabs+"    [%d]="%(i)+str(self.del_can_change_list[i]); 

		print tabs+'}'
		print tabs+"#"
		print tabs+"[set_noused_list]=>{";
		for i in range(len(self.set_noused_list)):
			print tabs+"    [%d]="%(i)+str(self.set_noused_list[i]); 

		print tabs+'}'
		print tabs+"#"
		print tabs+"[add_can_change_list]=>{";
		for i in range(len(self.add_can_change_list)):
			print tabs+"    [%d]="%(i)+str(self.add_can_change_list[i]); 

		print tabs+'}'
		print tabs+"#"
		print tabs+"[set_used_list]=>{";
		for i in range(len(self.set_used_list)):
			print tabs+"    [%d]="%(i)+str(self.set_used_list[i]); 

		print tabs+'}'
		
		pass;
	




class pop_set_nick_in :
	"""
参数列表:
	/*  */
	#定长数组,长度:16, 类型:char 
	public nick ;


	"""

	def read_from_buf(self, ba ):
		self.nick=ba.read_buf(16);
		if( self.nick==None ): return False;

		return True;


	def write_to_buf(self,ba):
		ba.write_buf(self.nick,16);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		tmp_str=str(self.nick);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[nick]=%s"%(tmp_str)
		
		pass;
	




class pop_task_complete_node_in :
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
	




class pop_task_del_in :
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
	




class pop_user_log_add_in :
	"""
参数列表:
	/*  */
	#类型:user_log_t
	public user_log;


	"""

	def read_from_buf(self, ba ):
		self.user_log=user_log_t();
		if ( not self.user_log.read_from_buf(ba)) :return False;

		return True;


	def write_to_buf(self,ba):
		if ( not isinstance(self.user_log,user_log_t)): return False; 
		if ( not self.user_log.write_to_buf(ba)): return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */""";
		print tabs+"[user_log]=>{";
		self.user_log.echo(print_hex,tabs+"    ");
		print tabs+"}";
		
		pass;
	




class pop_user_set_field_value_in :
	"""
参数列表:
	/* 字段名 */
	#定长数组,长度:64, 类型:char 
	public field_name ;

	/* 字段值 */
	#定长数组,长度:255, 类型:char 
	public field_value ;


	"""

	def read_from_buf(self, ba ):
		self.field_name=ba.read_buf(64);
		if( self.field_name==None ): return False;
		self.field_value=ba.read_buf(255);
		if( self.field_value==None ): return False;

		return True;


	def write_to_buf(self,ba):
		ba.write_buf(self.field_name,64);
		ba.write_buf(self.field_value,255);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 字段名 */"""
		tmp_str=str(self.field_name);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[field_name]=%s"%(tmp_str)
		print tabs+"""/* 字段值 */"""
		tmp_str=str(self.field_value);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[field_value]=%s"%(tmp_str)
		
		pass;
	




class udp_post_msg_in :
	"""
参数列表:
	/* 项目编号 */
	#类型:uint32
	public gameid;

	/* 无效填0 */
	#类型:uint32
	public nouse;

	/* 投稿类型 */
	#类型:uint32
	public type;

	/* 昵称 */
	#定长数组,长度:16, 类型:char 
	public nick ;

	/* 主题 */
	#定长数组,长度:60, 类型:char 
	public title ;

	/*投稿内容*/
	#变长数组,最大长度:4096, 类型:char
	public msg ;


	"""

	def read_from_buf(self, ba ):
		self.gameid=ba.read_uint32();
		if ( self.gameid== None): return False;
		self.nouse=ba.read_uint32();
		if ( self.nouse== None): return False;
		self.type=ba.read_uint32();
		if ( self.type== None): return False;
		self.nick=ba.read_buf(16);
		if( self.nick==None ): return False;
		self.title=ba.read_buf(60);
		if( self.title==None ): return False;

		msg_count= ba.read_uint32();
		if ( msg_count==None ): return False;
		self.msg=ba.read_buf(msg_count);
		if( self.msg==None ):return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.gameid)):return False;
		if ( not ba.write_uint32(self.nouse)):return False;
		if ( not ba.write_uint32(self.type)):return False;
		ba.write_buf(self.nick,16);
		ba.write_buf(self.title,60);
		msg_count=len(self.msg);
		ba.write_uint32(msg_count);
		ba.write_buf(self.msg,msg_count);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 项目编号 */"""
		print tabs+"[gameid]=%s"%(str(self.gameid))
		print tabs+"""/* 无效填0 */"""
		print tabs+"[nouse]=%s"%(str(self.nouse))
		print tabs+"""/* 投稿类型 */"""
		print tabs+"[type]=%s"%(str(self.type))
		print tabs+"""/* 昵称 */"""
		tmp_str=str(self.nick);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[nick]=%s"%(tmp_str)
		print tabs+"""/* 主题 */"""
		tmp_str=str(self.title);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[title]=%s"%(tmp_str)
		print tabs+"""/* 投稿内容 */"""
		print tabs+"[msg]=%s"%(str(self.msg))
		
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
	




create_time='2012年12月27日 星期4 12:04:27'
cmd_map={ }
cmd_map[0x1000]=(0x1000,'get_server_version',None,get_server_version_out,0xe3716819)
cmd_map[0x7401]=(0x7401,'pop_login',None,pop_login_out,0x02822bfc)
cmd_map[0x7402]=(0x7402,'pop_reg',pop_reg_in,None,0xb792ae08)
cmd_map[0x7403]=(0x7403,'pop_set_nick',pop_set_nick_in,None,0x874eb534)
cmd_map[0x7404]=(0x7404,'pop_copy_user',pop_copy_user_in,None,0x5394bd57)
cmd_map[0x7405]=(0x7405,'pop_get_base_info',None,pop_get_base_info_out,0xd9a8fc35)
cmd_map[0x7406]=(0x7406,'pop_set_flag',pop_set_flag_in,None,0x13813167)
cmd_map[0x7407]=(0x7407,'pop_set_color',pop_set_color_in,None,0x941960fe)
cmd_map[0x740A]=(0x740A,'pop_exchange',pop_exchange_in,None,0x79a82f26)
cmd_map[0x740C]=(0x740C,'pop_set_item_used_list',pop_set_item_used_list_in,None,0xc71f127c)
cmd_map[0x7410]=(0x7410,'pop_task_complete_node',pop_task_complete_node_in,None,0x18cc6534)
cmd_map[0x7411]=(0x7411,'pop_task_del',pop_task_del_in,None,0x4f39999d)
cmd_map[0x741A]=(0x741A,'pop_find_map_add',pop_find_map_add_in,None,0x4d028768)
cmd_map[0x741B]=(0x741B,'pop_logout',pop_logout_in,None,0x4d68721b)
cmd_map[0x741C]=(0x741C,'pop_user_log_add',pop_user_log_add_in,None,0xb2be2b7b)
cmd_map[0x74A0]=(0x74A0,'pop_get_all_info',None,pop_get_all_info_out,0x151b1b12)
cmd_map[0x74A1]=(0x74A1,'pop_user_set_field_value',pop_user_set_field_value_in,None,0x50ea630f)
cmd_map[0x74A2]=(0x74A2,'pop_opt',pop_opt_in,None,0x392d057e)
cmd_map[0x74A4]=(0x74A4,'pop_add_game_info',pop_add_game_info_in,None,0x95f2cb99)
cmd_map[0xF130]=(0xF130,'udp_post_msg',udp_post_msg_in,None,0xbe74ec43)
	
def init_cmd_map():
	pass

try:
	import pop_db_proto_base
	base_type=pop_db_proto_base.Cpop_db_proto_base
except:
	base_type=object
	pass


class Cpop_db_proto (base_type):
	def __init__(self,ip,port):
		pop_db_proto_base.Cpop_db_proto_base.__init__(self,ip,port)

	def get_server_version(self  ):
		"""
得到版本信息:

	"""
		pri_in=None;

		self.sendmsg(0x1000, pri_in );
		pass

	def pop_login(self  ):
		"""
登入:

	"""
		pri_in=None;

		self.sendmsg(0x7401, pri_in );
		pass

	def pop_reg(self ,color,age,nick,flag,add_item_list ):
		"""
注册:
	/**/
	#变长数组,最大长度:9999, 类型:item_exchange_t
	public add_item_list =array();


	"""
		pri_in=pop_reg_in();

		pri_in.color=color
		pri_in.age=age
		pri_in.nick=nick
		pri_in.flag=flag
		pri_in.add_item_list=add_item_list
		self.sendmsg(0x7402, pri_in );
		pass

	def pop_set_nick(self ,nick ):
		"""
设置昵称:
	/*  */
	#定长数组,长度:16, 类型:char 
	public nick ;


	"""
		pri_in=pop_set_nick_in();

		pri_in.nick=nick
		self.sendmsg(0x7403, pri_in );
		pass

	def pop_copy_user(self ,dsc_userid ):
		"""
复制用户:
	/* 目标userid */
	#类型:uint32
	public dsc_userid;


	"""
		pri_in=pop_copy_user_in();

		pri_in.dsc_userid=dsc_userid
		self.sendmsg(0x7404, pri_in );
		pass

	def pop_get_base_info(self  ):
		"""
:

	"""
		pri_in=None;

		self.sendmsg(0x7405, pri_in );
		pass

	def pop_set_flag(self ,flag,mask ):
		"""
设置用户标识:
	/* 掩码 */
	#类型:uint32
	public mask;


	"""
		pri_in=pop_set_flag_in();

		pri_in.flag=flag
		pri_in.mask=mask
		self.sendmsg(0x7406, pri_in );
		pass

	def pop_set_color(self ,color ):
		"""
:
	/*  */
	#类型:uint32
	public color;


	"""
		pri_in=pop_set_color_in();

		pri_in.color=color
		self.sendmsg(0x7407, pri_in );
		pass

	def pop_exchange(self ,opt_flag,add_item_list,del_item_list ):
		"""
交换协议，物品，任务，钱:
	/**/
	#变长数组,最大长度:9999, 类型:item_exchange_t
	public del_item_list =array();


	"""
		pri_in=pop_exchange_in();

		pri_in.opt_flag=opt_flag
		pri_in.add_item_list=add_item_list
		pri_in.del_item_list=del_item_list
		self.sendmsg(0x740A, pri_in );
		pass

	def pop_set_item_used_list(self ,del_can_change_list,set_noused_list,add_can_change_list,set_used_list ):
		"""
设置使用列表，会将原有使用的设置为未使用:
	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public set_used_list =array();


	"""
		pri_in=pop_set_item_used_list_in();

		pri_in.del_can_change_list=del_can_change_list
		pri_in.set_noused_list=set_noused_list
		pri_in.add_can_change_list=add_can_change_list
		pri_in.set_used_list=set_used_list
		self.sendmsg(0x740C, pri_in );
		pass

	def pop_task_complete_node(self ,taskid,task_nodeid ):
		"""
完成子节点任务:
	/*  */
	#类型:uint32
	public task_nodeid;


	"""
		pri_in=pop_task_complete_node_in();

		pri_in.taskid=taskid
		pri_in.task_nodeid=task_nodeid
		self.sendmsg(0x7410, pri_in );
		pass

	def pop_task_del(self ,taskid,task_nodeid ):
		"""
删除任务:
	/*  */
	#类型:uint32
	public task_nodeid;


	"""
		pri_in=pop_task_del_in();

		pri_in.taskid=taskid
		pri_in.task_nodeid=task_nodeid
		self.sendmsg(0x7411, pri_in );
		pass

	def pop_find_map_add(self ,islandid,mapid ):
		"""
找到哪个地图:
	/*  */
	#类型:uint32
	public mapid;


	"""
		pri_in=pop_find_map_add_in();

		pri_in.islandid=islandid
		pri_in.mapid=mapid
		self.sendmsg(0x741A, pri_in );
		pass

	def pop_logout(self ,last_islandid,last_mapid,last_x,last_y,last_login,online_time,last_online_id ):
		"""
退出:
	/* 最后一次登录的服务器 */
	#类型:uint32
	public last_online_id;


	"""
		pri_in=pop_logout_in();

		pri_in.last_islandid=last_islandid
		pri_in.last_mapid=last_mapid
		pri_in.last_x=last_x
		pri_in.last_y=last_y
		pri_in.last_login=last_login
		pri_in.online_time=online_time
		pri_in.last_online_id=last_online_id
		self.sendmsg(0x741B, pri_in );
		pass

	def pop_user_log_add(self ,user_log ):
		"""
用户足迹:
	/*  */
	#类型:user_log_t
	public user_log;


	"""
		pri_in=pop_user_log_add_in();

		pri_in.user_log=user_log
		self.sendmsg(0x741C, pri_in );
		pass

	def pop_get_all_info(self  ):
		"""
得到用户的所有信息:

	"""
		pri_in=None;

		self.sendmsg(0x74A0, pri_in );
		pass

	def pop_user_set_field_value(self ,field_name,field_value ):
		"""
设置user表某一字段的值:
	/* 字段值 */
	#定长数组,长度:255, 类型:char 
	public field_value ;


	"""
		pri_in=pop_user_set_field_value_in();

		pri_in.field_name=field_name
		pri_in.field_value=field_value
		self.sendmsg(0x74A1, pri_in );
		pass

	def pop_opt(self ,opt_groupid,opt_type,optid,count,v1,v2 ):
		"""
设置相关的值:
	/*  */
	#类型:uint32
	public v2;


	"""
		pri_in=pop_opt_in();

		pri_in.opt_groupid=opt_groupid
		pri_in.opt_type=opt_type
		pri_in.optid=optid
		pri_in.count=count
		pri_in.v1=v1
		pri_in.v2=v2
		self.sendmsg(0x74A2, pri_in );
		pass

	def pop_add_game_info(self ,gameid,win_flag ):
		"""
增加小游戏统计:
	/* 胜负标志0：负 1：胜 2：平 */
	#类型:uint32
	public win_flag;


	"""
		pri_in=pop_add_game_info_in();

		pri_in.gameid=gameid
		pri_in.win_flag=win_flag
		self.sendmsg(0x74A4, pri_in );
		pass

	def udp_post_msg(self ,gameid,nouse,type,nick,title,msg ):
		"""
udp 投稿:
	/*投稿内容*/
	#变长数组,最大长度:4096, 类型:char
	public msg ;


	"""
		pri_in=udp_post_msg_in();

		pri_in.gameid=gameid
		pri_in.nouse=nouse
		pri_in.type=type
		pri_in.nick=nick
		pri_in.title=title
		pri_in.msg=msg
		self.sendmsg(0xF130, pri_in );
		pass


