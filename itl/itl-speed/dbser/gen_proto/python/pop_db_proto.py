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

		
class item_exchange_t :
	"""
参数列表:
	/*  */
	#类型:uint32
	public itemid;

	/*  */
	#类型:uint32
	public count;

	/*  */
	#类型:uint32
	public max_count;


	"""

	def read_from_buf(self, ba ):
		self.itemid=ba.read_uint32();
		if ( self.itemid== None): return False;
		self.count=ba.read_uint32();
		if ( self.count== None): return False;
		self.max_count=ba.read_uint32();
		if ( self.max_count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.itemid)):return False;
		if ( not ba.write_uint32(self.count)):return False;
		if ( not ba.write_uint32(self.max_count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[itemid]=%s"%(str(self.itemid))
		print tabs+"""/*  */"""
		print tabs+"[count]=%s"%(str(self.count))
		print tabs+"""/*  */"""
		print tabs+"[max_count]=%s"%(str(self.max_count))
		
		pass;
	




class pop_exchange_in :
	"""
参数列表:
	/*  */
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
		if (add_item_list_count>9999):return False;
		self.add_item_list=[];
		for i in range(add_item_list_count):
			self.add_item_list.append(item_exchange_t());
			if ( not self.add_item_list[i].read_from_buf(ba)):return False;


		del_item_list_count= ba.read_uint32();
		if ( del_item_list_count==None ): return False;
		if (del_item_list_count>9999):return False;
		self.del_item_list=[];
		for i in range(del_item_list_count):
			self.del_item_list.append(item_exchange_t());
			if ( not self.del_item_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.opt_flag)):return False;
		add_item_list_count=len(self.add_item_list);
		if (add_item_list_count>9999 ): return False; 
		ba.write_uint32(add_item_list_count);
		for i in range(add_item_list_count):
			if (  not  isinstance(self.add_item_list[i] ,item_exchange_t) ):return False; 
			if ( not self.add_item_list[i].write_to_buf(ba)): return False;

		del_item_list_count=len(self.del_item_list);
		if (del_item_list_count>9999 ): return False; 
		ba.write_uint32(del_item_list_count);
		for i in range(del_item_list_count):
			if (  not  isinstance(self.del_item_list[i] ,item_exchange_t) ):return False; 
			if ( not self.del_item_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
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
	




class pop_login_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public register_time;

	/*  */
	#类型:uint32
	public sex;

	/*  */
	#类型:uint32
	public age;

	/*  */
	#定长数组,长度:16, 类型:char 
	public nick ;

	/**/
	#变长数组,最大长度:9999, 类型:task_t
	public task_list =array();


	"""

	def read_from_buf(self, ba ):
		self.register_time=ba.read_uint32();
		if ( self.register_time== None): return False;
		self.sex=ba.read_uint32();
		if ( self.sex== None): return False;
		self.age=ba.read_uint32();
		if ( self.age== None): return False;
		self.nick=ba.read_buf(16);
		if( self.nick==None ): return False;

		task_list_count= ba.read_uint32();
		if ( task_list_count==None ): return False;
		if (task_list_count>9999):return False;
		self.task_list=[];
		for i in range(task_list_count):
			self.task_list.append(task_t());
			if ( not self.task_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.register_time)):return False;
		if ( not ba.write_uint32(self.sex)):return False;
		if ( not ba.write_uint32(self.age)):return False;
		ba.write_buf(self.nick,16);
		task_list_count=len(self.task_list);
		if (task_list_count>9999 ): return False; 
		ba.write_uint32(task_list_count);
		for i in range(task_list_count):
			if (  not  isinstance(self.task_list[i] ,task_t) ):return False; 
			if ( not self.task_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[register_time]=%s"%(str(self.register_time))
		print tabs+"""/*  */"""
		print tabs+"[sex]=%s"%(str(self.sex))
		print tabs+"""/*  */"""
		print tabs+"[age]=%s"%(str(self.age))
		print tabs+"""/*  */"""
		tmp_str=str(self.nick);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[nick]=%s"%(tmp_str)
		print tabs+"#"
		print tabs+"[task_list]=>{";
		for i in range(len(self.task_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.task_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class pop_reg_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public sex;

	/*  */
	#类型:uint32
	public age;

	/*  */
	#定长数组,长度:16, 类型:char 
	public nick ;

	/*  */
	#类型:uint32
	public item_configid;


	"""

	def read_from_buf(self, ba ):
		self.sex=ba.read_uint32();
		if ( self.sex== None): return False;
		self.age=ba.read_uint32();
		if ( self.age== None): return False;
		self.nick=ba.read_buf(16);
		if( self.nick==None ): return False;
		self.item_configid=ba.read_uint32();
		if ( self.item_configid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.sex)):return False;
		if ( not ba.write_uint32(self.age)):return False;
		ba.write_buf(self.nick,16);
		if ( not ba.write_uint32(self.item_configid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[sex]=%s"%(str(self.sex))
		print tabs+"""/*  */"""
		print tabs+"[age]=%s"%(str(self.age))
		print tabs+"""/*  */"""
		tmp_str=str(self.nick);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[nick]=%s"%(tmp_str)
		print tabs+"""/*  */"""
		print tabs+"[item_configid]=%s"%(str(self.item_configid))
		
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
	




class pop_task_get_list_out :
	"""
参数列表:
	/**/
	#变长数组,最大长度:9999, 类型:task_t
	public task_list =array();


	"""

	def read_from_buf(self, ba ):

		task_list_count= ba.read_uint32();
		if ( task_list_count==None ): return False;
		if (task_list_count>9999):return False;
		self.task_list=[];
		for i in range(task_list_count):
			self.task_list.append(task_t());
			if ( not self.task_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		task_list_count=len(self.task_list);
		if (task_list_count>9999 ): return False; 
		ba.write_uint32(task_list_count);
		for i in range(task_list_count):
			if (  not  isinstance(self.task_list[i] ,task_t) ):return False; 
			if ( not self.task_list[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"#"
		print tabs+"[task_list]=>{";
		for i in range(len(self.task_list)):
			print tabs+"    [%d]=>{"%(i) 
			self.task_list[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
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
	




create_time='2011年05月18日 星期3 10:31:18'
cmd_map={ }
cmd_map[0x7401]=(0x7401,'pop_login',None,pop_login_out,0x45b4c3d2)
cmd_map[0x7402]=(0x7402,'pop_reg',pop_reg_in,None,0xe0fcffe6)
cmd_map[0x740A]=(0x740A,'pop_exchange',pop_exchange_in,None,0x97036323)
cmd_map[0x7410]=(0x7410,'pop_task_complete_node',pop_task_complete_node_in,None,0x18cc6534)
cmd_map[0x7411]=(0x7411,'pop_task_get_list',None,pop_task_get_list_out,0x8029753a)
	
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

	def pop_login(self  ):
		"""
登入:

	"""
		pri_in=None;

		self.sendmsg(0x7401, pri_in );
		pass

	def pop_reg(self ,sex,age,nick,item_configid ):
		"""
注册:
	/*  */
	#类型:uint32
	public item_configid;


	"""
		pri_in=pop_reg_in();

		pri_in.sex=sex
		pri_in.age=age
		pri_in.nick=nick
		pri_in.item_configid=item_configid
		self.sendmsg(0x7402, pri_in );
		pass

	def pop_exchange(self ,opt_flag,add_item_list,del_item_list ):
		"""
交换物品:
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

	def pop_task_get_list(self  ):
		"""
得到完成任务的进度列表:

	"""
		pri_in=None;

		self.sendmsg(0x7411, pri_in );
		pass


