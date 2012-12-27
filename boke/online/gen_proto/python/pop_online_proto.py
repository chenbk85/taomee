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

		
class cli_buy_item_in :
	"""
参数列表:
	/* 物品id */
	#类型:uint32
	public itemid;


	"""

	def read_from_buf(self, ba ):
		self.itemid=ba.read_uint32();
		if ( self.itemid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.itemid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 物品id */"""
		print tabs+"[itemid]=%s"%(str(self.itemid))
		
		pass;
	




class cli_buy_item_out :
	"""
参数列表:
	/* 剩余多少钱 */
	#类型:uint32
	public left_xiaomee;


	"""

	def read_from_buf(self, ba ):
		self.left_xiaomee=ba.read_uint32();
		if ( self.left_xiaomee== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.left_xiaomee)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 剩余多少钱 */"""
		print tabs+"[left_xiaomee]=%s"%(str(self.left_xiaomee))
		
		pass;
	




class cli_buy_item_use_gamept_in :
	"""
参数列表:
	/* 物品ID */
	#类型:uint32
	public itemid;


	"""

	def read_from_buf(self, ba ):
		self.itemid=ba.read_uint32();
		if ( self.itemid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.itemid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 物品ID */"""
		print tabs+"[itemid]=%s"%(str(self.itemid))
		
		pass;
	




class cli_buy_item_use_gamept_out :
	"""
参数列表:
	/* 剩余可用游戏积分 */
	#类型:uint32
	public left_gamept;


	"""

	def read_from_buf(self, ba ):
		self.left_gamept=ba.read_uint32();
		if ( self.left_gamept== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.left_gamept)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 剩余可用游戏积分 */"""
		print tabs+"[left_gamept]=%s"%(str(self.left_gamept))
		
		pass;
	




class cli_click_stat_in :
	"""
参数列表:
	/* 点击类型（1蜘蛛 2石碑） */
	#类型:uint32
	public type;


	"""

	def read_from_buf(self, ba ):
		self.type=ba.read_uint32();
		if ( self.type== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.type)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 点击类型（1蜘蛛 2石碑） */"""
		print tabs+"[type]=%s"%(str(self.type))
		
		pass;
	




class cli_cur_game_end_in :
	"""
参数列表:
	/* 自己是不是羸了 :0:不是,1:是,2:平局 */
	#类型:uint32
	public win_flag;


	"""

	def read_from_buf(self, ba ):
		self.win_flag=ba.read_uint32();
		if ( self.win_flag== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.win_flag)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 自己是不是羸了 :0:不是,1:是,2:平局 */"""
		print tabs+"[win_flag]=%s"%(str(self.win_flag))
		
		pass;
	




class item_t :
	"""
参数列表:
	/* 物品id */
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
		print tabs+"""/* 物品id */"""
		print tabs+"[itemid]=%s"%(str(self.itemid))
		print tabs+"""/*  */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class cli_del_item_in :
	"""
参数列表:
	/*要删除的物品列表*/
	#变长数组,最大长度:999, 类型:item_t
	public itemlist =array();


	"""

	def read_from_buf(self, ba ):

		itemlist_count= ba.read_uint32();
		if ( itemlist_count==None ): return False;
		self.itemlist=[];
		for i in range(itemlist_count):
			self.itemlist.append(item_t());
			if ( not self.itemlist[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		itemlist_count=len(self.itemlist);
		ba.write_uint32(itemlist_count);
		for i in range(itemlist_count):
			if (  not  isinstance(self.itemlist[i] ,item_t) ):return False; 
			if ( not self.itemlist[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"#要删除的物品列表"
		print tabs+"[itemlist]=>{";
		for i in range(len(self.itemlist)):
			print tabs+"    [%d]=>{"%(i) 
			self.itemlist[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class cli_draw_lottery_out :
	"""
参数列表:
	/* 获取的物品（2卜克豆） */
	#类型:item_t
	public item_get;


	"""

	def read_from_buf(self, ba ):
		self.item_get=item_t();
		if ( not self.item_get.read_from_buf(ba)) :return False;

		return True;


	def write_to_buf(self,ba):
		if ( not isinstance(self.item_get,item_t)): return False; 
		if ( not self.item_get.write_to_buf(ba)): return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 获取的物品（2卜克豆） */""";
		print tabs+"[item_get]=>{";
		self.item_get.echo(print_hex,tabs+"    ");
		print tabs+"}";
		
		pass;
	




class cli_find_map_add_in :
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
	




class cli_game_opt_in :
	"""
参数列表:
	/*游戏操作信息*/
	#变长数组,最大长度:9999, 类型:char
	public gamemsg ;


	"""

	def read_from_buf(self, ba ):

		gamemsg_count= ba.read_uint32();
		if ( gamemsg_count==None ): return False;
		self.gamemsg=ba.read_buf(gamemsg_count);
		if( self.gamemsg==None ):return False;

		return True;


	def write_to_buf(self,ba):
		gamemsg_count=len(self.gamemsg);
		ba.write_uint32(gamemsg_count);
		ba.write_buf(self.gamemsg,gamemsg_count);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 游戏操作信息 */"""
		print tabs+"[gamemsg]=%s"%(str(self.gamemsg))
		
		pass;
	




class cli_game_play_with_other_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public gameid;

	/* 该请求的key, 在 确认时将之一起发出 */
	#定长数组,长度:32, 类型:char 
	public request_session ;

	/* 目标userid */
	#类型:uint32
	public obj_userid;

	/* 是否开始,0:否(终止游戏), 1:是 */
	#类型:uint32
	public is_start;

	/* 为什么不玩, 1:自己关，2：在换装，3：购物 */
	#类型:uint32
	public why_not_start_flag;


	"""

	def read_from_buf(self, ba ):
		self.gameid=ba.read_uint32();
		if ( self.gameid== None): return False;
		self.request_session=ba.read_buf(32);
		if( self.request_session==None ): return False;
		self.obj_userid=ba.read_uint32();
		if ( self.obj_userid== None): return False;
		self.is_start=ba.read_uint32();
		if ( self.is_start== None): return False;
		self.why_not_start_flag=ba.read_uint32();
		if ( self.why_not_start_flag== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.gameid)):return False;
		ba.write_buf(self.request_session,32);
		if ( not ba.write_uint32(self.obj_userid)):return False;
		if ( not ba.write_uint32(self.is_start)):return False;
		if ( not ba.write_uint32(self.why_not_start_flag)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[gameid]=%s"%(str(self.gameid))
		print tabs+"""/* 该请求的key, 在 确认时将之一起发出 */"""
		tmp_str=str(self.request_session);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[request_session]=%s"%(tmp_str)
		print tabs+"""/* 目标userid */"""
		print tabs+"[obj_userid]=%s"%(str(self.obj_userid))
		print tabs+"""/* 是否开始,0:否(终止游戏), 1:是 */"""
		print tabs+"[is_start]=%s"%(str(self.is_start))
		print tabs+"""/* 为什么不玩, 1:自己关，2：在换装，3：购物 */"""
		print tabs+"[why_not_start_flag]=%s"%(str(self.why_not_start_flag))
		
		pass;
	




class cli_game_play_with_other_out :
	"""
参数列表:
	/* 1:不存地图，2:已经在其它游戏中 */
	#类型:uint32
	public obj_user_stat;


	"""

	def read_from_buf(self, ba ):
		self.obj_user_stat=ba.read_uint32();
		if ( self.obj_user_stat== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.obj_user_stat)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 1:不存地图，2:已经在其它游戏中 */"""
		print tabs+"[obj_user_stat]=%s"%(str(self.obj_user_stat))
		
		pass;
	




class cli_game_request_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public gameid;

	/* 目标userid */
	#类型:uint32
	public obj_userid;


	"""

	def read_from_buf(self, ba ):
		self.gameid=ba.read_uint32();
		if ( self.gameid== None): return False;
		self.obj_userid=ba.read_uint32();
		if ( self.obj_userid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.gameid)):return False;
		if ( not ba.write_uint32(self.obj_userid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[gameid]=%s"%(str(self.gameid))
		print tabs+"""/* 目标userid */"""
		print tabs+"[obj_userid]=%s"%(str(self.obj_userid))
		
		pass;
	




class cli_game_request_out :
	"""
参数列表:
	/* 1:不存地图，2:已经在其它游戏中 */
	#类型:uint32
	public obj_user_stat;


	"""

	def read_from_buf(self, ba ):
		self.obj_user_stat=ba.read_uint32();
		if ( self.obj_user_stat== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.obj_user_stat)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 1:不存地图，2:已经在其它游戏中 */"""
		print tabs+"[obj_user_stat]=%s"%(str(self.obj_user_stat))
		
		pass;
	




class online_user_info_t :
	"""
参数列表:
	/* 目标userid */
	#类型:uint32
	public obj_userid;

	/* 肤色 */
	#类型:uint32
	public color;

	/*  */
	#定长数组,长度:16, 类型:char 
	public nick ;

	/* 卜克豆 */
	#类型:uint32
	public xiaomee;

	/* 等级 */
	#类型:uint32
	public level;

	/* 经验值 */
	#类型:uint32
	public experience;

	/*  */
	#类型:int32
	public x;

	/*  */
	#类型:int32
	public y;

	/*勋章个数*/
	#变长数组,最大长度:100, 类型:uint32_t
	public medal_list =array();

	/* 小游戏积分 */
	#类型:uint32
	public game_point;

	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public use_clothes_list =array();

	/*使用中的特效卡片列表*/
	#变长数组,最大长度:999, 类型:uint32_t
	public effect_list =array();


	"""

	def read_from_buf(self, ba ):
		self.obj_userid=ba.read_uint32();
		if ( self.obj_userid== None): return False;
		self.color=ba.read_uint32();
		if ( self.color== None): return False;
		self.nick=ba.read_buf(16);
		if( self.nick==None ): return False;
		self.xiaomee=ba.read_uint32();
		if ( self.xiaomee== None): return False;
		self.level=ba.read_uint32();
		if ( self.level== None): return False;
		self.experience=ba.read_uint32();
		if ( self.experience== None): return False;
		self.x=ba.read_int32();
		if ( self.x== None): return False;
		self.y=ba.read_int32();
		if ( self.y== None): return False;

		medal_list_count= ba.read_uint32();
		if ( medal_list_count==None ): return False;
		self.medal_list=[];
		for i in range(medal_list_count):
			self.medal_list.append(ba.read_uint32());
			if ( self.medal_list[i]== None): return False;

		self.game_point=ba.read_uint32();
		if ( self.game_point== None): return False;

		use_clothes_list_count= ba.read_uint32();
		if ( use_clothes_list_count==None ): return False;
		self.use_clothes_list=[];
		for i in range(use_clothes_list_count):
			self.use_clothes_list.append(ba.read_uint32());
			if ( self.use_clothes_list[i]== None): return False;


		effect_list_count= ba.read_uint32();
		if ( effect_list_count==None ): return False;
		self.effect_list=[];
		for i in range(effect_list_count):
			self.effect_list.append(ba.read_uint32());
			if ( self.effect_list[i]== None): return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.obj_userid)):return False;
		if ( not ba.write_uint32(self.color)):return False;
		ba.write_buf(self.nick,16);
		if ( not ba.write_uint32(self.xiaomee)):return False;
		if ( not ba.write_uint32(self.level)):return False;
		if ( not ba.write_uint32(self.experience)):return False;
		if ( not ba.write_int32(self.x)):return False;
		if ( not ba.write_int32(self.y)):return False;
		medal_list_count=len(self.medal_list);
		ba.write_uint32(medal_list_count);
		for i in range(medal_list_count):
			if ( not ba.write_uint32(self.medal_list[i])):return False;

		if ( not ba.write_uint32(self.game_point)):return False;
		use_clothes_list_count=len(self.use_clothes_list);
		ba.write_uint32(use_clothes_list_count);
		for i in range(use_clothes_list_count):
			if ( not ba.write_uint32(self.use_clothes_list[i])):return False;

		effect_list_count=len(self.effect_list);
		ba.write_uint32(effect_list_count);
		for i in range(effect_list_count):
			if ( not ba.write_uint32(self.effect_list[i])):return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 目标userid */"""
		print tabs+"[obj_userid]=%s"%(str(self.obj_userid))
		print tabs+"""/* 肤色 */"""
		print tabs+"[color]=%s"%(str(self.color))
		print tabs+"""/*  */"""
		tmp_str=str(self.nick);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[nick]=%s"%(tmp_str)
		print tabs+"""/* 卜克豆 */"""
		print tabs+"[xiaomee]=%s"%(str(self.xiaomee))
		print tabs+"""/* 等级 */"""
		print tabs+"[level]=%s"%(str(self.level))
		print tabs+"""/* 经验值 */"""
		print tabs+"[experience]=%s"%(str(self.experience))
		print tabs+"""/*  */"""
		print tabs+"[x]=%s"%(str(self.x))
		print tabs+"""/*  */"""
		print tabs+"[y]=%s"%(str(self.y))
		print tabs+"#勋章个数"
		print tabs+"[medal_list]=>{";
		for i in range(len(self.medal_list)):
			print tabs+"    [%d]="%(i)+str(self.medal_list[i]); 

		print tabs+'}'
		print tabs+"""/* 小游戏积分 */"""
		print tabs+"[game_point]=%s"%(str(self.game_point))
		print tabs+"#"
		print tabs+"[use_clothes_list]=>{";
		for i in range(len(self.use_clothes_list)):
			print tabs+"    [%d]="%(i)+str(self.use_clothes_list[i]); 

		print tabs+'}'
		print tabs+"#使用中的特效卡片列表"
		print tabs+"[effect_list]=>{";
		for i in range(len(self.effect_list)):
			print tabs+"    [%d]="%(i)+str(self.effect_list[i]); 

		print tabs+'}'
		
		pass;
	




class cli_get_all_user_info_from_cur_map_out :
	"""
参数列表:
	/**/
	#变长数组,最大长度:9999, 类型:online_user_info_t
	public online_user_info_list =array();


	"""

	def read_from_buf(self, ba ):

		online_user_info_list_count= ba.read_uint32();
		if ( online_user_info_list_count==None ): return False;
		self.online_user_info_list=[];
		for i in range(online_user_info_list_count):
			self.online_user_info_list.append(online_user_info_t());
			if ( not self.online_user_info_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		online_user_info_list_count=len(self.online_user_info_list);
		ba.write_uint32(online_user_info_list_count);
		for i in range(online_user_info_list_count):
			if (  not  isinstance(self.online_user_info_list[i] ,online_user_info_t) ):return False; 
			if ( not self.online_user_info_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"#"
		print tabs+"[online_user_info_list]=>{";
		for i in range(len(self.online_user_info_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.online_user_info_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class cli_get_card_list_by_islandid_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public islandid;


	"""

	def read_from_buf(self, ba ):
		self.islandid=ba.read_uint32();
		if ( self.islandid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.islandid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[islandid]=%s"%(str(self.islandid))
		
		pass;
	




class cli_get_card_list_by_islandid_out :
	"""
参数列表:
	/**/
	#变长数组,最大长度:9999, 类型:item_t
	public cardid_list =array();


	"""

	def read_from_buf(self, ba ):

		cardid_list_count= ba.read_uint32();
		if ( cardid_list_count==None ): return False;
		self.cardid_list=[];
		for i in range(cardid_list_count):
			self.cardid_list.append(item_t());
			if ( not self.cardid_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		cardid_list_count=len(self.cardid_list);
		ba.write_uint32(cardid_list_count);
		for i in range(cardid_list_count):
			if (  not  isinstance(self.cardid_list[i] ,item_t) ):return False; 
			if ( not self.cardid_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"#"
		print tabs+"[cardid_list]=>{";
		for i in range(len(self.cardid_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.cardid_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class game_user_t :
	"""
参数列表:
	/* 发起的userid */
	#类型:uint32
	public src_userid;

	/* 项目编号 */
	#类型:uint32
	public gameid;

	/*游戏中的其他玩家列表*/
	#变长数组,最大长度:10, 类型:uint32_t
	public userlist =array();


	"""

	def read_from_buf(self, ba ):
		self.src_userid=ba.read_uint32();
		if ( self.src_userid== None): return False;
		self.gameid=ba.read_uint32();
		if ( self.gameid== None): return False;

		userlist_count= ba.read_uint32();
		if ( userlist_count==None ): return False;
		self.userlist=[];
		for i in range(userlist_count):
			self.userlist.append(ba.read_uint32());
			if ( self.userlist[i]== None): return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.src_userid)):return False;
		if ( not ba.write_uint32(self.gameid)):return False;
		userlist_count=len(self.userlist);
		ba.write_uint32(userlist_count);
		for i in range(userlist_count):
			if ( not ba.write_uint32(self.userlist[i])):return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 发起的userid */"""
		print tabs+"[src_userid]=%s"%(str(self.src_userid))
		print tabs+"""/* 项目编号 */"""
		print tabs+"[gameid]=%s"%(str(self.gameid))
		print tabs+"#游戏中的其他玩家列表"
		print tabs+"[userlist]=>{";
		for i in range(len(self.userlist)):
			print tabs+"    [%d]="%(i)+str(self.userlist[i]); 

		print tabs+'}'
		
		pass;
	




class cli_get_game_user_out :
	"""
参数列表:
	/*游戏中的用户列表*/
	#变长数组,最大长度:999, 类型:game_user_t
	public game_user_list =array();


	"""

	def read_from_buf(self, ba ):

		game_user_list_count= ba.read_uint32();
		if ( game_user_list_count==None ): return False;
		self.game_user_list=[];
		for i in range(game_user_list_count):
			self.game_user_list.append(game_user_t());
			if ( not self.game_user_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		game_user_list_count=len(self.game_user_list);
		ba.write_uint32(game_user_list_count);
		for i in range(game_user_list_count):
			if (  not  isinstance(self.game_user_list[i] ,game_user_t) ):return False; 
			if ( not self.game_user_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"#游戏中的用户列表"
		print tabs+"[game_user_list]=>{";
		for i in range(len(self.game_user_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.game_user_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class island_time_t :
	"""
参数列表:
	/* 岛屿ID */
	#类型:uint32
	public islandid;

	/* 当前是否开放 */
	#类型:uint32
	public is_open;

	/* 下次状态改变时间 */
	#类型:uint32
	public next_state_time;


	"""

	def read_from_buf(self, ba ):
		self.islandid=ba.read_uint32();
		if ( self.islandid== None): return False;
		self.is_open=ba.read_uint32();
		if ( self.is_open== None): return False;
		self.next_state_time=ba.read_uint32();
		if ( self.next_state_time== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.islandid)):return False;
		if ( not ba.write_uint32(self.is_open)):return False;
		if ( not ba.write_uint32(self.next_state_time)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 岛屿ID */"""
		print tabs+"[islandid]=%s"%(str(self.islandid))
		print tabs+"""/* 当前是否开放 */"""
		print tabs+"[is_open]=%s"%(str(self.is_open))
		print tabs+"""/* 下次状态改变时间 */"""
		print tabs+"[next_state_time]=%s"%(str(self.next_state_time))
		
		pass;
	




class cli_get_island_time_out :
	"""
参数列表:
	/* 当前服务器时间 */
	#类型:uint32
	public svr_time;

	/*岛屿开放时间列表*/
	#变长数组,最大长度:100, 类型:island_time_t
	public islandlist =array();


	"""

	def read_from_buf(self, ba ):
		self.svr_time=ba.read_uint32();
		if ( self.svr_time== None): return False;

		islandlist_count= ba.read_uint32();
		if ( islandlist_count==None ): return False;
		self.islandlist=[];
		for i in range(islandlist_count):
			self.islandlist.append(island_time_t());
			if ( not self.islandlist[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.svr_time)):return False;
		islandlist_count=len(self.islandlist);
		ba.write_uint32(islandlist_count);
		for i in range(islandlist_count):
			if (  not  isinstance(self.islandlist[i] ,island_time_t) ):return False; 
			if ( not self.islandlist[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 当前服务器时间 */"""
		print tabs+"[svr_time]=%s"%(str(self.svr_time))
		print tabs+"#岛屿开放时间列表"
		print tabs+"[islandlist]=>{";
		for i in range(len(self.islandlist)):
			print tabs+"    [%d]=>{"%(i) 
			self.islandlist[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class cli_get_item_list_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public startid;

	/*  */
	#类型:uint32
	public endid;


	"""

	def read_from_buf(self, ba ):
		self.startid=ba.read_uint32();
		if ( self.startid== None): return False;
		self.endid=ba.read_uint32();
		if ( self.endid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.startid)):return False;
		if ( not ba.write_uint32(self.endid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[startid]=%s"%(str(self.startid))
		print tabs+"""/*  */"""
		print tabs+"[endid]=%s"%(str(self.endid))
		
		pass;
	




class cli_get_item_list_out :
	"""
参数列表:
	/**/
	#变长数组,最大长度:9999, 类型:item_t
	public item_list =array();


	"""

	def read_from_buf(self, ba ):

		item_list_count= ba.read_uint32();
		if ( item_list_count==None ): return False;
		self.item_list=[];
		for i in range(item_list_count):
			self.item_list.append(item_t());
			if ( not self.item_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		item_list_count=len(self.item_list);
		ba.write_uint32(item_list_count);
		for i in range(item_list_count):
			if (  not  isinstance(self.item_list[i] ,item_t) ):return False; 
			if ( not self.item_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"#"
		print tabs+"[item_list]=>{";
		for i in range(len(self.item_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.item_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class cli_get_login_chest_out :
	"""
参数列表:
	/* 获取的物品（2卜克豆） */
	#类型:item_t
	public item_get;


	"""

	def read_from_buf(self, ba ):
		self.item_get=item_t();
		if ( not self.item_get.read_from_buf(ba)) :return False;

		return True;


	def write_to_buf(self,ba):
		if ( not isinstance(self.item_get,item_t)): return False; 
		if ( not self.item_get.write_to_buf(ba)): return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 获取的物品（2卜克豆） */""";
		print tabs+"[item_get]=>{";
		self.item_get.echo(print_hex,tabs+"    ");
		print tabs+"}";
		
		pass;
	




class cli_get_login_reward_in :
	"""
参数列表:
	/* 序号（0~6） */
	#类型:uint32
	public index;


	"""

	def read_from_buf(self, ba ):
		self.index=ba.read_uint32();
		if ( self.index== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.index)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 序号（0~6） */"""
		print tabs+"[index]=%s"%(str(self.index))
		
		pass;
	




class cli_get_login_reward_out :
	"""
参数列表:
	/* 获取的物品（2卜克豆） */
	#类型:item_t
	public item_get;


	"""

	def read_from_buf(self, ba ):
		self.item_get=item_t();
		if ( not self.item_get.read_from_buf(ba)) :return False;

		return True;


	def write_to_buf(self,ba):
		if ( not isinstance(self.item_get,item_t)): return False; 
		if ( not self.item_get.write_to_buf(ba)): return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 获取的物品（2卜克豆） */""";
		print tabs+"[item_get]=>{";
		self.item_get.echo(print_hex,tabs+"    ");
		print tabs+"}";
		
		pass;
	




class cli_get_lottery_count_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public count;


	"""

	def read_from_buf(self, ba ):
		self.count=ba.read_uint32();
		if ( self.count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class cli_get_spec_item_in :
	"""
参数列表:
	/* 物品ID（3战斗积分4在线时长5打工次数8圣诞礼券） */
	#类型:uint32
	public itemid;


	"""

	def read_from_buf(self, ba ):
		self.itemid=ba.read_uint32();
		if ( self.itemid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.itemid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 物品ID（3战斗积分4在线时长5打工次数8圣诞礼券） */"""
		print tabs+"[itemid]=%s"%(str(self.itemid))
		
		pass;
	




class cli_get_spec_item_list_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public startid;

	/*  */
	#类型:uint32
	public endid;


	"""

	def read_from_buf(self, ba ):
		self.startid=ba.read_uint32();
		if ( self.startid== None): return False;
		self.endid=ba.read_uint32();
		if ( self.endid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.startid)):return False;
		if ( not ba.write_uint32(self.endid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[startid]=%s"%(str(self.startid))
		print tabs+"""/*  */"""
		print tabs+"[endid]=%s"%(str(self.endid))
		
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
	




class cli_get_spec_item_list_out :
	"""
参数列表:
	/**/
	#变长数组,最大长度:1000, 类型:item_day_limit_t
	public spec_item_list =array();


	"""

	def read_from_buf(self, ba ):

		spec_item_list_count= ba.read_uint32();
		if ( spec_item_list_count==None ): return False;
		self.spec_item_list=[];
		for i in range(spec_item_list_count):
			self.spec_item_list.append(item_day_limit_t());
			if ( not self.spec_item_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		spec_item_list_count=len(self.spec_item_list);
		ba.write_uint32(spec_item_list_count);
		for i in range(spec_item_list_count):
			if (  not  isinstance(self.spec_item_list[i] ,item_day_limit_t) ):return False; 
			if ( not self.spec_item_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"#"
		print tabs+"[spec_item_list]=>{";
		for i in range(len(self.spec_item_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.spec_item_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class cli_get_spec_item_out :
	"""
参数列表:
	/* 物品数量 */
	#类型:item_day_limit_t
	public item;


	"""

	def read_from_buf(self, ba ):
		self.item=item_day_limit_t();
		if ( not self.item.read_from_buf(ba)) :return False;

		return True;


	def write_to_buf(self,ba):
		if ( not isinstance(self.item,item_day_limit_t)): return False; 
		if ( not self.item.write_to_buf(ba)): return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 物品数量 */""";
		print tabs+"[item]=>{";
		self.item.echo(print_hex,tabs+"    ");
		print tabs+"}";
		
		pass;
	




class cli_get_user_game_stat_in :
	"""
参数列表:
	/* 目标userid */
	#类型:uint32
	public obj_userid;


	"""

	def read_from_buf(self, ba ):
		self.obj_userid=ba.read_uint32();
		if ( self.obj_userid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.obj_userid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 目标userid */"""
		print tabs+"[obj_userid]=%s"%(str(self.obj_userid))
		
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
	




class cli_get_user_game_stat_out :
	"""
参数列表:
	/* 目标userid */
	#类型:uint32
	public obj_userid;

	/*小游戏统计信息*/
	#变长数组,最大长度:9999, 类型:game_info_t
	public game_stat_list =array();


	"""

	def read_from_buf(self, ba ):
		self.obj_userid=ba.read_uint32();
		if ( self.obj_userid== None): return False;

		game_stat_list_count= ba.read_uint32();
		if ( game_stat_list_count==None ): return False;
		self.game_stat_list=[];
		for i in range(game_stat_list_count):
			self.game_stat_list.append(game_info_t());
			if ( not self.game_stat_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.obj_userid)):return False;
		game_stat_list_count=len(self.game_stat_list);
		ba.write_uint32(game_stat_list_count);
		for i in range(game_stat_list_count):
			if (  not  isinstance(self.game_stat_list[i] ,game_info_t) ):return False; 
			if ( not self.game_stat_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 目标userid */"""
		print tabs+"[obj_userid]=%s"%(str(self.obj_userid))
		print tabs+"#小游戏统计信息"
		print tabs+"[game_stat_list]=>{";
		for i in range(len(self.game_stat_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.game_stat_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class cli_get_user_info_in :
	"""
参数列表:
	/* 目标userid */
	#类型:uint32
	public obj_userid;


	"""

	def read_from_buf(self, ba ):
		self.obj_userid=ba.read_uint32();
		if ( self.obj_userid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.obj_userid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 目标userid */"""
		print tabs+"[obj_userid]=%s"%(str(self.obj_userid))
		
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
	




class cli_get_user_info_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public regtime;

	/* 肤色 */
	#类型:uint32
	public color;

	/*  */
	#定长数组,长度:16, 类型:char 
	public nick ;

	/* 剩余多少钱 */
	#类型:uint32
	public left_xiaomee;

	/* 等级 */
	#类型:uint32
	public level;

	/* 经验值 */
	#类型:uint32
	public experience;

	/* 小游戏积分 */
	#类型:uint32
	public game_point;

	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public use_clothes_list =array();

	/*使用中的特效卡片列表*/
	#变长数组,最大长度:999, 类型:uint32_t
	public effect_list =array();

	/*用户足迹列表*/
	#变长数组,最大长度:9999, 类型:user_log_t
	public user_log_list =array();

	/*完成 岛的列表*/
	#变长数组,最大长度:9999, 类型:uint32_t
	public complete_islandid_list =array();


	"""

	def read_from_buf(self, ba ):
		self.regtime=ba.read_uint32();
		if ( self.regtime== None): return False;
		self.color=ba.read_uint32();
		if ( self.color== None): return False;
		self.nick=ba.read_buf(16);
		if( self.nick==None ): return False;
		self.left_xiaomee=ba.read_uint32();
		if ( self.left_xiaomee== None): return False;
		self.level=ba.read_uint32();
		if ( self.level== None): return False;
		self.experience=ba.read_uint32();
		if ( self.experience== None): return False;
		self.game_point=ba.read_uint32();
		if ( self.game_point== None): return False;

		use_clothes_list_count= ba.read_uint32();
		if ( use_clothes_list_count==None ): return False;
		self.use_clothes_list=[];
		for i in range(use_clothes_list_count):
			self.use_clothes_list.append(ba.read_uint32());
			if ( self.use_clothes_list[i]== None): return False;


		effect_list_count= ba.read_uint32();
		if ( effect_list_count==None ): return False;
		self.effect_list=[];
		for i in range(effect_list_count):
			self.effect_list.append(ba.read_uint32());
			if ( self.effect_list[i]== None): return False;


		user_log_list_count= ba.read_uint32();
		if ( user_log_list_count==None ): return False;
		self.user_log_list=[];
		for i in range(user_log_list_count):
			self.user_log_list.append(user_log_t());
			if ( not self.user_log_list[i].read_from_buf(ba)):return False;


		complete_islandid_list_count= ba.read_uint32();
		if ( complete_islandid_list_count==None ): return False;
		self.complete_islandid_list=[];
		for i in range(complete_islandid_list_count):
			self.complete_islandid_list.append(ba.read_uint32());
			if ( self.complete_islandid_list[i]== None): return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.regtime)):return False;
		if ( not ba.write_uint32(self.color)):return False;
		ba.write_buf(self.nick,16);
		if ( not ba.write_uint32(self.left_xiaomee)):return False;
		if ( not ba.write_uint32(self.level)):return False;
		if ( not ba.write_uint32(self.experience)):return False;
		if ( not ba.write_uint32(self.game_point)):return False;
		use_clothes_list_count=len(self.use_clothes_list);
		ba.write_uint32(use_clothes_list_count);
		for i in range(use_clothes_list_count):
			if ( not ba.write_uint32(self.use_clothes_list[i])):return False;

		effect_list_count=len(self.effect_list);
		ba.write_uint32(effect_list_count);
		for i in range(effect_list_count):
			if ( not ba.write_uint32(self.effect_list[i])):return False;

		user_log_list_count=len(self.user_log_list);
		ba.write_uint32(user_log_list_count);
		for i in range(user_log_list_count):
			if (  not  isinstance(self.user_log_list[i] ,user_log_t) ):return False; 
			if ( not self.user_log_list[i].write_to_buf(ba)): return False;

		complete_islandid_list_count=len(self.complete_islandid_list);
		ba.write_uint32(complete_islandid_list_count);
		for i in range(complete_islandid_list_count):
			if ( not ba.write_uint32(self.complete_islandid_list[i])):return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[regtime]=%s"%(str(self.regtime))
		print tabs+"""/* 肤色 */"""
		print tabs+"[color]=%s"%(str(self.color))
		print tabs+"""/*  */"""
		tmp_str=str(self.nick);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[nick]=%s"%(tmp_str)
		print tabs+"""/* 剩余多少钱 */"""
		print tabs+"[left_xiaomee]=%s"%(str(self.left_xiaomee))
		print tabs+"""/* 等级 */"""
		print tabs+"[level]=%s"%(str(self.level))
		print tabs+"""/* 经验值 */"""
		print tabs+"[experience]=%s"%(str(self.experience))
		print tabs+"""/* 小游戏积分 */"""
		print tabs+"[game_point]=%s"%(str(self.game_point))
		print tabs+"#"
		print tabs+"[use_clothes_list]=>{";
		for i in range(len(self.use_clothes_list)):
			print tabs+"    [%d]="%(i)+str(self.use_clothes_list[i]); 

		print tabs+'}'
		print tabs+"#使用中的特效卡片列表"
		print tabs+"[effect_list]=>{";
		for i in range(len(self.effect_list)):
			print tabs+"    [%d]="%(i)+str(self.effect_list[i]); 

		print tabs+'}'
		print tabs+"#用户足迹列表"
		print tabs+"[user_log_list]=>{";
		for i in range(len(self.user_log_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.user_log_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		print tabs+"#完成 岛的列表"
		print tabs+"[complete_islandid_list]=>{";
		for i in range(len(self.complete_islandid_list)):
			print tabs+"    [%d]="%(i)+str(self.complete_islandid_list[i]); 

		print tabs+'}'
		
		pass;
	




class cli_get_user_island_find_map_info_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public islandid;


	"""

	def read_from_buf(self, ba ):
		self.islandid=ba.read_uint32();
		if ( self.islandid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.islandid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[islandid]=%s"%(str(self.islandid))
		
		pass;
	




class cli_get_user_island_find_map_info_out :
	"""
参数列表:
	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public find_map_list =array();


	"""

	def read_from_buf(self, ba ):

		find_map_list_count= ba.read_uint32();
		if ( find_map_list_count==None ): return False;
		self.find_map_list=[];
		for i in range(find_map_list_count):
			self.find_map_list.append(ba.read_uint32());
			if ( self.find_map_list[i]== None): return False;


		return True;


	def write_to_buf(self,ba):
		find_map_list_count=len(self.find_map_list);
		ba.write_uint32(find_map_list_count);
		for i in range(find_map_list_count):
			if ( not ba.write_uint32(self.find_map_list[i])):return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"#"
		print tabs+"[find_map_list]=>{";
		for i in range(len(self.find_map_list)):
			print tabs+"    [%d]="%(i)+str(self.find_map_list[i]); 

		print tabs+'}'
		
		pass;
	




class cli_get_user_island_task_info_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public islandid;


	"""

	def read_from_buf(self, ba ):
		self.islandid=ba.read_uint32();
		if ( self.islandid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.islandid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[islandid]=%s"%(str(self.islandid))
		
		pass;
	




class cli_get_user_island_task_info_out :
	"""
参数列表:
	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public task_nodeid_list =array();


	"""

	def read_from_buf(self, ba ):

		task_nodeid_list_count= ba.read_uint32();
		if ( task_nodeid_list_count==None ): return False;
		self.task_nodeid_list=[];
		for i in range(task_nodeid_list_count):
			self.task_nodeid_list.append(ba.read_uint32());
			if ( self.task_nodeid_list[i]== None): return False;


		return True;


	def write_to_buf(self,ba):
		task_nodeid_list_count=len(self.task_nodeid_list);
		ba.write_uint32(task_nodeid_list_count);
		for i in range(task_nodeid_list_count):
			if ( not ba.write_uint32(self.task_nodeid_list[i])):return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"#"
		print tabs+"[task_nodeid_list]=>{";
		for i in range(len(self.task_nodeid_list)):
			print tabs+"    [%d]="%(i)+str(self.task_nodeid_list[i]); 

		print tabs+'}'
		
		pass;
	




class cli_get_valid_gamept_out :
	"""
参数列表:
	/* 剩余可用游戏积分 */
	#类型:uint32
	public left_gamept;


	"""

	def read_from_buf(self, ba ):
		self.left_gamept=ba.read_uint32();
		if ( self.left_gamept== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.left_gamept)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 剩余可用游戏积分 */"""
		print tabs+"[left_gamept]=%s"%(str(self.left_gamept))
		
		pass;
	




class cli_hang_bell_get_item_in :
	"""
参数列表:
	/* 铃铛类型(0五彩1金色2银色) */
	#类型:uint32
	public bell_type;

	/* 投中区域(0蓝1绿2红3没投中) */
	#类型:uint32
	public zone_type;


	"""

	def read_from_buf(self, ba ):
		self.bell_type=ba.read_uint32();
		if ( self.bell_type== None): return False;
		self.zone_type=ba.read_uint32();
		if ( self.zone_type== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.bell_type)):return False;
		if ( not ba.write_uint32(self.zone_type)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 铃铛类型(0五彩1金色2银色) */"""
		print tabs+"[bell_type]=%s"%(str(self.bell_type))
		print tabs+"""/* 投中区域(0蓝1绿2红3没投中) */"""
		print tabs+"[zone_type]=%s"%(str(self.zone_type))
		
		pass;
	




class cli_hang_bell_get_item_out :
	"""
参数列表:
	/* 获取的物品（2卜克豆） */
	#类型:item_t
	public item_get;


	"""

	def read_from_buf(self, ba ):
		self.item_get=item_t();
		if ( not self.item_get.read_from_buf(ba)) :return False;

		return True;


	def write_to_buf(self,ba):
		if ( not isinstance(self.item_get,item_t)): return False; 
		if ( not self.item_get.write_to_buf(ba)): return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 获取的物品（2卜克豆） */""";
		print tabs+"[item_get]=>{";
		self.item_get.echo(print_hex,tabs+"    ");
		print tabs+"}";
		
		pass;
	




class cli_login_in :
	"""
参数列表:
	/* 服务器编号 */
	#类型:uint32
	public server_id;

	/* 米米号 */
	#类型:uint32
	public login_userid;

	/* 用于跳转时无需密码 */
	#定长数组,长度:32, 类型:char 
	public session ;


	"""

	def read_from_buf(self, ba ):
		self.server_id=ba.read_uint32();
		if ( self.server_id== None): return False;
		self.login_userid=ba.read_uint32();
		if ( self.login_userid== None): return False;
		self.session=ba.read_buf(32);
		if( self.session==None ): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.server_id)):return False;
		if ( not ba.write_uint32(self.login_userid)):return False;
		ba.write_buf(self.session,32);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 服务器编号 */"""
		print tabs+"[server_id]=%s"%(str(self.server_id))
		print tabs+"""/* 米米号 */"""
		print tabs+"[login_userid]=%s"%(str(self.login_userid))
		print tabs+"""/* 用于跳转时无需密码 */"""
		tmp_str=str(self.session);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[session]=%s"%(tmp_str)
		
		pass;
	




class cli_login_out :
	"""
参数列表:
	/* 按位标记：0-》注册 1-》新注册米米号 2-》聊天样本 3-》点击过指引 */
	#类型:uint32
	public regflag;

	/* 剩余多少钱 */
	#类型:uint32
	public left_xiaomee;

	/* 肤色 */
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

	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public task_nodeid_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public use_clothes_list =array();

	/*使用中的特效卡片列表*/
	#变长数组,最大长度:999, 类型:uint32_t
	public effect_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public find_map_list =array();


	"""

	def read_from_buf(self, ba ):
		self.regflag=ba.read_uint32();
		if ( self.regflag== None): return False;
		self.left_xiaomee=ba.read_uint32();
		if ( self.left_xiaomee== None): return False;
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

		task_nodeid_list_count= ba.read_uint32();
		if ( task_nodeid_list_count==None ): return False;
		self.task_nodeid_list=[];
		for i in range(task_nodeid_list_count):
			self.task_nodeid_list.append(ba.read_uint32());
			if ( self.task_nodeid_list[i]== None): return False;


		use_clothes_list_count= ba.read_uint32();
		if ( use_clothes_list_count==None ): return False;
		self.use_clothes_list=[];
		for i in range(use_clothes_list_count):
			self.use_clothes_list.append(ba.read_uint32());
			if ( self.use_clothes_list[i]== None): return False;


		effect_list_count= ba.read_uint32();
		if ( effect_list_count==None ): return False;
		self.effect_list=[];
		for i in range(effect_list_count):
			self.effect_list.append(ba.read_uint32());
			if ( self.effect_list[i]== None): return False;


		find_map_list_count= ba.read_uint32();
		if ( find_map_list_count==None ): return False;
		self.find_map_list=[];
		for i in range(find_map_list_count):
			self.find_map_list.append(ba.read_uint32());
			if ( self.find_map_list[i]== None): return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.regflag)):return False;
		if ( not ba.write_uint32(self.left_xiaomee)):return False;
		if ( not ba.write_uint32(self.color)):return False;
		if ( not ba.write_uint32(self.age)):return False;
		ba.write_buf(self.nick,16);
		if ( not ba.write_uint32(self.last_islandid)):return False;
		if ( not ba.write_uint32(self.last_mapid)):return False;
		if ( not ba.write_uint32(self.last_x)):return False;
		if ( not ba.write_uint32(self.last_y)):return False;
		task_nodeid_list_count=len(self.task_nodeid_list);
		ba.write_uint32(task_nodeid_list_count);
		for i in range(task_nodeid_list_count):
			if ( not ba.write_uint32(self.task_nodeid_list[i])):return False;

		use_clothes_list_count=len(self.use_clothes_list);
		ba.write_uint32(use_clothes_list_count);
		for i in range(use_clothes_list_count):
			if ( not ba.write_uint32(self.use_clothes_list[i])):return False;

		effect_list_count=len(self.effect_list);
		ba.write_uint32(effect_list_count);
		for i in range(effect_list_count):
			if ( not ba.write_uint32(self.effect_list[i])):return False;

		find_map_list_count=len(self.find_map_list);
		ba.write_uint32(find_map_list_count);
		for i in range(find_map_list_count):
			if ( not ba.write_uint32(self.find_map_list[i])):return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 按位标记：0-》注册 1-》新注册米米号 2-》聊天样本 3-》点击过指引 */"""
		print tabs+"[regflag]=%s"%(str(self.regflag))
		print tabs+"""/* 剩余多少钱 */"""
		print tabs+"[left_xiaomee]=%s"%(str(self.left_xiaomee))
		print tabs+"""/* 肤色 */"""
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
		print tabs+"#"
		print tabs+"[task_nodeid_list]=>{";
		for i in range(len(self.task_nodeid_list)):
			print tabs+"    [%d]="%(i)+str(self.task_nodeid_list[i]); 

		print tabs+'}'
		print tabs+"#"
		print tabs+"[use_clothes_list]=>{";
		for i in range(len(self.use_clothes_list)):
			print tabs+"    [%d]="%(i)+str(self.use_clothes_list[i]); 

		print tabs+'}'
		print tabs+"#使用中的特效卡片列表"
		print tabs+"[effect_list]=>{";
		for i in range(len(self.effect_list)):
			print tabs+"    [%d]="%(i)+str(self.effect_list[i]); 

		print tabs+'}'
		print tabs+"#"
		print tabs+"[find_map_list]=>{";
		for i in range(len(self.find_map_list)):
			print tabs+"    [%d]="%(i)+str(self.find_map_list[i]); 

		print tabs+'}'
		
		pass;
	




class cli_noti_effect_used_out :
	"""
参数列表:
	/* 发起的userid */
	#类型:uint32
	public src_userid;

	/* 物品ID */
	#类型:uint32
	public itemid;

	/* 要取消的特效卡片ID（没有为0） */
	#类型:uint32
	public unset_itemid;


	"""

	def read_from_buf(self, ba ):
		self.src_userid=ba.read_uint32();
		if ( self.src_userid== None): return False;
		self.itemid=ba.read_uint32();
		if ( self.itemid== None): return False;
		self.unset_itemid=ba.read_uint32();
		if ( self.unset_itemid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.src_userid)):return False;
		if ( not ba.write_uint32(self.itemid)):return False;
		if ( not ba.write_uint32(self.unset_itemid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 发起的userid */"""
		print tabs+"[src_userid]=%s"%(str(self.src_userid))
		print tabs+"""/* 物品ID */"""
		print tabs+"[itemid]=%s"%(str(self.itemid))
		print tabs+"""/* 要取消的特效卡片ID（没有为0） */"""
		print tabs+"[unset_itemid]=%s"%(str(self.unset_itemid))
		
		pass;
	




class cli_noti_first_enter_island_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public islandid;

	/* 获得卜克豆 */
	#类型:uint32
	public xiaomee;

	/* 是否新注册的米米号 */
	#类型:uint32
	public is_newid;


	"""

	def read_from_buf(self, ba ):
		self.islandid=ba.read_uint32();
		if ( self.islandid== None): return False;
		self.xiaomee=ba.read_uint32();
		if ( self.xiaomee== None): return False;
		self.is_newid=ba.read_uint32();
		if ( self.is_newid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.islandid)):return False;
		if ( not ba.write_uint32(self.xiaomee)):return False;
		if ( not ba.write_uint32(self.is_newid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[islandid]=%s"%(str(self.islandid))
		print tabs+"""/* 获得卜克豆 */"""
		print tabs+"[xiaomee]=%s"%(str(self.xiaomee))
		print tabs+"""/* 是否新注册的米米号 */"""
		print tabs+"[is_newid]=%s"%(str(self.is_newid))
		
		pass;
	




class user_game_win_t :
	"""
参数列表:
	/* 目标userid */
	#类型:uint32
	public obj_userid;

	/* 输羸 :0:不是,1:是,2:平局 */
	#类型:uint32
	public win_flag;

	/* 正数得到负数减去 */
	#类型:int32
	public get_point;


	"""

	def read_from_buf(self, ba ):
		self.obj_userid=ba.read_uint32();
		if ( self.obj_userid== None): return False;
		self.win_flag=ba.read_uint32();
		if ( self.win_flag== None): return False;
		self.get_point=ba.read_int32();
		if ( self.get_point== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.obj_userid)):return False;
		if ( not ba.write_uint32(self.win_flag)):return False;
		if ( not ba.write_int32(self.get_point)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 目标userid */"""
		print tabs+"[obj_userid]=%s"%(str(self.obj_userid))
		print tabs+"""/* 输羸 :0:不是,1:是,2:平局 */"""
		print tabs+"[win_flag]=%s"%(str(self.win_flag))
		print tabs+"""/* 正数得到负数减去 */"""
		print tabs+"[get_point]=%s"%(str(self.get_point))
		
		pass;
	




class cli_noti_game_end_out :
	"""
参数列表:
	/**/
	#变长数组,最大长度:10, 类型:user_game_win_t
	public user_win_list =array();


	"""

	def read_from_buf(self, ba ):

		user_win_list_count= ba.read_uint32();
		if ( user_win_list_count==None ): return False;
		self.user_win_list=[];
		for i in range(user_win_list_count):
			self.user_win_list.append(user_game_win_t());
			if ( not self.user_win_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		user_win_list_count=len(self.user_win_list);
		ba.write_uint32(user_win_list_count);
		for i in range(user_win_list_count):
			if (  not  isinstance(self.user_win_list[i] ,user_game_win_t) ):return False; 
			if ( not self.user_win_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"#"
		print tabs+"[user_win_list]=>{";
		for i in range(len(self.user_win_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.user_win_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class cli_noti_game_is_start_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public gameid;

	/* 是否开始,0:否(终止游戏), 1:是 */
	#类型:uint32
	public is_start;

	/* 为什么不玩, 1:自己关，2：在换装，3：购物 */
	#类型:uint32
	public why_not_start_flag;

	/* 目标userid */
	#类型:uint32
	public obj_userid;


	"""

	def read_from_buf(self, ba ):
		self.gameid=ba.read_uint32();
		if ( self.gameid== None): return False;
		self.is_start=ba.read_uint32();
		if ( self.is_start== None): return False;
		self.why_not_start_flag=ba.read_uint32();
		if ( self.why_not_start_flag== None): return False;
		self.obj_userid=ba.read_uint32();
		if ( self.obj_userid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.gameid)):return False;
		if ( not ba.write_uint32(self.is_start)):return False;
		if ( not ba.write_uint32(self.why_not_start_flag)):return False;
		if ( not ba.write_uint32(self.obj_userid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[gameid]=%s"%(str(self.gameid))
		print tabs+"""/* 是否开始,0:否(终止游戏), 1:是 */"""
		print tabs+"[is_start]=%s"%(str(self.is_start))
		print tabs+"""/* 为什么不玩, 1:自己关，2：在换装，3：购物 */"""
		print tabs+"[why_not_start_flag]=%s"%(str(self.why_not_start_flag))
		print tabs+"""/* 目标userid */"""
		print tabs+"[obj_userid]=%s"%(str(self.obj_userid))
		
		pass;
	




class cli_noti_game_opt_out :
	"""
参数列表:
	/* 发起的userid */
	#类型:uint32
	public src_userid;

	/*游戏操作信息*/
	#变长数组,最大长度:9999, 类型:char
	public gamemsg ;


	"""

	def read_from_buf(self, ba ):
		self.src_userid=ba.read_uint32();
		if ( self.src_userid== None): return False;

		gamemsg_count= ba.read_uint32();
		if ( gamemsg_count==None ): return False;
		self.gamemsg=ba.read_buf(gamemsg_count);
		if( self.gamemsg==None ):return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.src_userid)):return False;
		gamemsg_count=len(self.gamemsg);
		ba.write_uint32(gamemsg_count);
		ba.write_buf(self.gamemsg,gamemsg_count);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 发起的userid */"""
		print tabs+"[src_userid]=%s"%(str(self.src_userid))
		print tabs+"""/* 游戏操作信息 */"""
		print tabs+"[gamemsg]=%s"%(str(self.gamemsg))
		
		pass;
	




class cli_noti_game_request_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public gameid;

	/* 发起的userid */
	#类型:uint32
	public src_userid;

	/* 该请求的key, 在 确认时将之一起发出 */
	#定长数组,长度:32, 类型:char 
	public request_session ;


	"""

	def read_from_buf(self, ba ):
		self.gameid=ba.read_uint32();
		if ( self.gameid== None): return False;
		self.src_userid=ba.read_uint32();
		if ( self.src_userid== None): return False;
		self.request_session=ba.read_buf(32);
		if( self.request_session==None ): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.gameid)):return False;
		if ( not ba.write_uint32(self.src_userid)):return False;
		ba.write_buf(self.request_session,32);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[gameid]=%s"%(str(self.gameid))
		print tabs+"""/* 发起的userid */"""
		print tabs+"[src_userid]=%s"%(str(self.src_userid))
		print tabs+"""/* 该请求的key, 在 确认时将之一起发出 */"""
		tmp_str=str(self.request_session);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[request_session]=%s"%(tmp_str)
		
		pass;
	




class game_seat_t :
	"""
参数列表:
	/* 项目编号 */
	#类型:uint32
	public gameid;

	/* 0左1右 */
	#类型:uint32
	public side;

	/* 在座位上的米米号 */
	#类型:uint32
	public userid;


	"""

	def read_from_buf(self, ba ):
		self.gameid=ba.read_uint32();
		if ( self.gameid== None): return False;
		self.side=ba.read_uint32();
		if ( self.side== None): return False;
		self.userid=ba.read_uint32();
		if ( self.userid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.gameid)):return False;
		if ( not ba.write_uint32(self.side)):return False;
		if ( not ba.write_uint32(self.userid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 项目编号 */"""
		print tabs+"[gameid]=%s"%(str(self.gameid))
		print tabs+"""/* 0左1右 */"""
		print tabs+"[side]=%s"%(str(self.side))
		print tabs+"""/* 在座位上的米米号 */"""
		print tabs+"[userid]=%s"%(str(self.userid))
		
		pass;
	




class cli_noti_game_seat_out :
	"""
参数列表:
	/*小游戏列表*/
	#变长数组,最大长度:100, 类型:game_seat_t
	public gamelist =array();


	"""

	def read_from_buf(self, ba ):

		gamelist_count= ba.read_uint32();
		if ( gamelist_count==None ): return False;
		self.gamelist=[];
		for i in range(gamelist_count):
			self.gamelist.append(game_seat_t());
			if ( not self.gamelist[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		gamelist_count=len(self.gamelist);
		ba.write_uint32(gamelist_count);
		for i in range(gamelist_count):
			if (  not  isinstance(self.gamelist[i] ,game_seat_t) ):return False; 
			if ( not self.gamelist[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"#小游戏列表"
		print tabs+"[gamelist]=>{";
		for i in range(len(self.gamelist)):
			print tabs+"    [%d]=>{"%(i) 
			self.gamelist[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class cli_noti_game_start_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public gameid;

	/*一起玩的userid列表*/
	#变长数组,最大长度:100, 类型:uint32_t
	public userid_list =array();

	/* 第一步的用户,如果为0，则没有限定 */
	#类型:uint32
	public start_userid;

	/* 每一步超时时间 ,0：不限定 */
	#类型:uint32
	public step_timeout;


	"""

	def read_from_buf(self, ba ):
		self.gameid=ba.read_uint32();
		if ( self.gameid== None): return False;

		userid_list_count= ba.read_uint32();
		if ( userid_list_count==None ): return False;
		self.userid_list=[];
		for i in range(userid_list_count):
			self.userid_list.append(ba.read_uint32());
			if ( self.userid_list[i]== None): return False;

		self.start_userid=ba.read_uint32();
		if ( self.start_userid== None): return False;
		self.step_timeout=ba.read_uint32();
		if ( self.step_timeout== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.gameid)):return False;
		userid_list_count=len(self.userid_list);
		ba.write_uint32(userid_list_count);
		for i in range(userid_list_count):
			if ( not ba.write_uint32(self.userid_list[i])):return False;

		if ( not ba.write_uint32(self.start_userid)):return False;
		if ( not ba.write_uint32(self.step_timeout)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[gameid]=%s"%(str(self.gameid))
		print tabs+"#一起玩的userid列表"
		print tabs+"[userid_list]=>{";
		for i in range(len(self.userid_list)):
			print tabs+"    [%d]="%(i)+str(self.userid_list[i]); 

		print tabs+'}'
		print tabs+"""/* 第一步的用户,如果为0，则没有限定 */"""
		print tabs+"[start_userid]=%s"%(str(self.start_userid))
		print tabs+"""/* 每一步超时时间 ,0：不限定 */"""
		print tabs+"[step_timeout]=%s"%(str(self.step_timeout))
		
		pass;
	




class cli_noti_game_user_left_game_out :
	"""
参数列表:
	/* 目标userid */
	#类型:uint32
	public obj_userid;


	"""

	def read_from_buf(self, ba ):
		self.obj_userid=ba.read_uint32();
		if ( self.obj_userid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.obj_userid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 目标userid */"""
		print tabs+"[obj_userid]=%s"%(str(self.obj_userid))
		
		pass;
	




class cli_noti_game_user_out :
	"""
参数列表:
	/* 状态（1开始2结束） */
	#类型:uint32
	public state;

	/*  */
	#类型:game_user_t
	public game_user;


	"""

	def read_from_buf(self, ba ):
		self.state=ba.read_uint32();
		if ( self.state== None): return False;
		self.game_user=game_user_t();
		if ( not self.game_user.read_from_buf(ba)) :return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.state)):return False;
		if ( not isinstance(self.game_user,game_user_t)): return False; 
		if ( not self.game_user.write_to_buf(ba)): return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 状态（1开始2结束） */"""
		print tabs+"[state]=%s"%(str(self.state))
		print tabs+"""/*  */""";
		print tabs+"[game_user]=>{";
		self.game_user.echo(print_hex,tabs+"    ");
		print tabs+"}";
		
		pass;
	




class cli_noti_get_invitation_out :
	"""
参数列表:
	/* 物品邀请函ID */
	#类型:uint32
	public itemid;


	"""

	def read_from_buf(self, ba ):
		self.itemid=ba.read_uint32();
		if ( self.itemid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.itemid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 物品邀请函ID */"""
		print tabs+"[itemid]=%s"%(str(self.itemid))
		
		pass;
	




class cli_noti_get_item_list_out :
	"""
参数列表:
	/*得到的物品列表*/
	#变长数组,最大长度:9999, 类型:item_t
	public item_list =array();


	"""

	def read_from_buf(self, ba ):

		item_list_count= ba.read_uint32();
		if ( item_list_count==None ): return False;
		self.item_list=[];
		for i in range(item_list_count):
			self.item_list.append(item_t());
			if ( not self.item_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		item_list_count=len(self.item_list);
		ba.write_uint32(item_list_count);
		for i in range(item_list_count):
			if (  not  isinstance(self.item_list[i] ,item_t) ):return False; 
			if ( not self.item_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"#得到的物品列表"
		print tabs+"[item_list]=>{";
		for i in range(len(self.item_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.item_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class cli_noti_island_complete_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public userid;

	/*  */
	#定长数组,长度:16, 类型:char 
	public nick ;

	/*  */
	#类型:uint32
	public islandid;


	"""

	def read_from_buf(self, ba ):
		self.userid=ba.read_uint32();
		if ( self.userid== None): return False;
		self.nick=ba.read_buf(16);
		if( self.nick==None ): return False;
		self.islandid=ba.read_uint32();
		if ( self.islandid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.userid)):return False;
		ba.write_buf(self.nick,16);
		if ( not ba.write_uint32(self.islandid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[userid]=%s"%(str(self.userid))
		print tabs+"""/*  */"""
		tmp_str=str(self.nick);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[nick]=%s"%(tmp_str)
		print tabs+"""/*  */"""
		print tabs+"[islandid]=%s"%(str(self.islandid))
		
		pass;
	




class cli_noti_leave_island_out :
	"""
参数列表:
	/* 当前岛ID */
	#类型:uint32
	public cur_island;

	/* 当前地图ID */
	#类型:uint32
	public cur_mapid;

	/* 前往的岛屿ID */
	#类型:uint32
	public islandid;

	/* 前往的地图ID */
	#类型:uint32
	public mapid;


	"""

	def read_from_buf(self, ba ):
		self.cur_island=ba.read_uint32();
		if ( self.cur_island== None): return False;
		self.cur_mapid=ba.read_uint32();
		if ( self.cur_mapid== None): return False;
		self.islandid=ba.read_uint32();
		if ( self.islandid== None): return False;
		self.mapid=ba.read_uint32();
		if ( self.mapid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.cur_island)):return False;
		if ( not ba.write_uint32(self.cur_mapid)):return False;
		if ( not ba.write_uint32(self.islandid)):return False;
		if ( not ba.write_uint32(self.mapid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 当前岛ID */"""
		print tabs+"[cur_island]=%s"%(str(self.cur_island))
		print tabs+"""/* 当前地图ID */"""
		print tabs+"[cur_mapid]=%s"%(str(self.cur_mapid))
		print tabs+"""/* 前往的岛屿ID */"""
		print tabs+"[islandid]=%s"%(str(self.islandid))
		print tabs+"""/* 前往的地图ID */"""
		print tabs+"[mapid]=%s"%(str(self.mapid))
		
		pass;
	




class cli_noti_one_user_info_out :
	"""
参数列表:
	/*  */
	#类型:online_user_info_t
	public user_info;


	"""

	def read_from_buf(self, ba ):
		self.user_info=online_user_info_t();
		if ( not self.user_info.read_from_buf(ba)) :return False;

		return True;


	def write_to_buf(self,ba):
		if ( not isinstance(self.user_info,online_user_info_t)): return False; 
		if ( not self.user_info.write_to_buf(ba)): return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */""";
		print tabs+"[user_info]=>{";
		self.user_info.echo(print_hex,tabs+"    ");
		print tabs+"}";
		
		pass;
	




class cli_noti_set_color_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public userid;

	/* 肤色 */
	#类型:uint32
	public color;


	"""

	def read_from_buf(self, ba ):
		self.userid=ba.read_uint32();
		if ( self.userid== None): return False;
		self.color=ba.read_uint32();
		if ( self.color== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.userid)):return False;
		if ( not ba.write_uint32(self.color)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[userid]=%s"%(str(self.userid))
		print tabs+"""/* 肤色 */"""
		print tabs+"[color]=%s"%(str(self.color))
		
		pass;
	




class cli_noti_show_out :
	"""
参数列表:
	/* 发起的userid */
	#类型:uint32
	public src_userid;

	/* 表情ID */
	#类型:uint32
	public expression_id;

	/* 目标userid（0：场景 1游戏中） */
	#类型:uint32
	public obj_userid;


	"""

	def read_from_buf(self, ba ):
		self.src_userid=ba.read_uint32();
		if ( self.src_userid== None): return False;
		self.expression_id=ba.read_uint32();
		if ( self.expression_id== None): return False;
		self.obj_userid=ba.read_uint32();
		if ( self.obj_userid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.src_userid)):return False;
		if ( not ba.write_uint32(self.expression_id)):return False;
		if ( not ba.write_uint32(self.obj_userid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 发起的userid */"""
		print tabs+"[src_userid]=%s"%(str(self.src_userid))
		print tabs+"""/* 表情ID */"""
		print tabs+"[expression_id]=%s"%(str(self.expression_id))
		print tabs+"""/* 目标userid（0：场景 1游戏中） */"""
		print tabs+"[obj_userid]=%s"%(str(self.obj_userid))
		
		pass;
	




class cli_noti_svr_time_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public timestamp;


	"""

	def read_from_buf(self, ba ):
		self.timestamp=ba.read_uint32();
		if ( self.timestamp== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.timestamp)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[timestamp]=%s"%(str(self.timestamp))
		
		pass;
	




class cli_noti_talk_out :
	"""
参数列表:
	/* 发起的userid */
	#类型:uint32
	public src_userid;

	/*  */
	#定长数组,长度:16, 类型:char 
	public src_nick ;

	/* 目标userid */
	#类型:uint32
	public obj_userid;

	/**/
	#变长数组,最大长度:9999, 类型:char
	public msg ;


	"""

	def read_from_buf(self, ba ):
		self.src_userid=ba.read_uint32();
		if ( self.src_userid== None): return False;
		self.src_nick=ba.read_buf(16);
		if( self.src_nick==None ): return False;
		self.obj_userid=ba.read_uint32();
		if ( self.obj_userid== None): return False;

		msg_count= ba.read_uint32();
		if ( msg_count==None ): return False;
		self.msg=ba.read_buf(msg_count);
		if( self.msg==None ):return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.src_userid)):return False;
		ba.write_buf(self.src_nick,16);
		if ( not ba.write_uint32(self.obj_userid)):return False;
		msg_count=len(self.msg);
		ba.write_uint32(msg_count);
		ba.write_buf(self.msg,msg_count);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 发起的userid */"""
		print tabs+"[src_userid]=%s"%(str(self.src_userid))
		print tabs+"""/*  */"""
		tmp_str=str(self.src_nick);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[src_nick]=%s"%(tmp_str)
		print tabs+"""/* 目标userid */"""
		print tabs+"[obj_userid]=%s"%(str(self.obj_userid))
		print tabs+"""/*  */"""
		print tabs+"[msg]=%s"%(str(self.msg))
		
		pass;
	




class cli_noti_user_left_map_out :
	"""
参数列表:
	/* 目标userid */
	#类型:uint32
	public obj_userid;


	"""

	def read_from_buf(self, ba ):
		self.obj_userid=ba.read_uint32();
		if ( self.obj_userid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.obj_userid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 目标userid */"""
		print tabs+"[obj_userid]=%s"%(str(self.obj_userid))
		
		pass;
	




class cli_noti_user_level_up_out :
	"""
参数列表:
	/* 发起的userid */
	#类型:uint32
	public src_userid;

	/* 等级 */
	#类型:uint32
	public level;

	/* 经验值 */
	#类型:uint32
	public experience;

	/* 此次得到的经验 */
	#类型:uint32
	public addexp;

	/* 是否升级 */
	#类型:uint32
	public is_level_up;

	/* 升级获得的奖励物品 */
	#类型:uint32
	public itemid;


	"""

	def read_from_buf(self, ba ):
		self.src_userid=ba.read_uint32();
		if ( self.src_userid== None): return False;
		self.level=ba.read_uint32();
		if ( self.level== None): return False;
		self.experience=ba.read_uint32();
		if ( self.experience== None): return False;
		self.addexp=ba.read_uint32();
		if ( self.addexp== None): return False;
		self.is_level_up=ba.read_uint32();
		if ( self.is_level_up== None): return False;
		self.itemid=ba.read_uint32();
		if ( self.itemid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.src_userid)):return False;
		if ( not ba.write_uint32(self.level)):return False;
		if ( not ba.write_uint32(self.experience)):return False;
		if ( not ba.write_uint32(self.addexp)):return False;
		if ( not ba.write_uint32(self.is_level_up)):return False;
		if ( not ba.write_uint32(self.itemid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 发起的userid */"""
		print tabs+"[src_userid]=%s"%(str(self.src_userid))
		print tabs+"""/* 等级 */"""
		print tabs+"[level]=%s"%(str(self.level))
		print tabs+"""/* 经验值 */"""
		print tabs+"[experience]=%s"%(str(self.experience))
		print tabs+"""/* 此次得到的经验 */"""
		print tabs+"[addexp]=%s"%(str(self.addexp))
		print tabs+"""/* 是否升级 */"""
		print tabs+"[is_level_up]=%s"%(str(self.is_level_up))
		print tabs+"""/* 升级获得的奖励物品 */"""
		print tabs+"[itemid]=%s"%(str(self.itemid))
		
		pass;
	




class cli_noti_user_move_out :
	"""
参数列表:
	/* 目标userid */
	#类型:uint32
	public obj_userid;

	/*  */
	#类型:uint32
	public start_x;

	/*  */
	#类型:uint32
	public start_y;

	/*  */
	#类型:uint32
	public mouse_x;

	/*  */
	#类型:uint32
	public mouse_y;

	/* 移动模式 */
	#类型:uint32
	public type;


	"""

	def read_from_buf(self, ba ):
		self.obj_userid=ba.read_uint32();
		if ( self.obj_userid== None): return False;
		self.start_x=ba.read_uint32();
		if ( self.start_x== None): return False;
		self.start_y=ba.read_uint32();
		if ( self.start_y== None): return False;
		self.mouse_x=ba.read_uint32();
		if ( self.mouse_x== None): return False;
		self.mouse_y=ba.read_uint32();
		if ( self.mouse_y== None): return False;
		self.type=ba.read_uint32();
		if ( self.type== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.obj_userid)):return False;
		if ( not ba.write_uint32(self.start_x)):return False;
		if ( not ba.write_uint32(self.start_y)):return False;
		if ( not ba.write_uint32(self.mouse_x)):return False;
		if ( not ba.write_uint32(self.mouse_y)):return False;
		if ( not ba.write_uint32(self.type)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 目标userid */"""
		print tabs+"[obj_userid]=%s"%(str(self.obj_userid))
		print tabs+"""/*  */"""
		print tabs+"[start_x]=%s"%(str(self.start_x))
		print tabs+"""/*  */"""
		print tabs+"[start_y]=%s"%(str(self.start_y))
		print tabs+"""/*  */"""
		print tabs+"[mouse_x]=%s"%(str(self.mouse_x))
		print tabs+"""/*  */"""
		print tabs+"[mouse_y]=%s"%(str(self.mouse_y))
		print tabs+"""/* 移动模式 */"""
		print tabs+"[type]=%s"%(str(self.type))
		
		pass;
	




class cli_noti_user_online_time_out :
	"""
参数列表:
	/* 计时类型（0连续 1累计） */
	#类型:uint32
	public type;

	/* 在线时间（分钟） */
	#类型:uint32
	public time;


	"""

	def read_from_buf(self, ba ):
		self.type=ba.read_uint32();
		if ( self.type== None): return False;
		self.time=ba.read_uint32();
		if ( self.time== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.type)):return False;
		if ( not ba.write_uint32(self.time)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 计时类型（0连续 1累计） */"""
		print tabs+"[type]=%s"%(str(self.type))
		print tabs+"""/* 在线时间（分钟） */"""
		print tabs+"[time]=%s"%(str(self.time))
		
		pass;
	




class cli_noti_user_use_clothes_list_out :
	"""
参数列表:
	/* 目标userid */
	#类型:uint32
	public obj_userid;

	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public use_clothes_list =array();


	"""

	def read_from_buf(self, ba ):
		self.obj_userid=ba.read_uint32();
		if ( self.obj_userid== None): return False;

		use_clothes_list_count= ba.read_uint32();
		if ( use_clothes_list_count==None ): return False;
		self.use_clothes_list=[];
		for i in range(use_clothes_list_count):
			self.use_clothes_list.append(ba.read_uint32());
			if ( self.use_clothes_list[i]== None): return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.obj_userid)):return False;
		use_clothes_list_count=len(self.use_clothes_list);
		ba.write_uint32(use_clothes_list_count);
		for i in range(use_clothes_list_count):
			if ( not ba.write_uint32(self.use_clothes_list[i])):return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 目标userid */"""
		print tabs+"[obj_userid]=%s"%(str(self.obj_userid))
		print tabs+"#"
		print tabs+"[use_clothes_list]=>{";
		for i in range(len(self.use_clothes_list)):
			print tabs+"    [%d]="%(i)+str(self.use_clothes_list[i]); 

		print tabs+'}'
		
		pass;
	




class cli_post_msg_in :
	"""
参数列表:
	/* 投稿类型 */
	#类型:uint32
	public type;

	/* 主题 */
	#定长数组,长度:60, 类型:char 
	public title ;

	/*投稿内容*/
	#变长数组,最大长度:4096, 类型:char
	public msg ;


	"""

	def read_from_buf(self, ba ):
		self.type=ba.read_uint32();
		if ( self.type== None): return False;
		self.title=ba.read_buf(60);
		if( self.title==None ): return False;

		msg_count= ba.read_uint32();
		if ( msg_count==None ): return False;
		self.msg=ba.read_buf(msg_count);
		if( self.msg==None ):return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.type)):return False;
		ba.write_buf(self.title,60);
		msg_count=len(self.msg);
		ba.write_uint32(msg_count);
		ba.write_buf(self.msg,msg_count);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 投稿类型 */"""
		print tabs+"[type]=%s"%(str(self.type))
		print tabs+"""/* 主题 */"""
		tmp_str=str(self.title);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[title]=%s"%(tmp_str)
		print tabs+"""/* 投稿内容 */"""
		print tabs+"[msg]=%s"%(str(self.msg))
		
		pass;
	




class cli_proto_header_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public proto_length;

	/*  */
	#类型:uint16
	public cmdid;

	/*  */
	#类型:uint32
	public timestamp;

	/*  */
	#类型:uint32
	public seq;

	/*  */
	#类型:uint16
	public result;

	/*  */
	#类型:uint16
	public useSever;


	"""

	def read_from_buf(self, ba ):
		self.proto_length=ba.read_uint32();
		if ( self.proto_length== None): return False;
		self.cmdid=ba.read_uint16();
		if ( self.cmdid== None): return False;
		self.timestamp=ba.read_uint32();
		if ( self.timestamp== None): return False;
		self.seq=ba.read_uint32();
		if ( self.seq== None): return False;
		self.result=ba.read_uint16();
		if ( self.result== None): return False;
		self.useSever=ba.read_uint16();
		if ( self.useSever== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.proto_length)):return False;
		if ( not ba.write_uint16(self.cmdid)):return False;
		if ( not ba.write_uint32(self.timestamp)):return False;
		if ( not ba.write_uint32(self.seq)):return False;
		if ( not ba.write_uint16(self.result)):return False;
		if ( not ba.write_uint16(self.useSever)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[proto_length]=%s"%(str(self.proto_length))
		print tabs+"""/*  */"""
		print tabs+"[cmdid]=%s"%(str(self.cmdid))
		print tabs+"""/*  */"""
		print tabs+"[timestamp]=%s"%(str(self.timestamp))
		print tabs+"""/*  */"""
		print tabs+"[seq]=%s"%(str(self.seq))
		print tabs+"""/*  */"""
		print tabs+"[result]=%s"%(str(self.result))
		print tabs+"""/*  */"""
		print tabs+"[useSever]=%s"%(str(self.useSever))
		
		pass;
	




class cli_reg_in :
	"""
参数列表:
	/* 渠道id(填0) */
	#类型:uint32
	public channelid;

	/*  */
	#定长数组,长度:16, 类型:char 
	public nick ;

	/*  */
	#类型:uint32
	public age;

	/* 肤色 */
	#类型:uint32
	public color;

	/**/
	#变长数组,最大长度:9999, 类型:item_t
	public item_list =array();


	"""

	def read_from_buf(self, ba ):
		self.channelid=ba.read_uint32();
		if ( self.channelid== None): return False;
		self.nick=ba.read_buf(16);
		if( self.nick==None ): return False;
		self.age=ba.read_uint32();
		if ( self.age== None): return False;
		self.color=ba.read_uint32();
		if ( self.color== None): return False;

		item_list_count= ba.read_uint32();
		if ( item_list_count==None ): return False;
		self.item_list=[];
		for i in range(item_list_count):
			self.item_list.append(item_t());
			if ( not self.item_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.channelid)):return False;
		ba.write_buf(self.nick,16);
		if ( not ba.write_uint32(self.age)):return False;
		if ( not ba.write_uint32(self.color)):return False;
		item_list_count=len(self.item_list);
		ba.write_uint32(item_list_count);
		for i in range(item_list_count):
			if (  not  isinstance(self.item_list[i] ,item_t) ):return False; 
			if ( not self.item_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 渠道id(填0) */"""
		print tabs+"[channelid]=%s"%(str(self.channelid))
		print tabs+"""/*  */"""
		tmp_str=str(self.nick);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[nick]=%s"%(tmp_str)
		print tabs+"""/*  */"""
		print tabs+"[age]=%s"%(str(self.age))
		print tabs+"""/* 肤色 */"""
		print tabs+"[color]=%s"%(str(self.color))
		print tabs+"#"
		print tabs+"[item_list]=>{";
		for i in range(len(self.item_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.item_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class cli_reg_out :
	"""
参数列表:
	/* 剩余多少钱 */
	#类型:uint32
	public left_xiaomee;

	/* 昵称 */
	#定长数组,长度:16, 类型:char 
	public nick ;


	"""

	def read_from_buf(self, ba ):
		self.left_xiaomee=ba.read_uint32();
		if ( self.left_xiaomee== None): return False;
		self.nick=ba.read_buf(16);
		if( self.nick==None ): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.left_xiaomee)):return False;
		ba.write_buf(self.nick,16);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 剩余多少钱 */"""
		print tabs+"[left_xiaomee]=%s"%(str(self.left_xiaomee))
		print tabs+"""/* 昵称 */"""
		tmp_str=str(self.nick);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[nick]=%s"%(tmp_str)
		
		pass;
	




class cli_set_busy_state_in :
	"""
参数列表:
	/* 忙状态 */
	#类型:uint32
	public state;


	"""

	def read_from_buf(self, ba ):
		self.state=ba.read_uint32();
		if ( self.state== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.state)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 忙状态 */"""
		print tabs+"[state]=%s"%(str(self.state))
		
		pass;
	




class cli_set_color_in :
	"""
参数列表:
	/* 肤色 */
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
		print tabs+"""/* 肤色 */"""
		print tabs+"[color]=%s"%(str(self.color))
		
		pass;
	




class cli_set_effect_used_in :
	"""
参数列表:
	/* 物品ID */
	#类型:uint32
	public itemid;

	/* （1穿上 0去掉） */
	#类型:uint32
	public type;


	"""

	def read_from_buf(self, ba ):
		self.itemid=ba.read_uint32();
		if ( self.itemid== None): return False;
		self.type=ba.read_uint32();
		if ( self.type== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.itemid)):return False;
		if ( not ba.write_uint32(self.type)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 物品ID */"""
		print tabs+"[itemid]=%s"%(str(self.itemid))
		print tabs+"""/* （1穿上 0去掉） */"""
		print tabs+"[type]=%s"%(str(self.type))
		
		pass;
	




class cli_set_item_used_list_in :
	"""
参数列表:
	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public itemid_list =array();


	"""

	def read_from_buf(self, ba ):

		itemid_list_count= ba.read_uint32();
		if ( itemid_list_count==None ): return False;
		self.itemid_list=[];
		for i in range(itemid_list_count):
			self.itemid_list.append(ba.read_uint32());
			if ( self.itemid_list[i]== None): return False;


		return True;


	def write_to_buf(self,ba):
		itemid_list_count=len(self.itemid_list);
		ba.write_uint32(itemid_list_count);
		for i in range(itemid_list_count):
			if ( not ba.write_uint32(self.itemid_list[i])):return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"#"
		print tabs+"[itemid_list]=>{";
		for i in range(len(self.itemid_list)):
			print tabs+"    [%d]="%(i)+str(self.itemid_list[i]); 

		print tabs+'}'
		
		pass;
	




class cli_set_nick_in :
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
	




class cli_show_in :
	"""
参数列表:
	/* 表情ID */
	#类型:uint32
	public expression_id;

	/* 目标userid（0：场景 1：游戏中） */
	#类型:uint32
	public obj_userid;


	"""

	def read_from_buf(self, ba ):
		self.expression_id=ba.read_uint32();
		if ( self.expression_id== None): return False;
		self.obj_userid=ba.read_uint32();
		if ( self.obj_userid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.expression_id)):return False;
		if ( not ba.write_uint32(self.obj_userid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 表情ID */"""
		print tabs+"[expression_id]=%s"%(str(self.expression_id))
		print tabs+"""/* 目标userid（0：场景 1：游戏中） */"""
		print tabs+"[obj_userid]=%s"%(str(self.obj_userid))
		
		pass;
	




class cli_talk_in :
	"""
参数列表:
	/* 目标userid */
	#类型:uint32
	public obj_userid;

	/**/
	#变长数组,最大长度:9999, 类型:char
	public msg ;


	"""

	def read_from_buf(self, ba ):
		self.obj_userid=ba.read_uint32();
		if ( self.obj_userid== None): return False;

		msg_count= ba.read_uint32();
		if ( msg_count==None ): return False;
		self.msg=ba.read_buf(msg_count);
		if( self.msg==None ):return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.obj_userid)):return False;
		msg_count=len(self.msg);
		ba.write_uint32(msg_count);
		ba.write_buf(self.msg,msg_count);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 目标userid */"""
		print tabs+"[obj_userid]=%s"%(str(self.obj_userid))
		print tabs+"""/*  */"""
		print tabs+"[msg]=%s"%(str(self.msg))
		
		pass;
	




class cli_talk_npc_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public npc_id;


	"""

	def read_from_buf(self, ba ):
		self.npc_id=ba.read_uint32();
		if ( self.npc_id== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.npc_id)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[npc_id]=%s"%(str(self.npc_id))
		
		pass;
	




class cli_task_complete_node_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public islandid;

	/* 子任务id */
	#类型:uint32
	public task_nodeid;


	"""

	def read_from_buf(self, ba ):
		self.islandid=ba.read_uint32();
		if ( self.islandid== None): return False;
		self.task_nodeid=ba.read_uint32();
		if ( self.task_nodeid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.islandid)):return False;
		if ( not ba.write_uint32(self.task_nodeid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[islandid]=%s"%(str(self.islandid))
		print tabs+"""/* 子任务id */"""
		print tabs+"[task_nodeid]=%s"%(str(self.task_nodeid))
		
		pass;
	




class cli_task_complete_node_out :
	"""
参数列表:
	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public task_nodeid_list =array();


	"""

	def read_from_buf(self, ba ):

		task_nodeid_list_count= ba.read_uint32();
		if ( task_nodeid_list_count==None ): return False;
		self.task_nodeid_list=[];
		for i in range(task_nodeid_list_count):
			self.task_nodeid_list.append(ba.read_uint32());
			if ( self.task_nodeid_list[i]== None): return False;


		return True;


	def write_to_buf(self,ba):
		task_nodeid_list_count=len(self.task_nodeid_list);
		ba.write_uint32(task_nodeid_list_count);
		for i in range(task_nodeid_list_count):
			if ( not ba.write_uint32(self.task_nodeid_list[i])):return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"#"
		print tabs+"[task_nodeid_list]=>{";
		for i in range(len(self.task_nodeid_list)):
			print tabs+"    [%d]="%(i)+str(self.task_nodeid_list[i]); 

		print tabs+'}'
		
		pass;
	




class cli_task_del_node_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public islandid;

	/* 子任务id */
	#类型:uint32
	public task_nodeid;


	"""

	def read_from_buf(self, ba ):
		self.islandid=ba.read_uint32();
		if ( self.islandid== None): return False;
		self.task_nodeid=ba.read_uint32();
		if ( self.task_nodeid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.islandid)):return False;
		if ( not ba.write_uint32(self.task_nodeid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[islandid]=%s"%(str(self.islandid))
		print tabs+"""/* 子任务id */"""
		print tabs+"[task_nodeid]=%s"%(str(self.task_nodeid))
		
		pass;
	




class cli_user_move_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public start_x;

	/*  */
	#类型:uint32
	public start_y;

	/*  */
	#类型:uint32
	public mouse_x;

	/*  */
	#类型:uint32
	public mouse_y;

	/* 移动模式 */
	#类型:uint32
	public type;


	"""

	def read_from_buf(self, ba ):
		self.start_x=ba.read_uint32();
		if ( self.start_x== None): return False;
		self.start_y=ba.read_uint32();
		if ( self.start_y== None): return False;
		self.mouse_x=ba.read_uint32();
		if ( self.mouse_x== None): return False;
		self.mouse_y=ba.read_uint32();
		if ( self.mouse_y== None): return False;
		self.type=ba.read_uint32();
		if ( self.type== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.start_x)):return False;
		if ( not ba.write_uint32(self.start_y)):return False;
		if ( not ba.write_uint32(self.mouse_x)):return False;
		if ( not ba.write_uint32(self.mouse_y)):return False;
		if ( not ba.write_uint32(self.type)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[start_x]=%s"%(str(self.start_x))
		print tabs+"""/*  */"""
		print tabs+"[start_y]=%s"%(str(self.start_y))
		print tabs+"""/*  */"""
		print tabs+"[mouse_x]=%s"%(str(self.mouse_x))
		print tabs+"""/*  */"""
		print tabs+"[mouse_y]=%s"%(str(self.mouse_y))
		print tabs+"""/* 移动模式 */"""
		print tabs+"[type]=%s"%(str(self.type))
		
		pass;
	




class cli_walk_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public islandid;

	/*  */
	#类型:uint32
	public mapid;

	/*  */
	#类型:uint32
	public x;

	/*  */
	#类型:uint32
	public y;


	"""

	def read_from_buf(self, ba ):
		self.islandid=ba.read_uint32();
		if ( self.islandid== None): return False;
		self.mapid=ba.read_uint32();
		if ( self.mapid== None): return False;
		self.x=ba.read_uint32();
		if ( self.x== None): return False;
		self.y=ba.read_uint32();
		if ( self.y== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.islandid)):return False;
		if ( not ba.write_uint32(self.mapid)):return False;
		if ( not ba.write_uint32(self.x)):return False;
		if ( not ba.write_uint32(self.y)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[islandid]=%s"%(str(self.islandid))
		print tabs+"""/*  */"""
		print tabs+"[mapid]=%s"%(str(self.mapid))
		print tabs+"""/*  */"""
		print tabs+"[x]=%s"%(str(self.x))
		print tabs+"""/*  */"""
		print tabs+"[y]=%s"%(str(self.y))
		
		pass;
	




class cli_work_get_lottery_out :
	"""
参数列表:
	/* 今天获得的次数 */
	#类型:uint32
	public get_count;


	"""

	def read_from_buf(self, ba ):
		self.get_count=ba.read_uint32();
		if ( self.get_count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.get_count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 今天获得的次数 */"""
		print tabs+"[get_count]=%s"%(str(self.get_count))
		
		pass;
	




class login_activate_game_in :
	"""
参数列表:
	/* 项目id (填12) */
	#类型:uint32
	public which_game;

	/* 激活码：6-8个0-9、A-Z、a-z字符，没有用到的字节填0 */
	#定长数组,长度:10, 类型:char 
	public active_code ;

	/* 验证码ID */
	#定长数组,长度:16, 类型:char 
	public imgid ;

	/* 验证码 (空) */
	#定长数组,长度:6, 类型:char 
	public verif_code ;


	"""

	def read_from_buf(self, ba ):
		self.which_game=ba.read_uint32();
		if ( self.which_game== None): return False;
		self.active_code=ba.read_buf(10);
		if( self.active_code==None ): return False;
		self.imgid=ba.read_buf(16);
		if( self.imgid==None ): return False;
		self.verif_code=ba.read_buf(6);
		if( self.verif_code==None ): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.which_game)):return False;
		ba.write_buf(self.active_code,10);
		ba.write_buf(self.imgid,16);
		ba.write_buf(self.verif_code,6);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 项目id (填12) */"""
		print tabs+"[which_game]=%s"%(str(self.which_game))
		print tabs+"""/* 激活码：6-8个0-9、A-Z、a-z字符，没有用到的字节填0 */"""
		tmp_str=str(self.active_code);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[active_code]=%s"%(tmp_str)
		print tabs+"""/* 验证码ID */"""
		tmp_str=str(self.imgid);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[imgid]=%s"%(tmp_str)
		print tabs+"""/* 验证码 (空) */"""
		tmp_str=str(self.verif_code);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[verif_code]=%s"%(tmp_str)
		
		pass;
	




class login_activate_game_out :
	"""
参数列表:
	/* 0表示激活成功，不会发送蓝色部分的内容 1激活码错误次数过多，需要输入验证码后再激活 2验证码输入错误 */
	#类型:uint32
	public flag;

	/* 验证图片id(空） */
	#定长数组,长度:16, 类型:char 
	public img_id ;

	/*验证图片*/
	#变长数组,最大长度:1000, 类型:char
	public img ;


	"""

	def read_from_buf(self, ba ):
		self.flag=ba.read_uint32();
		if ( self.flag== None): return False;
		self.img_id=ba.read_buf(16);
		if( self.img_id==None ): return False;

		img_count= ba.read_uint32();
		if ( img_count==None ): return False;
		self.img=ba.read_buf(img_count);
		if( self.img==None ):return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.flag)):return False;
		ba.write_buf(self.img_id,16);
		img_count=len(self.img);
		ba.write_uint32(img_count);
		ba.write_buf(self.img,img_count);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 0表示激活成功，不会发送蓝色部分的内容 1激活码错误次数过多，需要输入验证码后再激活 2验证码输入错误 */"""
		print tabs+"[flag]=%s"%(str(self.flag))
		print tabs+"""/* 验证图片id(空） */"""
		tmp_str=str(self.img_id);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[img_id]=%s"%(tmp_str)
		print tabs+"""/* 验证图片 */"""
		print tabs+"[img]=%s"%(str(self.img))
		
		pass;
	




class login_check_game_activate_in :
	"""
参数列表:
	/* 项目id (填12) */
	#类型:uint32
	public which_game;


	"""

	def read_from_buf(self, ba ):
		self.which_game=ba.read_uint32();
		if ( self.which_game== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.which_game)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 项目id (填12) */"""
		print tabs+"[which_game]=%s"%(str(self.which_game))
		
		pass;
	




class login_check_game_activate_out :
	"""
参数列表:
	/* 0未激活1已激活 */
	#类型:uint32
	public flag;


	"""

	def read_from_buf(self, ba ):
		self.flag=ba.read_uint32();
		if ( self.flag== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.flag)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 0未激活1已激活 */"""
		print tabs+"[flag]=%s"%(str(self.flag))
		
		pass;
	




class login_get_ranged_svr_list_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public startid;

	/*  */
	#类型:uint32
	public endid;


	"""

	def read_from_buf(self, ba ):
		self.startid=ba.read_uint32();
		if ( self.startid== None): return False;
		self.endid=ba.read_uint32();
		if ( self.endid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.startid)):return False;
		if ( not ba.write_uint32(self.endid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[startid]=%s"%(str(self.startid))
		print tabs+"""/*  */"""
		print tabs+"[endid]=%s"%(str(self.endid))
		
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
	




class login_get_ranged_svr_list_out :
	"""
参数列表:
	/**/
	#变长数组,最大长度:1000, 类型:online_item_t
	public online_list =array();


	"""

	def read_from_buf(self, ba ):

		online_list_count= ba.read_uint32();
		if ( online_list_count==None ): return False;
		self.online_list=[];
		for i in range(online_list_count):
			self.online_list.append(online_item_t());
			if ( not self.online_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		online_list_count=len(self.online_list);
		ba.write_uint32(online_list_count);
		for i in range(online_list_count):
			if (  not  isinstance(self.online_list[i] ,online_item_t) ):return False; 
			if ( not self.online_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"#"
		print tabs+"[online_list]=>{";
		for i in range(len(self.online_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.online_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class login_get_recommeded_list_in :
	"""
参数列表:
	/* 用于跳转时无需密码 */
	#定长数组,长度:32, 类型:char 
	public session ;


	"""

	def read_from_buf(self, ba ):
		self.session=ba.read_buf(32);
		if( self.session==None ): return False;

		return True;


	def write_to_buf(self,ba):
		ba.write_buf(self.session,32);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 用于跳转时无需密码 */"""
		tmp_str=str(self.session);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[session]=%s"%(tmp_str)
		
		pass;
	




class login_get_recommeded_list_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public max_online_id;

	/*  */
	#类型:uint32
	public vip;

	/**/
	#变长数组,最大长度:1000, 类型:online_item_t
	public online_list =array();


	"""

	def read_from_buf(self, ba ):
		self.max_online_id=ba.read_uint32();
		if ( self.max_online_id== None): return False;
		self.vip=ba.read_uint32();
		if ( self.vip== None): return False;

		online_list_count= ba.read_uint32();
		if ( online_list_count==None ): return False;
		self.online_list=[];
		for i in range(online_list_count):
			self.online_list.append(online_item_t());
			if ( not self.online_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.max_online_id)):return False;
		if ( not ba.write_uint32(self.vip)):return False;
		online_list_count=len(self.online_list);
		ba.write_uint32(online_list_count);
		for i in range(online_list_count):
			if (  not  isinstance(self.online_list[i] ,online_item_t) ):return False; 
			if ( not self.online_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[max_online_id]=%s"%(str(self.max_online_id))
		print tabs+"""/*  */"""
		print tabs+"[vip]=%s"%(str(self.vip))
		print tabs+"#"
		print tabs+"[online_list]=>{";
		for i in range(len(self.online_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.online_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class login_login_in :
	"""
参数列表:
	/* 密码的两次md5值 */
	#定长数组,长度:32, 类型:char 
	public passwd_md5_two ;

	/* 渠道id(填0) */
	#类型:uint32
	public channelid;

	/* 项目id (填12) */
	#类型:uint32
	public which_game;

	/* ip 地址 */
	#类型:uint32
	public user_ip;

	/* 验证图片id(空） */
	#定长数组,长度:16, 类型:char 
	public img_id ;

	/* 验证码 (空) */
	#定长数组,长度:6, 类型:char 
	public verif_code ;


	"""

	def read_from_buf(self, ba ):
		self.passwd_md5_two=ba.read_buf(32);
		if( self.passwd_md5_two==None ): return False;
		self.channelid=ba.read_uint32();
		if ( self.channelid== None): return False;
		self.which_game=ba.read_uint32();
		if ( self.which_game== None): return False;
		self.user_ip=ba.read_uint32();
		if ( self.user_ip== None): return False;
		self.img_id=ba.read_buf(16);
		if( self.img_id==None ): return False;
		self.verif_code=ba.read_buf(6);
		if( self.verif_code==None ): return False;

		return True;


	def write_to_buf(self,ba):
		ba.write_buf(self.passwd_md5_two,32);
		if ( not ba.write_uint32(self.channelid)):return False;
		if ( not ba.write_uint32(self.which_game)):return False;
		if ( not ba.write_uint32(self.user_ip)):return False;
		ba.write_buf(self.img_id,16);
		ba.write_buf(self.verif_code,6);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 密码的两次md5值 */"""
		tmp_str=str(self.passwd_md5_two);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[passwd_md5_two]=%s"%(tmp_str)
		print tabs+"""/* 渠道id(填0) */"""
		print tabs+"[channelid]=%s"%(str(self.channelid))
		print tabs+"""/* 项目id (填12) */"""
		print tabs+"[which_game]=%s"%(str(self.which_game))
		print tabs+"""/* ip 地址 */"""
		print tabs+"[user_ip]=%s"%(str(self.user_ip))
		print tabs+"""/* 验证图片id(空） */"""
		tmp_str=str(self.img_id);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[img_id]=%s"%(tmp_str)
		print tabs+"""/* 验证码 (空) */"""
		tmp_str=str(self.verif_code);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[verif_code]=%s"%(tmp_str)
		
		pass;
	




class login_login_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public verif_flag;

	/* 用于跳转时无需密码 */
	#定长数组,长度:16, 类型:char 
	public session ;

	/*  */
	#类型:uint32
	public create_role_flag;


	"""

	def read_from_buf(self, ba ):
		self.verif_flag=ba.read_uint32();
		if ( self.verif_flag== None): return False;
		self.session=ba.read_buf(16);
		if( self.session==None ): return False;
		self.create_role_flag=ba.read_uint32();
		if ( self.create_role_flag== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.verif_flag)):return False;
		ba.write_buf(self.session,16);
		if ( not ba.write_uint32(self.create_role_flag)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[verif_flag]=%s"%(str(self.verif_flag))
		print tabs+"""/* 用于跳转时无需密码 */"""
		tmp_str=str(self.session);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[session]=%s"%(tmp_str)
		print tabs+"""/*  */"""
		print tabs+"[create_role_flag]=%s"%(str(self.create_role_flag))
		
		pass;
	




class login_refresh_img_in :
	"""
参数列表:
	/* 0正常登录； 1使用激活码时（可以不发送，兼容旧版本即正常登录） */
	#类型:uint32
	public flag;


	"""

	def read_from_buf(self, ba ):
		self.flag=ba.read_uint32();
		if ( self.flag== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.flag)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 0正常登录； 1使用激活码时（可以不发送，兼容旧版本即正常登录） */"""
		print tabs+"[flag]=%s"%(str(self.flag))
		
		pass;
	




class login_refresh_img_out :
	"""
参数列表:
	/* 0不需要输入验证码，1需要输入验证（为1时才有验证码)） */
	#类型:uint32
	public flag;

	/* 图片ID。发送验证码时，需要把这个ID带上 */
	#定长数组,长度:16, 类型:char 
	public imgid ;

	/*验证图片*/
	#变长数组,最大长度:1000, 类型:char
	public img ;


	"""

	def read_from_buf(self, ba ):
		self.flag=ba.read_uint32();
		if ( self.flag== None): return False;
		self.imgid=ba.read_buf(16);
		if( self.imgid==None ): return False;

		img_count= ba.read_uint32();
		if ( img_count==None ): return False;
		self.img=ba.read_buf(img_count);
		if( self.img==None ):return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.flag)):return False;
		ba.write_buf(self.imgid,16);
		img_count=len(self.img);
		ba.write_uint32(img_count);
		ba.write_buf(self.img,img_count);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 0不需要输入验证码，1需要输入验证（为1时才有验证码)） */"""
		print tabs+"[flag]=%s"%(str(self.flag))
		print tabs+"""/* 图片ID。发送验证码时，需要把这个ID带上 */"""
		tmp_str=str(self.imgid);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[imgid]=%s"%(tmp_str)
		print tabs+"""/* 验证图片 */"""
		print tabs+"[img]=%s"%(str(self.img))
		
		pass;
	




class noti_cli_leave_out :
	"""
参数列表:
	/* 离线原因 （1午夜休息） */
	#类型:uint32
	public reason;


	"""

	def read_from_buf(self, ba ):
		self.reason=ba.read_uint32();
		if ( self.reason== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.reason)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 离线原因 （1午夜休息） */"""
		print tabs+"[reason]=%s"%(str(self.reason))
		
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
	/* 物品id */
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
		print tabs+"""/* 物品id */"""
		print tabs+"[itemid]=%s"%(str(self.itemid))
		print tabs+"""/*  */"""
		print tabs+"[use_count]=%s"%(str(self.use_count))
		print tabs+"""/*  */"""
		print tabs+"[count]=%s"%(str(self.count))
		
		pass;
	




class task_t :
	"""
参数列表:
	/* 主任务id */
	#类型:uint32
	public taskid;

	/* 子任务id */
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
		print tabs+"""/* 主任务id */"""
		print tabs+"[taskid]=%s"%(str(self.taskid))
		print tabs+"""/* 子任务id */"""
		print tabs+"[task_nodeid]=%s"%(str(self.task_nodeid))
		
		pass;
	




create_time='2012年12月27日 星期4 12:04:26'
cmd_map={ }
cmd_map[1]=(1,'cli_proto_header',cli_proto_header_in,None,0x2e14bf71)
cmd_map[101]=(101,'login_refresh_img',login_refresh_img_in,login_refresh_img_out,0xed66049a)
cmd_map[102]=(102,'login_check_game_activate',login_check_game_activate_in,login_check_game_activate_out,0x68dbed7e)
cmd_map[103]=(103,'login_login',login_login_in,login_login_out,0xc0774b87)
cmd_map[104]=(104,'login_activate_game',login_activate_game_in,login_activate_game_out,0xec256cd8)
cmd_map[105]=(105,'login_get_recommeded_list',login_get_recommeded_list_in,login_get_recommeded_list_out,0x842984ca)
cmd_map[106]=(106,'login_get_ranged_svr_list',login_get_ranged_svr_list_in,login_get_ranged_svr_list_out,0xb38d89ee)
cmd_map[1001]=(1001,'cli_login',cli_login_in,cli_login_out,0x0a410403)
cmd_map[1002]=(1002,'cli_reg',cli_reg_in,cli_reg_out,0xc558bcb8)
cmd_map[1003]=(1003,'noti_cli_leave',None,noti_cli_leave_out,0x8154f01d)
cmd_map[1004]=(1004,'cli_noti_island_complete',None,cli_noti_island_complete_out,0x5117b356)
cmd_map[1011]=(1011,'cli_task_complete_node',cli_task_complete_node_in,cli_task_complete_node_out,0xb9a905ba)
cmd_map[1012]=(1012,'cli_get_item_list',cli_get_item_list_in,cli_get_item_list_out,0xedc9abd8)
cmd_map[1013]=(1013,'cli_get_spec_item',cli_get_spec_item_in,cli_get_spec_item_out,0xd8099850)
cmd_map[1014]=(1014,'cli_walk',cli_walk_in,None,0x23916811)
cmd_map[1015]=(1015,'cli_get_user_island_task_info',cli_get_user_island_task_info_in,cli_get_user_island_task_info_out,0xffd0451d)
cmd_map[1016]=(1016,'cli_find_map_add',cli_find_map_add_in,None,0x200d7500)
cmd_map[1017]=(1017,'cli_set_nick',cli_set_nick_in,None,0x52ac0fda)
cmd_map[1018]=(1018,'cli_set_item_used_list',cli_set_item_used_list_in,None,0xccc87448)
cmd_map[1019]=(1019,'cli_task_del_node',cli_task_del_node_in,None,0xa80dbeed)
cmd_map[1020]=(1020,'cli_get_user_island_find_map_info',cli_get_user_island_find_map_info_in,cli_get_user_island_find_map_info_out,0xa91abb1c)
cmd_map[1021]=(1021,'cli_get_card_list_by_islandid',cli_get_card_list_by_islandid_in,cli_get_card_list_by_islandid_out,0x3d8e05fa)
cmd_map[1022]=(1022,'cli_talk_npc',cli_talk_npc_in,None,0x3c2927f4)
cmd_map[1023]=(1023,'cli_get_user_info',cli_get_user_info_in,cli_get_user_info_out,0x7e293ceb)
cmd_map[1024]=(1024,'cli_get_user_game_stat',cli_get_user_game_stat_in,cli_get_user_game_stat_out,0xc11a3f99)
cmd_map[1025]=(1025,'cli_noti_user_online_time',None,cli_noti_user_online_time_out,0x94941e35)
cmd_map[1026]=(1026,'cli_noti_first_enter_island',None,cli_noti_first_enter_island_out,0xc775a40e)
cmd_map[1028]=(1028,'cli_set_effect_used',cli_set_effect_used_in,None,0xf86b3c0a)
cmd_map[1029]=(1029,'cli_noti_effect_used',None,cli_noti_effect_used_out,0xffd34345)
cmd_map[1030]=(1030,'cli_noti_get_item_list',None,cli_noti_get_item_list_out,0xc0680138)
cmd_map[1031]=(1031,'cli_del_item',cli_del_item_in,None,0x53797358)
cmd_map[1032]=(1032,'cli_get_island_time',None,cli_get_island_time_out,0xca1743b9)
cmd_map[1033]=(1033,'cli_noti_leave_island',None,cli_noti_leave_island_out,0x9b5762ff)
cmd_map[1034]=(1034,'cli_get_spec_item_list',cli_get_spec_item_list_in,cli_get_spec_item_list_out,0xfdf51e9a)
cmd_map[1035]=(1035,'cli_set_color',cli_set_color_in,None,0x2dfde07c)
cmd_map[1036]=(1036,'cli_noti_set_color',None,cli_noti_set_color_out,0xfcdf2bf3)
cmd_map[1100]=(1100,'cli_noti_one_user_info',None,cli_noti_one_user_info_out,0x4854c772)
cmd_map[1101]=(1101,'cli_get_all_user_info_from_cur_map',None,cli_get_all_user_info_from_cur_map_out,0x5b4b845a)
cmd_map[1102]=(1102,'cli_user_move',cli_user_move_in,None,0x20304753)
cmd_map[1103]=(1103,'cli_noti_user_move',None,cli_noti_user_move_out,0x55ccb12f)
cmd_map[1104]=(1104,'cli_noti_user_left_map',None,cli_noti_user_left_map_out,0x9e97ab66)
cmd_map[1105]=(1105,'cli_talk',cli_talk_in,None,0x0e444aef)
cmd_map[1106]=(1106,'cli_noti_user_use_clothes_list',None,cli_noti_user_use_clothes_list_out,0xaf464111)
cmd_map[1107]=(1107,'cli_noti_talk',None,cli_noti_talk_out,0x52b6ada9)
cmd_map[1108]=(1108,'cli_show',cli_show_in,None,0xe8810497)
cmd_map[1109]=(1109,'cli_noti_show',None,cli_noti_show_out,0x33e74a5e)
cmd_map[1110]=(1110,'cli_set_busy_state',cli_set_busy_state_in,None,0x43f7232d)
cmd_map[1111]=(1111,'cli_noti_user_level_up',None,cli_noti_user_level_up_out,0x01d4492a)
cmd_map[1201]=(1201,'cli_game_request',cli_game_request_in,cli_game_request_out,0xba487b08)
cmd_map[1202]=(1202,'cli_noti_game_request',None,cli_noti_game_request_out,0xddb60a52)
cmd_map[1203]=(1203,'cli_game_play_with_other',cli_game_play_with_other_in,cli_game_play_with_other_out,0x5fb58c09)
cmd_map[1204]=(1204,'cli_noti_game_is_start',None,cli_noti_game_is_start_out,0xaf7927e5)
cmd_map[1205]=(1205,'cli_game_can_start',None,None,0x511673c0)
cmd_map[1206]=(1206,'cli_noti_game_start',None,cli_noti_game_start_out,0x81759933)
cmd_map[1207]=(1207,'cli_game_opt',cli_game_opt_in,None,0xe2b1aad9)
cmd_map[1208]=(1208,'cli_noti_game_opt',None,cli_noti_game_opt_out,0x0d347d17)
cmd_map[1209]=(1209,'cli_cur_game_end',cli_cur_game_end_in,None,0xf944db64)
cmd_map[1210]=(1210,'cli_noti_game_end',None,cli_noti_game_end_out,0xf86ab508)
cmd_map[1211]=(1211,'cli_game_user_left_game',None,None,0x5b5a3ffd)
cmd_map[1212]=(1212,'cli_noti_game_user_left_game',None,cli_noti_game_user_left_game_out,0xbb260459)
cmd_map[1213]=(1213,'cli_buy_item',cli_buy_item_in,cli_buy_item_out,0x98174eba)
cmd_map[1214]=(1214,'cli_noti_game_user',None,cli_noti_game_user_out,0x0ba31f2c)
cmd_map[1215]=(1215,'cli_get_game_user',None,cli_get_game_user_out,0xcdb47db1)
cmd_map[1216]=(1216,'cli_buy_item_use_gamept',cli_buy_item_use_gamept_in,cli_buy_item_use_gamept_out,0x937ad5bd)
cmd_map[1217]=(1217,'cli_noti_game_seat',None,cli_noti_game_seat_out,0xbac528d8)
cmd_map[1218]=(1218,'cli_get_valid_gamept',None,cli_get_valid_gamept_out,0x459682e3)
cmd_map[1301]=(1301,'cli_post_msg',cli_post_msg_in,None,0xedc17e31)
cmd_map[1401]=(1401,'cli_work_get_lottery',None,cli_work_get_lottery_out,0x491f80bf)
cmd_map[1402]=(1402,'cli_draw_lottery',None,cli_draw_lottery_out,0x4983d8c5)
cmd_map[1403]=(1403,'cli_get_lottery_count',None,cli_get_lottery_count_out,0x51b8de2e)
cmd_map[1404]=(1404,'cli_hang_bell_get_item',cli_hang_bell_get_item_in,cli_hang_bell_get_item_out,0xd39e6602)
cmd_map[1405]=(1405,'cli_click_chris_tree',None,None,0xcf1c12cc)
cmd_map[1406]=(1406,'cli_click_guide',None,None,0x9f0e3ad1)
cmd_map[1407]=(1407,'cli_click_stat',cli_click_stat_in,None,0x6997c174)
cmd_map[1408]=(1408,'cli_click_game_notice',None,None,0x73830881)
cmd_map[1409]=(1409,'cli_noti_get_invitation',None,cli_noti_get_invitation_out,0xb33f420b)
cmd_map[1410]=(1410,'cli_noti_svr_time',None,cli_noti_svr_time_out,0xd317b23a)
cmd_map[1411]=(1411,'cli_click_pet_egg',None,None,0x3de04bd1)
cmd_map[1412]=(1412,'cli_get_login_chest',None,cli_get_login_chest_out,0x50943a28)
cmd_map[1413]=(1413,'cli_get_login_reward',cli_get_login_reward_in,cli_get_login_reward_out,0x10d92a72)
	
def init_cmd_map():
	pass

try:
	import pop_online_proto_base
	base_type=pop_online_proto_base.Cpop_online_proto_base
except:
	base_type=object
	pass


class Cpop_online_proto (base_type):
	def __init__(self,ip,port):
		pop_online_proto_base.Cpop_online_proto_base.__init__(self,ip,port)

	def cli_proto_header(self ,proto_length,cmdid,timestamp,seq,result,useSever ):
		"""
报文头部<font color=red >小端字节序</font>:
	/*  */
	#类型:uint16
	public useSever;


	"""
		pri_in=cli_proto_header_in();

		pri_in.proto_length=proto_length
		pri_in.cmdid=cmdid
		pri_in.timestamp=timestamp
		pri_in.seq=seq
		pri_in.result=result
		pri_in.useSever=useSever
		self.sendmsg(1, pri_in );
		pass

	def login_refresh_img(self ,flag ):
		"""
重新获取验证码:
	/* 0正常登录； 1使用激活码时（可以不发送，兼容旧版本即正常登录） */
	#类型:uint32
	public flag;


	"""
		pri_in=login_refresh_img_in();

		pri_in.flag=flag
		self.sendmsg(101, pri_in );
		pass

	def login_check_game_activate(self ,which_game ):
		"""
检查游戏是否激活:
	/* 项目id (填12) */
	#类型:uint32
	public which_game;


	"""
		pri_in=login_check_game_activate_in();

		pri_in.which_game=which_game
		self.sendmsg(102, pri_in );
		pass

	def login_login(self ,passwd_md5_two,channelid,which_game,user_ip,img_id,verif_code ):
		"""
登录:
	/* 验证码 (空) */
	#定长数组,长度:6, 类型:char 
	public verif_code ;


	"""
		pri_in=login_login_in();

		pri_in.passwd_md5_two=passwd_md5_two
		pri_in.channelid=channelid
		pri_in.which_game=which_game
		pri_in.user_ip=user_ip
		pri_in.img_id=img_id
		pri_in.verif_code=verif_code
		self.sendmsg(103, pri_in );
		pass

	def login_activate_game(self ,which_game,active_code,imgid,verif_code ):
		"""
激活游戏:
	/* 验证码 (空) */
	#定长数组,长度:6, 类型:char 
	public verif_code ;


	"""
		pri_in=login_activate_game_in();

		pri_in.which_game=which_game
		pri_in.active_code=active_code
		pri_in.imgid=imgid
		pri_in.verif_code=verif_code
		self.sendmsg(104, pri_in );
		pass

	def login_get_recommeded_list(self ,session ):
		"""
得到推荐服务器列表:
	/* 用于跳转时无需密码 */
	#定长数组,长度:32, 类型:char 
	public session ;


	"""
		pri_in=login_get_recommeded_list_in();

		pri_in.session=session
		self.sendmsg(105, pri_in );
		pass

	def login_get_ranged_svr_list(self ,startid,endid ):
		"""
得到服务器范围列表:
	/*  */
	#类型:uint32
	public endid;


	"""
		pri_in=login_get_ranged_svr_list_in();

		pri_in.startid=startid
		pri_in.endid=endid
		self.sendmsg(106, pri_in );
		pass

	def cli_login(self ,server_id,login_userid,session ):
		"""
登入:
	/* 用于跳转时无需密码 */
	#定长数组,长度:32, 类型:char 
	public session ;


	"""
		pri_in=cli_login_in();

		pri_in.server_id=server_id
		pri_in.login_userid=login_userid
		pri_in.session=session
		self.sendmsg(1001, pri_in );
		pass

	def cli_reg(self ,channelid,nick,age,color,item_list ):
		"""
用户注册:
	/**/
	#变长数组,最大长度:9999, 类型:item_t
	public item_list =array();


	"""
		pri_in=cli_reg_in();

		pri_in.channelid=channelid
		pri_in.nick=nick
		pri_in.age=age
		pri_in.color=color
		pri_in.item_list=item_list
		self.sendmsg(1002, pri_in );
		pass

	def noti_cli_leave(self  ):
		"""
通知用户离线:

	"""
		pri_in=None;

		self.sendmsg(1003, pri_in );
		pass

	def cli_noti_island_complete(self  ):
		"""
通知用户某人岛屿完成:

	"""
		pri_in=None;

		self.sendmsg(1004, pri_in );
		pass

	def cli_task_complete_node(self ,islandid,task_nodeid ):
		"""
完成任务节点:
	/* 子任务id */
	#类型:uint32
	public task_nodeid;


	"""
		pri_in=cli_task_complete_node_in();

		pri_in.islandid=islandid
		pri_in.task_nodeid=task_nodeid
		self.sendmsg(1011, pri_in );
		pass

	def cli_get_item_list(self ,startid,endid ):
		"""
得到物品列表:
	/*  */
	#类型:uint32
	public endid;


	"""
		pri_in=cli_get_item_list_in();

		pri_in.startid=startid
		pri_in.endid=endid
		self.sendmsg(1012, pri_in );
		pass

	def cli_get_spec_item(self ,itemid ):
		"""
获取特殊物品数量:
	/* 物品ID（3战斗积分4在线时长5打工次数8圣诞礼券） */
	#类型:uint32
	public itemid;


	"""
		pri_in=cli_get_spec_item_in();

		pri_in.itemid=itemid
		self.sendmsg(1013, pri_in );
		pass

	def cli_walk(self ,islandid,mapid,x,y ):
		"""
进入场景:
	/*  */
	#类型:uint32
	public y;


	"""
		pri_in=cli_walk_in();

		pri_in.islandid=islandid
		pri_in.mapid=mapid
		pri_in.x=x
		pri_in.y=y
		self.sendmsg(1014, pri_in );
		pass

	def cli_get_user_island_task_info(self ,islandid ):
		"""
得到用户在该岛上的任务信息:
	/*  */
	#类型:uint32
	public islandid;


	"""
		pri_in=cli_get_user_island_task_info_in();

		pri_in.islandid=islandid
		self.sendmsg(1015, pri_in );
		pass

	def cli_find_map_add(self ,islandid,mapid ):
		"""
增加找到的地图:
	/*  */
	#类型:uint32
	public mapid;


	"""
		pri_in=cli_find_map_add_in();

		pri_in.islandid=islandid
		pri_in.mapid=mapid
		self.sendmsg(1016, pri_in );
		pass

	def cli_set_nick(self ,nick ):
		"""
设置昵称:
	/*  */
	#定长数组,长度:16, 类型:char 
	public nick ;


	"""
		pri_in=cli_set_nick_in();

		pri_in.nick=nick
		self.sendmsg(1017, pri_in );
		pass

	def cli_set_item_used_list(self ,itemid_list ):
		"""
设置使用物品列表:
	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public itemid_list =array();


	"""
		pri_in=cli_set_item_used_list_in();

		pri_in.itemid_list=itemid_list
		self.sendmsg(1018, pri_in );
		pass

	def cli_task_del_node(self ,islandid,task_nodeid ):
		"""
删除任务:
	/* 子任务id */
	#类型:uint32
	public task_nodeid;


	"""
		pri_in=cli_task_del_node_in();

		pri_in.islandid=islandid
		pri_in.task_nodeid=task_nodeid
		self.sendmsg(1019, pri_in );
		pass

	def cli_get_user_island_find_map_info(self ,islandid ):
		"""
得到岛上的找到地图的信息:
	/*  */
	#类型:uint32
	public islandid;


	"""
		pri_in=cli_get_user_island_find_map_info_in();

		pri_in.islandid=islandid
		self.sendmsg(1020, pri_in );
		pass

	def cli_get_card_list_by_islandid(self ,islandid ):
		"""
:
	/*  */
	#类型:uint32
	public islandid;


	"""
		pri_in=cli_get_card_list_by_islandid_in();

		pri_in.islandid=islandid
		self.sendmsg(1021, pri_in );
		pass

	def cli_talk_npc(self ,npc_id ):
		"""
和npc聊天:
	/*  */
	#类型:uint32
	public npc_id;


	"""
		pri_in=cli_talk_npc_in();

		pri_in.npc_id=npc_id
		self.sendmsg(1022, pri_in );
		pass

	def cli_get_user_info(self ,obj_userid ):
		"""
得到用户信息:
	/* 目标userid */
	#类型:uint32
	public obj_userid;


	"""
		pri_in=cli_get_user_info_in();

		pri_in.obj_userid=obj_userid
		self.sendmsg(1023, pri_in );
		pass

	def cli_get_user_game_stat(self ,obj_userid ):
		"""
获取用户的小游戏统计:
	/* 目标userid */
	#类型:uint32
	public obj_userid;


	"""
		pri_in=cli_get_user_game_stat_in();

		pri_in.obj_userid=obj_userid
		self.sendmsg(1024, pri_in );
		pass

	def cli_noti_user_online_time(self  ):
		"""
通知客户端用户在线时间（分钟）:

	"""
		pri_in=None;

		self.sendmsg(1025, pri_in );
		pass

	def cli_noti_first_enter_island(self  ):
		"""
通知用户第一次进某岛:

	"""
		pri_in=None;

		self.sendmsg(1026, pri_in );
		pass

	def cli_set_effect_used(self ,itemid,type ):
		"""
使用特效卡片:
	/* （1穿上 0去掉） */
	#类型:uint32
	public type;


	"""
		pri_in=cli_set_effect_used_in();

		pri_in.itemid=itemid
		pri_in.type=type
		self.sendmsg(1028, pri_in );
		pass

	def cli_noti_effect_used(self  ):
		"""
通知有人使用特效卡片:

	"""
		pri_in=None;

		self.sendmsg(1029, pri_in );
		pass

	def cli_noti_get_item_list(self  ):
		"""
通知客户端得到一些物品:

	"""
		pri_in=None;

		self.sendmsg(1030, pri_in );
		pass

	def cli_del_item(self ,itemlist ):
		"""
删除一些物品:
	/*要删除的物品列表*/
	#变长数组,最大长度:999, 类型:item_t
	public itemlist =array();


	"""
		pri_in=cli_del_item_in();

		pri_in.itemlist=itemlist
		self.sendmsg(1031, pri_in );
		pass

	def cli_get_island_time(self  ):
		"""
获取限时岛屿的开放时间:

	"""
		pri_in=None;

		self.sendmsg(1032, pri_in );
		pass

	def cli_noti_leave_island(self  ):
		"""
通知用户离开限时岛屿:

	"""
		pri_in=None;

		self.sendmsg(1033, pri_in );
		pass

	def cli_get_spec_item_list(self ,startid,endid ):
		"""
拉取范围的特殊物品:
	/*  */
	#类型:uint32
	public endid;


	"""
		pri_in=cli_get_spec_item_list_in();

		pri_in.startid=startid
		pri_in.endid=endid
		self.sendmsg(1034, pri_in );
		pass

	def cli_set_color(self ,color ):
		"""
换肤:
	/* 肤色 */
	#类型:uint32
	public color;


	"""
		pri_in=cli_set_color_in();

		pri_in.color=color
		self.sendmsg(1035, pri_in );
		pass

	def cli_noti_set_color(self  ):
		"""
:

	"""
		pri_in=None;

		self.sendmsg(1036, pri_in );
		pass

	def cli_noti_one_user_info(self  ):
		"""
通知单个用户在线信息(别人进来了):

	"""
		pri_in=None;

		self.sendmsg(1100, pri_in );
		pass

	def cli_get_all_user_info_from_cur_map(self  ):
		"""
得到当前地图的所有人信息:

	"""
		pri_in=None;

		self.sendmsg(1101, pri_in );
		pass

	def cli_user_move(self ,start_x,start_y,mouse_x,mouse_y,type ):
		"""
用户移动 多人模式下:
	/* 移动模式 */
	#类型:uint32
	public type;


	"""
		pri_in=cli_user_move_in();

		pri_in.start_x=start_x
		pri_in.start_y=start_y
		pri_in.mouse_x=mouse_x
		pri_in.mouse_y=mouse_y
		pri_in.type=type
		self.sendmsg(1102, pri_in );
		pass

	def cli_noti_user_move(self  ):
		"""
通知用户移动:

	"""
		pri_in=None;

		self.sendmsg(1103, pri_in );
		pass

	def cli_noti_user_left_map(self  ):
		"""
通知用户离开地图:

	"""
		pri_in=None;

		self.sendmsg(1104, pri_in );
		pass

	def cli_talk(self ,obj_userid,msg ):
		"""
聊天:
	/**/
	#变长数组,最大长度:9999, 类型:char
	public msg ;


	"""
		pri_in=cli_talk_in();

		pri_in.obj_userid=obj_userid
		pri_in.msg=msg
		self.sendmsg(1105, pri_in );
		pass

	def cli_noti_user_use_clothes_list(self  ):
		"""
通知用户更换装扮:

	"""
		pri_in=None;

		self.sendmsg(1106, pri_in );
		pass

	def cli_noti_talk(self  ):
		"""
通知聊天:

	"""
		pri_in=None;

		self.sendmsg(1107, pri_in );
		pass

	def cli_show(self ,expression_id,obj_userid ):
		"""
发表情:
	/* 目标userid（0：场景 1：游戏中） */
	#类型:uint32
	public obj_userid;


	"""
		pri_in=cli_show_in();

		pri_in.expression_id=expression_id
		pri_in.obj_userid=obj_userid
		self.sendmsg(1108, pri_in );
		pass

	def cli_noti_show(self  ):
		"""
通知某用户做表情:

	"""
		pri_in=None;

		self.sendmsg(1109, pri_in );
		pass

	def cli_set_busy_state(self ,state ):
		"""
设置忙状态:
	/* 忙状态 */
	#类型:uint32
	public state;


	"""
		pri_in=cli_set_busy_state_in();

		pri_in.state=state
		self.sendmsg(1110, pri_in );
		pass

	def cli_noti_user_level_up(self  ):
		"""
通知有人升级:

	"""
		pri_in=None;

		self.sendmsg(1111, pri_in );
		pass

	def cli_game_request(self ,gameid,obj_userid ):
		"""
请求和某人玩游戏:
	/* 目标userid */
	#类型:uint32
	public obj_userid;


	"""
		pri_in=cli_game_request_in();

		pri_in.gameid=gameid
		pri_in.obj_userid=obj_userid
		self.sendmsg(1201, pri_in );
		pass

	def cli_noti_game_request(self  ):
		"""
通知游戏请求:

	"""
		pri_in=None;

		self.sendmsg(1202, pri_in );
		pass

	def cli_game_play_with_other(self ,gameid,request_session,obj_userid,is_start,why_not_start_flag ):
		"""
确认请求:
	/* 为什么不玩, 1:自己关，2：在换装，3：购物 */
	#类型:uint32
	public why_not_start_flag;


	"""
		pri_in=cli_game_play_with_other_in();

		pri_in.gameid=gameid
		pri_in.request_session=request_session
		pri_in.obj_userid=obj_userid
		pri_in.is_start=is_start
		pri_in.why_not_start_flag=why_not_start_flag
		self.sendmsg(1203, pri_in );
		pass

	def cli_noti_game_is_start(self  ):
		"""
通知请求游戏是否开始,如果is_start=1,（发给双方)，is_start=0(发给请求方):

	"""
		pri_in=None;

		self.sendmsg(1204, pri_in );
		pass

	def cli_game_can_start(self  ):
		"""
客户端确认游戏可以开始了( 1204 返回is_start=1 时，在加载游戏完成时发):

	"""
		pri_in=None;

		self.sendmsg(1205, pri_in );
		pass

	def cli_noti_game_start(self  ):
		"""
通知游戏开始:

	"""
		pri_in=None;

		self.sendmsg(1206, pri_in );
		pass

	def cli_game_opt(self ,gamemsg ):
		"""
用户游戏操作:
	/*游戏操作信息*/
	#变长数组,最大长度:9999, 类型:char
	public gamemsg ;


	"""
		pri_in=cli_game_opt_in();

		pri_in.gamemsg=gamemsg
		self.sendmsg(1207, pri_in );
		pass

	def cli_noti_game_opt(self  ):
		"""
通知用户游戏操作:

	"""
		pri_in=None;

		self.sendmsg(1208, pri_in );
		pass

	def cli_cur_game_end(self ,win_flag ):
		"""
当前一局游戏结束:
	/* 自己是不是羸了 :0:不是,1:是,2:平局 */
	#类型:uint32
	public win_flag;


	"""
		pri_in=cli_cur_game_end_in();

		pri_in.win_flag=win_flag
		self.sendmsg(1209, pri_in );
		pass

	def cli_noti_game_end(self  ):
		"""
通知游戏结束:

	"""
		pri_in=None;

		self.sendmsg(1210, pri_in );
		pass

	def cli_game_user_left_game(self  ):
		"""
离开游戏:

	"""
		pri_in=None;

		self.sendmsg(1211, pri_in );
		pass

	def cli_noti_game_user_left_game(self  ):
		"""
用户离开游戏通知:

	"""
		pri_in=None;

		self.sendmsg(1212, pri_in );
		pass

	def cli_buy_item(self ,itemid ):
		"""
购买物品:
	/* 物品id */
	#类型:uint32
	public itemid;


	"""
		pri_in=cli_buy_item_in();

		pri_in.itemid=itemid
		self.sendmsg(1213, pri_in );
		pass

	def cli_noti_game_user(self  ):
		"""
通知场景中的所有玩家有人进入游戏:

	"""
		pri_in=None;

		self.sendmsg(1214, pri_in );
		pass

	def cli_get_game_user(self  ):
		"""
获取当前场景正在游戏的玩家:

	"""
		pri_in=None;

		self.sendmsg(1215, pri_in );
		pass

	def cli_buy_item_use_gamept(self ,itemid ):
		"""
使用游戏积分购买物品:
	/* 物品ID */
	#类型:uint32
	public itemid;


	"""
		pri_in=cli_buy_item_use_gamept_in();

		pri_in.itemid=itemid
		self.sendmsg(1216, pri_in );
		pass

	def cli_noti_game_seat(self  ):
		"""
获取游戏座位信息:

	"""
		pri_in=None;

		self.sendmsg(1217, pri_in );
		pass

	def cli_get_valid_gamept(self  ):
		"""
拉取有效的小游戏积分:

	"""
		pri_in=None;

		self.sendmsg(1218, pri_in );
		pass

	def cli_post_msg(self ,type,title,msg ):
		"""
投稿投诉:
	/*投稿内容*/
	#变长数组,最大长度:4096, 类型:char
	public msg ;


	"""
		pri_in=cli_post_msg_in();

		pri_in.type=type
		pri_in.title=title
		pri_in.msg=msg
		self.sendmsg(1301, pri_in );
		pass

	def cli_work_get_lottery(self  ):
		"""
打工获得抽奖机会:

	"""
		pri_in=None;

		self.sendmsg(1401, pri_in );
		pass

	def cli_draw_lottery(self  ):
		"""
抽奖:

	"""
		pri_in=None;

		self.sendmsg(1402, pri_in );
		pass

	def cli_get_lottery_count(self  ):
		"""
获取剩余抽奖机会:

	"""
		pri_in=None;

		self.sendmsg(1403, pri_in );
		pass

	def cli_hang_bell_get_item(self ,bell_type,zone_type ):
		"""
挂铃铛赢奖励:
	/* 投中区域(0蓝1绿2红3没投中) */
	#类型:uint32
	public zone_type;


	"""
		pri_in=cli_hang_bell_get_item_in();

		pri_in.bell_type=bell_type
		pri_in.zone_type=zone_type
		self.sendmsg(1404, pri_in );
		pass

	def cli_click_chris_tree(self  ):
		"""
点击圣诞树统计:

	"""
		pri_in=None;

		self.sendmsg(1405, pri_in );
		pass

	def cli_click_guide(self  ):
		"""
点击新手指引:

	"""
		pri_in=None;

		self.sendmsg(1406, pri_in );
		pass

	def cli_click_stat(self ,type ):
		"""
点击统计:
	/* 点击类型（1蜘蛛 2石碑） */
	#类型:uint32
	public type;


	"""
		pri_in=cli_click_stat_in();

		pri_in.type=type
		self.sendmsg(1407, pri_in );
		pass

	def cli_click_game_notice(self  ):
		"""
点击游戏王公告:

	"""
		pri_in=None;

		self.sendmsg(1408, pri_in );
		pass

	def cli_noti_get_invitation(self  ):
		"""
通知用户得到邀请函:

	"""
		pri_in=None;

		self.sendmsg(1409, pri_in );
		pass

	def cli_noti_svr_time(self  ):
		"""
通知客户端系统时间:

	"""
		pri_in=None;

		self.sendmsg(1410, pri_in );
		pass

	def cli_click_pet_egg(self  ):
		"""
点击宠物蛋:

	"""
		pri_in=None;

		self.sendmsg(1411, pri_in );
		pass

	def cli_get_login_chest(self  ):
		"""
获取登录宝箱:

	"""
		pri_in=None;

		self.sendmsg(1412, pri_in );
		pass

	def cli_get_login_reward(self ,index ):
		"""
获取登录奖励:
	/* 序号（0~6） */
	#类型:uint32
	public index;


	"""
		pri_in=cli_get_login_reward_in();

		pri_in.index=index
		self.sendmsg(1413, pri_in );
		pass


