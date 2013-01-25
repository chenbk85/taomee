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

		
class cli_engage_survey_in :
	"""
参数列表:
	/* 年龄选项 */
	#类型:uint32
	public age_option;

	/* 性别选项 */
	#类型:uint32
	public sex_option;


	"""

	def read_from_buf(self, ba ):
		self.age_option=ba.read_uint32();
		if ( self.age_option== None): return False;
		self.sex_option=ba.read_uint32();
		if ( self.sex_option== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.age_option)):return False;
		if ( not ba.write_uint32(self.sex_option)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 年龄选项 */"""
		print tabs+"[age_option]=%s"%(str(self.age_option))
		print tabs+"""/* 性别选项 */"""
		print tabs+"[sex_option]=%s"%(str(self.sex_option))
		
		pass;
	




class cli_fishing_get_item_in :
	"""
参数列表:
	/* 获得的物品ID */
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
		print tabs+"""/* 获得的物品ID */"""
		print tabs+"[itemid]=%s"%(str(self.itemid))
		
		pass;
	




class cli_fishing_get_item_out :
	"""
参数列表:
	/* 获得的物品ID */
	#类型:uint32
	public itemid;

	/* 获得物品的位置（0背包，1仓库） */
	#类型:uint32
	public pos;


	"""

	def read_from_buf(self, ba ):
		self.itemid=ba.read_uint32();
		if ( self.itemid== None): return False;
		self.pos=ba.read_uint32();
		if ( self.pos== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.itemid)):return False;
		if ( not ba.write_uint32(self.pos)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 获得的物品ID */"""
		print tabs+"[itemid]=%s"%(str(self.itemid))
		print tabs+"""/* 获得物品的位置（0背包，1仓库） */"""
		print tabs+"[pos]=%s"%(str(self.pos))
		
		pass;
	




class cli_get_survey_info_out :
	"""
参数列表:
	/* 是否参与调查0未参与1已参与 */
	#类型:uint32
	public surveyed;


	"""

	def read_from_buf(self, ba ):
		self.surveyed=ba.read_uint32();
		if ( self.surveyed== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.surveyed)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 是否参与调查0未参与1已参与 */"""
		print tabs+"[surveyed]=%s"%(str(self.surveyed))
		
		pass;
	




class vip_item_info_t :
	"""
参数列表:
	/*  */
	#类型:uint32
	public same_period_randid;

	/*  */
	#类型:uint32
	public same_period_count;


	"""

	def read_from_buf(self, ba ):
		self.same_period_randid=ba.read_uint32();
		if ( self.same_period_randid== None): return False;
		self.same_period_count=ba.read_uint32();
		if ( self.same_period_count== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.same_period_randid)):return False;
		if ( not ba.write_uint32(self.same_period_count)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[same_period_randid]=%s"%(str(self.same_period_randid))
		print tabs+"""/*  */"""
		print tabs+"[same_period_count]=%s"%(str(self.same_period_count))
		
		pass;
	




class cli_get_vip_item_cnt_out :
	"""
参数列表:
	/**/
	#变长数组,最大长度:9999, 类型:vip_item_info_t
	public item_list =array();


	"""

	def read_from_buf(self, ba ):

		item_list_count= ba.read_uint32();
		if ( item_list_count==None ): return False;
		if (item_list_count>9999):return False;
		self.item_list=[];
		for i in range(item_list_count):
			self.item_list.append(vip_item_info_t());
			if ( not self.item_list[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		item_list_count=len(self.item_list);
		if (item_list_count>9999 ): return False; 
		ba.write_uint32(item_list_count);
		for i in range(item_list_count):
			if (  not  isinstance(self.item_list[i] ,vip_item_info_t) ):return False; 
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
	




class cli_set_injury_in :
	"""
参数列表:
	/* 受伤等级 */
	#类型:uint32
	public injurylv;


	"""

	def read_from_buf(self, ba ):
		self.injurylv=ba.read_uint32();
		if ( self.injurylv== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.injurylv)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 受伤等级 */"""
		print tabs+"[injurylv]=%s"%(str(self.injurylv))
		
		pass;
	




class cli_set_injury_out :
	"""
参数列表:
	/* 用户ID */
	#类型:uint32
	public userid;

	/* hp当前值 */
	#类型:uint32
	public hp;

	/* mp当前值 */
	#类型:uint32
	public mp;

	/* hp最大值 */
	#类型:uint32
	public hpmax;

	/* mp最大值 */
	#类型:uint32
	public mpmax;

	/* 受伤等级 */
	#类型:uint32
	public injurylv;


	"""

	def read_from_buf(self, ba ):
		self.userid=ba.read_uint32();
		if ( self.userid== None): return False;
		self.hp=ba.read_uint32();
		if ( self.hp== None): return False;
		self.mp=ba.read_uint32();
		if ( self.mp== None): return False;
		self.hpmax=ba.read_uint32();
		if ( self.hpmax== None): return False;
		self.mpmax=ba.read_uint32();
		if ( self.mpmax== None): return False;
		self.injurylv=ba.read_uint32();
		if ( self.injurylv== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.userid)):return False;
		if ( not ba.write_uint32(self.hp)):return False;
		if ( not ba.write_uint32(self.mp)):return False;
		if ( not ba.write_uint32(self.hpmax)):return False;
		if ( not ba.write_uint32(self.mpmax)):return False;
		if ( not ba.write_uint32(self.injurylv)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 用户ID */"""
		print tabs+"[userid]=%s"%(str(self.userid))
		print tabs+"""/* hp当前值 */"""
		print tabs+"[hp]=%s"%(str(self.hp))
		print tabs+"""/* mp当前值 */"""
		print tabs+"[mp]=%s"%(str(self.mp))
		print tabs+"""/* hp最大值 */"""
		print tabs+"[hpmax]=%s"%(str(self.hpmax))
		print tabs+"""/* mp最大值 */"""
		print tabs+"[mpmax]=%s"%(str(self.mpmax))
		print tabs+"""/* 受伤等级 */"""
		print tabs+"[injurylv]=%s"%(str(self.injurylv))
		
		pass;
	




class cli_user_walk_in :
	"""
参数列表:
	/*  */
	#类型:uint16
	public sx;

	/*  */
	#类型:uint16
	public sy;

	/*  */
	#类型:uint32
	public ex;

	/*  */
	#类型:uint32
	public ey;

	/*  */
	#类型:uint16
	public step;

	/*  */
	#类型:uint32
	public direction;

	/*  */
	#类型:uint32
	public type;

	/*  */
	#类型:uint16
	public nouse;


	"""

	def read_from_buf(self, ba ):
		self.sx=ba.read_uint16();
		if ( self.sx== None): return False;
		self.sy=ba.read_uint16();
		if ( self.sy== None): return False;
		self.ex=ba.read_uint32();
		if ( self.ex== None): return False;
		self.ey=ba.read_uint32();
		if ( self.ey== None): return False;
		self.step=ba.read_uint16();
		if ( self.step== None): return False;
		self.direction=ba.read_uint32();
		if ( self.direction== None): return False;
		self.type=ba.read_uint32();
		if ( self.type== None): return False;
		self.nouse=ba.read_uint16();
		if ( self.nouse== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint16(self.sx)):return False;
		if ( not ba.write_uint16(self.sy)):return False;
		if ( not ba.write_uint32(self.ex)):return False;
		if ( not ba.write_uint32(self.ey)):return False;
		if ( not ba.write_uint16(self.step)):return False;
		if ( not ba.write_uint32(self.direction)):return False;
		if ( not ba.write_uint32(self.type)):return False;
		if ( not ba.write_uint16(self.nouse)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[sx]=%s"%(str(self.sx))
		print tabs+"""/*  */"""
		print tabs+"[sy]=%s"%(str(self.sy))
		print tabs+"""/*  */"""
		print tabs+"[ex]=%s"%(str(self.ex))
		print tabs+"""/*  */"""
		print tabs+"[ey]=%s"%(str(self.ey))
		print tabs+"""/*  */"""
		print tabs+"[step]=%s"%(str(self.step))
		print tabs+"""/*  */"""
		print tabs+"[direction]=%s"%(str(self.direction))
		print tabs+"""/*  */"""
		print tabs+"[type]=%s"%(str(self.type))
		print tabs+"""/*  */"""
		print tabs+"[nouse]=%s"%(str(self.nouse))
		
		pass;
	




class cli_user_walk_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public id;

	/*  */
	#类型:uint32
	public posX;

	/*  */
	#类型:uint32
	public posY;

	/*  */
	#类型:uint16
	public left_step;

	/*  */
	#类型:uint32
	public direction;

	/*  */
	#类型:uint32
	public type;


	"""

	def read_from_buf(self, ba ):
		self.id=ba.read_uint32();
		if ( self.id== None): return False;
		self.posX=ba.read_uint32();
		if ( self.posX== None): return False;
		self.posY=ba.read_uint32();
		if ( self.posY== None): return False;
		self.left_step=ba.read_uint16();
		if ( self.left_step== None): return False;
		self.direction=ba.read_uint32();
		if ( self.direction== None): return False;
		self.type=ba.read_uint32();
		if ( self.type== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.id)):return False;
		if ( not ba.write_uint32(self.posX)):return False;
		if ( not ba.write_uint32(self.posY)):return False;
		if ( not ba.write_uint16(self.left_step)):return False;
		if ( not ba.write_uint32(self.direction)):return False;
		if ( not ba.write_uint32(self.type)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[id]=%s"%(str(self.id))
		print tabs+"""/*  */"""
		print tabs+"[posX]=%s"%(str(self.posX))
		print tabs+"""/*  */"""
		print tabs+"[posY]=%s"%(str(self.posY))
		print tabs+"""/*  */"""
		print tabs+"[left_step]=%s"%(str(self.left_step))
		print tabs+"""/*  */"""
		print tabs+"[direction]=%s"%(str(self.direction))
		print tabs+"""/*  */"""
		print tabs+"[type]=%s"%(str(self.type))
		
		pass;
	




class reset_pet_add_attr_in :
	"""
参数列表:
	/* 需要重置的宠物ID */
	#类型:uint32
	public petid;

	/* 重置需要的物品 */
	#类型:uint32
	public itemid;


	"""

	def read_from_buf(self, ba ):
		self.petid=ba.read_uint32();
		if ( self.petid== None): return False;
		self.itemid=ba.read_uint32();
		if ( self.itemid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.petid)):return False;
		if ( not ba.write_uint32(self.itemid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 需要重置的宠物ID */"""
		print tabs+"[petid]=%s"%(str(self.petid))
		print tabs+"""/* 重置需要的物品 */"""
		print tabs+"[itemid]=%s"%(str(self.itemid))
		
		pass;
	




class stru_user_skill :
	"""
参数列表:
	/* 技能ID */
	#类型:uint32
	public skillid;

	/* 技能等级 */
	#类型:uint8
	public skilllv;

	/* 技能经验 */
	#类型:uint32
	public skillexp;


	"""

	def read_from_buf(self, ba ):
		self.skillid=ba.read_uint32();
		if ( self.skillid== None): return False;
		self.skilllv=ba.read_uint8();
		if ( self.skilllv== None): return False;
		self.skillexp=ba.read_uint32();
		if ( self.skillexp== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.skillid)):return False;
		if ( not ba.write_uint8(self.skilllv)):return False;
		if ( not ba.write_uint32(self.skillexp)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 技能ID */"""
		print tabs+"[skillid]=%s"%(str(self.skillid))
		print tabs+"""/* 技能等级 */"""
		print tabs+"[skilllv]=%s"%(str(self.skilllv))
		print tabs+"""/* 技能经验 */"""
		print tabs+"[skillexp]=%s"%(str(self.skillexp))
		
		pass;
	




class reset_pet_add_attr_out :
	"""
参数列表:
	/* 需要重置的宠物ID */
	#类型:uint32
	public petid;

	/* 宠物类型 */
	#类型:uint32
	public pettype;

	/* 宠物种族 */
	#类型:uint8
	public race;

	/* 宠物标识 */
	#类型:uint32
	public flag;

	/* 宠物昵称 */
	#定长数组,长度:16, 类型:char 
	public nick ;

	/* 等级 */
	#类型:uint32
	public level;

	/* 宠物经验 */
	#类型:uint32
	public experience;

	/* 体力 */
	#类型:uint16
	public physique;

	/* 力量 */
	#类型:uint16
	public strength;

	/* 耐力 */
	#类型:uint16
	public endurance;

	/* 速度 */
	#类型:uint16
	public quick;

	/* 智力 */
	#类型:uint16
	public intelligence;

	/* 剩余属性点 */
	#类型:uint16
	public attr_addition;

	/* 已配置属性点 */
	#类型:uint16
	public attr_add;

	/* hp */
	#类型:uint32
	public hp;

	/* mp */
	#类型:uint32
	public mp;

	/* 地 */
	#类型:uint8
	public earth;

	/* 水 */
	#类型:uint8
	public water;

	/* 火 */
	#类型:uint8
	public fire;

	/* 风 */
	#类型:uint8
	public wind;

	/* 受伤等级 */
	#类型:uint32
	public injurylv;

	/* 出战状态 */
	#类型:uint8
	public state;

	/* hp上限 */
	#类型:uint32
	public hpmax;

	/* mp上限 */
	#类型:uint32
	public mpmax;

	/* 物攻 */
	#类型:uint16
	public attack;

	/* 物防 */
	#类型:uint16
	public defense;

	/* 敏捷 */
	#类型:uint16
	public speed;

	/* 精神 */
	#类型:uint16
	public spirit;

	/* 回复 */
	#类型:uint16
	public resume;

	/* 命中 */
	#类型:uint16
	public hit;

	/* 闪避 */
	#类型:uint16
	public avoid;

	/* 必杀 */
	#类型:uint16
	public bisha;

	/* 反击 */
	#类型:uint16
	public fightback;

	/* 成长值 */
	#类型:uint16
	public para_all;

	/*宠物技能*/
	#变长数组,最大长度:10, 类型:stru_user_skill
	public skills =array();


	"""

	def read_from_buf(self, ba ):
		self.petid=ba.read_uint32();
		if ( self.petid== None): return False;
		self.pettype=ba.read_uint32();
		if ( self.pettype== None): return False;
		self.race=ba.read_uint8();
		if ( self.race== None): return False;
		self.flag=ba.read_uint32();
		if ( self.flag== None): return False;
		self.nick=ba.read_buf(16);
		if( self.nick==None ): return False;
		self.level=ba.read_uint32();
		if ( self.level== None): return False;
		self.experience=ba.read_uint32();
		if ( self.experience== None): return False;
		self.physique=ba.read_uint16();
		if ( self.physique== None): return False;
		self.strength=ba.read_uint16();
		if ( self.strength== None): return False;
		self.endurance=ba.read_uint16();
		if ( self.endurance== None): return False;
		self.quick=ba.read_uint16();
		if ( self.quick== None): return False;
		self.intelligence=ba.read_uint16();
		if ( self.intelligence== None): return False;
		self.attr_addition=ba.read_uint16();
		if ( self.attr_addition== None): return False;
		self.attr_add=ba.read_uint16();
		if ( self.attr_add== None): return False;
		self.hp=ba.read_uint32();
		if ( self.hp== None): return False;
		self.mp=ba.read_uint32();
		if ( self.mp== None): return False;
		self.earth=ba.read_uint8();
		if ( self.earth== None): return False;
		self.water=ba.read_uint8();
		if ( self.water== None): return False;
		self.fire=ba.read_uint8();
		if ( self.fire== None): return False;
		self.wind=ba.read_uint8();
		if ( self.wind== None): return False;
		self.injurylv=ba.read_uint32();
		if ( self.injurylv== None): return False;
		self.state=ba.read_uint8();
		if ( self.state== None): return False;
		self.hpmax=ba.read_uint32();
		if ( self.hpmax== None): return False;
		self.mpmax=ba.read_uint32();
		if ( self.mpmax== None): return False;
		self.attack=ba.read_uint16();
		if ( self.attack== None): return False;
		self.defense=ba.read_uint16();
		if ( self.defense== None): return False;
		self.speed=ba.read_uint16();
		if ( self.speed== None): return False;
		self.spirit=ba.read_uint16();
		if ( self.spirit== None): return False;
		self.resume=ba.read_uint16();
		if ( self.resume== None): return False;
		self.hit=ba.read_uint16();
		if ( self.hit== None): return False;
		self.avoid=ba.read_uint16();
		if ( self.avoid== None): return False;
		self.bisha=ba.read_uint16();
		if ( self.bisha== None): return False;
		self.fightback=ba.read_uint16();
		if ( self.fightback== None): return False;
		self.para_all=ba.read_uint16();
		if ( self.para_all== None): return False;

		skills_count= ba.read_uint32();
		if ( skills_count==None ): return False;
		if (skills_count>10):return False;
		self.skills=[];
		for i in range(skills_count):
			self.skills.append(stru_user_skill());
			if ( not self.skills[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.petid)):return False;
		if ( not ba.write_uint32(self.pettype)):return False;
		if ( not ba.write_uint8(self.race)):return False;
		if ( not ba.write_uint32(self.flag)):return False;
		ba.write_buf(self.nick,16);
		if ( not ba.write_uint32(self.level)):return False;
		if ( not ba.write_uint32(self.experience)):return False;
		if ( not ba.write_uint16(self.physique)):return False;
		if ( not ba.write_uint16(self.strength)):return False;
		if ( not ba.write_uint16(self.endurance)):return False;
		if ( not ba.write_uint16(self.quick)):return False;
		if ( not ba.write_uint16(self.intelligence)):return False;
		if ( not ba.write_uint16(self.attr_addition)):return False;
		if ( not ba.write_uint16(self.attr_add)):return False;
		if ( not ba.write_uint32(self.hp)):return False;
		if ( not ba.write_uint32(self.mp)):return False;
		if ( not ba.write_uint8(self.earth)):return False;
		if ( not ba.write_uint8(self.water)):return False;
		if ( not ba.write_uint8(self.fire)):return False;
		if ( not ba.write_uint8(self.wind)):return False;
		if ( not ba.write_uint32(self.injurylv)):return False;
		if ( not ba.write_uint8(self.state)):return False;
		if ( not ba.write_uint32(self.hpmax)):return False;
		if ( not ba.write_uint32(self.mpmax)):return False;
		if ( not ba.write_uint16(self.attack)):return False;
		if ( not ba.write_uint16(self.defense)):return False;
		if ( not ba.write_uint16(self.speed)):return False;
		if ( not ba.write_uint16(self.spirit)):return False;
		if ( not ba.write_uint16(self.resume)):return False;
		if ( not ba.write_uint16(self.hit)):return False;
		if ( not ba.write_uint16(self.avoid)):return False;
		if ( not ba.write_uint16(self.bisha)):return False;
		if ( not ba.write_uint16(self.fightback)):return False;
		if ( not ba.write_uint16(self.para_all)):return False;
		skills_count=len(self.skills);
		if (skills_count>10 ): return False; 
		ba.write_uint32(skills_count);
		for i in range(skills_count):
			if (  not  isinstance(self.skills[i] ,stru_user_skill) ):return False; 
			if ( not self.skills[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 需要重置的宠物ID */"""
		print tabs+"[petid]=%s"%(str(self.petid))
		print tabs+"""/* 宠物类型 */"""
		print tabs+"[pettype]=%s"%(str(self.pettype))
		print tabs+"""/* 宠物种族 */"""
		print tabs+"[race]=%s"%(str(self.race))
		print tabs+"""/* 宠物标识 */"""
		print tabs+"[flag]=%s"%(str(self.flag))
		print tabs+"""/* 宠物昵称 */"""
		tmp_str=str(self.nick);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[nick]=%s"%(tmp_str)
		print tabs+"""/* 等级 */"""
		print tabs+"[level]=%s"%(str(self.level))
		print tabs+"""/* 宠物经验 */"""
		print tabs+"[experience]=%s"%(str(self.experience))
		print tabs+"""/* 体力 */"""
		print tabs+"[physique]=%s"%(str(self.physique))
		print tabs+"""/* 力量 */"""
		print tabs+"[strength]=%s"%(str(self.strength))
		print tabs+"""/* 耐力 */"""
		print tabs+"[endurance]=%s"%(str(self.endurance))
		print tabs+"""/* 速度 */"""
		print tabs+"[quick]=%s"%(str(self.quick))
		print tabs+"""/* 智力 */"""
		print tabs+"[intelligence]=%s"%(str(self.intelligence))
		print tabs+"""/* 剩余属性点 */"""
		print tabs+"[attr_addition]=%s"%(str(self.attr_addition))
		print tabs+"""/* 已配置属性点 */"""
		print tabs+"[attr_add]=%s"%(str(self.attr_add))
		print tabs+"""/* hp */"""
		print tabs+"[hp]=%s"%(str(self.hp))
		print tabs+"""/* mp */"""
		print tabs+"[mp]=%s"%(str(self.mp))
		print tabs+"""/* 地 */"""
		print tabs+"[earth]=%s"%(str(self.earth))
		print tabs+"""/* 水 */"""
		print tabs+"[water]=%s"%(str(self.water))
		print tabs+"""/* 火 */"""
		print tabs+"[fire]=%s"%(str(self.fire))
		print tabs+"""/* 风 */"""
		print tabs+"[wind]=%s"%(str(self.wind))
		print tabs+"""/* 受伤等级 */"""
		print tabs+"[injurylv]=%s"%(str(self.injurylv))
		print tabs+"""/* 出战状态 */"""
		print tabs+"[state]=%s"%(str(self.state))
		print tabs+"""/* hp上限 */"""
		print tabs+"[hpmax]=%s"%(str(self.hpmax))
		print tabs+"""/* mp上限 */"""
		print tabs+"[mpmax]=%s"%(str(self.mpmax))
		print tabs+"""/* 物攻 */"""
		print tabs+"[attack]=%s"%(str(self.attack))
		print tabs+"""/* 物防 */"""
		print tabs+"[defense]=%s"%(str(self.defense))
		print tabs+"""/* 敏捷 */"""
		print tabs+"[speed]=%s"%(str(self.speed))
		print tabs+"""/* 精神 */"""
		print tabs+"[spirit]=%s"%(str(self.spirit))
		print tabs+"""/* 回复 */"""
		print tabs+"[resume]=%s"%(str(self.resume))
		print tabs+"""/* 命中 */"""
		print tabs+"[hit]=%s"%(str(self.hit))
		print tabs+"""/* 闪避 */"""
		print tabs+"[avoid]=%s"%(str(self.avoid))
		print tabs+"""/* 必杀 */"""
		print tabs+"[bisha]=%s"%(str(self.bisha))
		print tabs+"""/* 反击 */"""
		print tabs+"[fightback]=%s"%(str(self.fightback))
		print tabs+"""/* 成长值 */"""
		print tabs+"[para_all]=%s"%(str(self.para_all))
		print tabs+"#宠物技能"
		print tabs+"[skills]=>{";
		for i in range(len(self.skills)):
			print tabs+"    [%d]=>{"%(i) 
			self.skills[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class stru_cli_pet_info :
	"""
参数列表:
	/* 需要重置的宠物ID */
	#类型:uint32
	public petid;

	/* 宠物类型 */
	#类型:uint32
	public pettype;

	/* 宠物种族 */
	#类型:uint8
	public race;

	/* 宠物标识 */
	#类型:uint32
	public flag;

	/* 宠物昵称 */
	#定长数组,长度:16, 类型:char 
	public nick ;

	/* 等级 */
	#类型:uint32
	public level;

	/* 宠物经验 */
	#类型:uint32
	public experience;

	/* 体力 */
	#类型:uint16
	public physique;

	/* 力量 */
	#类型:uint16
	public strength;

	/* 耐力 */
	#类型:uint16
	public endurance;

	/* 速度 */
	#类型:uint16
	public quick;

	/* 智力 */
	#类型:uint16
	public intelligence;

	/* 剩余属性点 */
	#类型:uint16
	public attr_addition;

	/* 已配置属性点 */
	#类型:uint16
	public attr_add;

	/* hp */
	#类型:uint32
	public hp;

	/* mp */
	#类型:uint32
	public mp;

	/* 地 */
	#类型:uint8
	public earth;

	/* 水 */
	#类型:uint8
	public water;

	/* 火 */
	#类型:uint8
	public fire;

	/* 风 */
	#类型:uint8
	public wind;

	/* 受伤等级 */
	#类型:uint32
	public injurylv;

	/* 出战状态 */
	#类型:uint8
	public state;

	/* hp上限 */
	#类型:uint32
	public hpmax;

	/* mp上限 */
	#类型:uint32
	public mpmax;

	/* 物攻 */
	#类型:uint16
	public attack;

	/* 物防 */
	#类型:uint16
	public defense;

	/* 敏捷 */
	#类型:uint16
	public speed;

	/* 精神 */
	#类型:uint16
	public spirit;

	/* 回复 */
	#类型:int16
	public resume;

	/* 命中 */
	#类型:uint16
	public hit;

	/* 闪避 */
	#类型:uint16
	public avoid;

	/* 必杀 */
	#类型:uint16
	public bisha;

	/* 反击 */
	#类型:uint16
	public fightback;

	/* 成长值 */
	#类型:uint16
	public para_all;

	/*宠物技能*/
	#变长数组,最大长度:10, 类型:stru_user_skill
	public skills =array();


	"""

	def read_from_buf(self, ba ):
		self.petid=ba.read_uint32();
		if ( self.petid== None): return False;
		self.pettype=ba.read_uint32();
		if ( self.pettype== None): return False;
		self.race=ba.read_uint8();
		if ( self.race== None): return False;
		self.flag=ba.read_uint32();
		if ( self.flag== None): return False;
		self.nick=ba.read_buf(16);
		if( self.nick==None ): return False;
		self.level=ba.read_uint32();
		if ( self.level== None): return False;
		self.experience=ba.read_uint32();
		if ( self.experience== None): return False;
		self.physique=ba.read_uint16();
		if ( self.physique== None): return False;
		self.strength=ba.read_uint16();
		if ( self.strength== None): return False;
		self.endurance=ba.read_uint16();
		if ( self.endurance== None): return False;
		self.quick=ba.read_uint16();
		if ( self.quick== None): return False;
		self.intelligence=ba.read_uint16();
		if ( self.intelligence== None): return False;
		self.attr_addition=ba.read_uint16();
		if ( self.attr_addition== None): return False;
		self.attr_add=ba.read_uint16();
		if ( self.attr_add== None): return False;
		self.hp=ba.read_uint32();
		if ( self.hp== None): return False;
		self.mp=ba.read_uint32();
		if ( self.mp== None): return False;
		self.earth=ba.read_uint8();
		if ( self.earth== None): return False;
		self.water=ba.read_uint8();
		if ( self.water== None): return False;
		self.fire=ba.read_uint8();
		if ( self.fire== None): return False;
		self.wind=ba.read_uint8();
		if ( self.wind== None): return False;
		self.injurylv=ba.read_uint32();
		if ( self.injurylv== None): return False;
		self.state=ba.read_uint8();
		if ( self.state== None): return False;
		self.hpmax=ba.read_uint32();
		if ( self.hpmax== None): return False;
		self.mpmax=ba.read_uint32();
		if ( self.mpmax== None): return False;
		self.attack=ba.read_uint16();
		if ( self.attack== None): return False;
		self.defense=ba.read_uint16();
		if ( self.defense== None): return False;
		self.speed=ba.read_uint16();
		if ( self.speed== None): return False;
		self.spirit=ba.read_uint16();
		if ( self.spirit== None): return False;
		self.resume=ba.read_int16();
		if ( self.resume== None): return False;
		self.hit=ba.read_uint16();
		if ( self.hit== None): return False;
		self.avoid=ba.read_uint16();
		if ( self.avoid== None): return False;
		self.bisha=ba.read_uint16();
		if ( self.bisha== None): return False;
		self.fightback=ba.read_uint16();
		if ( self.fightback== None): return False;
		self.para_all=ba.read_uint16();
		if ( self.para_all== None): return False;

		skills_count= ba.read_uint32();
		if ( skills_count==None ): return False;
		if (skills_count>10):return False;
		self.skills=[];
		for i in range(skills_count):
			self.skills.append(stru_user_skill());
			if ( not self.skills[i].read_from_buf(ba)):return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.petid)):return False;
		if ( not ba.write_uint32(self.pettype)):return False;
		if ( not ba.write_uint8(self.race)):return False;
		if ( not ba.write_uint32(self.flag)):return False;
		ba.write_buf(self.nick,16);
		if ( not ba.write_uint32(self.level)):return False;
		if ( not ba.write_uint32(self.experience)):return False;
		if ( not ba.write_uint16(self.physique)):return False;
		if ( not ba.write_uint16(self.strength)):return False;
		if ( not ba.write_uint16(self.endurance)):return False;
		if ( not ba.write_uint16(self.quick)):return False;
		if ( not ba.write_uint16(self.intelligence)):return False;
		if ( not ba.write_uint16(self.attr_addition)):return False;
		if ( not ba.write_uint16(self.attr_add)):return False;
		if ( not ba.write_uint32(self.hp)):return False;
		if ( not ba.write_uint32(self.mp)):return False;
		if ( not ba.write_uint8(self.earth)):return False;
		if ( not ba.write_uint8(self.water)):return False;
		if ( not ba.write_uint8(self.fire)):return False;
		if ( not ba.write_uint8(self.wind)):return False;
		if ( not ba.write_uint32(self.injurylv)):return False;
		if ( not ba.write_uint8(self.state)):return False;
		if ( not ba.write_uint32(self.hpmax)):return False;
		if ( not ba.write_uint32(self.mpmax)):return False;
		if ( not ba.write_uint16(self.attack)):return False;
		if ( not ba.write_uint16(self.defense)):return False;
		if ( not ba.write_uint16(self.speed)):return False;
		if ( not ba.write_uint16(self.spirit)):return False;
		if ( not ba.write_int16(self.resume)):return False;
		if ( not ba.write_uint16(self.hit)):return False;
		if ( not ba.write_uint16(self.avoid)):return False;
		if ( not ba.write_uint16(self.bisha)):return False;
		if ( not ba.write_uint16(self.fightback)):return False;
		if ( not ba.write_uint16(self.para_all)):return False;
		skills_count=len(self.skills);
		if (skills_count>10 ): return False; 
		ba.write_uint32(skills_count);
		for i in range(skills_count):
			if (  not  isinstance(self.skills[i] ,stru_user_skill) ):return False; 
			if ( not self.skills[i].write_to_buf(ba)): return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 需要重置的宠物ID */"""
		print tabs+"[petid]=%s"%(str(self.petid))
		print tabs+"""/* 宠物类型 */"""
		print tabs+"[pettype]=%s"%(str(self.pettype))
		print tabs+"""/* 宠物种族 */"""
		print tabs+"[race]=%s"%(str(self.race))
		print tabs+"""/* 宠物标识 */"""
		print tabs+"[flag]=%s"%(str(self.flag))
		print tabs+"""/* 宠物昵称 */"""
		tmp_str=str(self.nick);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[nick]=%s"%(tmp_str)
		print tabs+"""/* 等级 */"""
		print tabs+"[level]=%s"%(str(self.level))
		print tabs+"""/* 宠物经验 */"""
		print tabs+"[experience]=%s"%(str(self.experience))
		print tabs+"""/* 体力 */"""
		print tabs+"[physique]=%s"%(str(self.physique))
		print tabs+"""/* 力量 */"""
		print tabs+"[strength]=%s"%(str(self.strength))
		print tabs+"""/* 耐力 */"""
		print tabs+"[endurance]=%s"%(str(self.endurance))
		print tabs+"""/* 速度 */"""
		print tabs+"[quick]=%s"%(str(self.quick))
		print tabs+"""/* 智力 */"""
		print tabs+"[intelligence]=%s"%(str(self.intelligence))
		print tabs+"""/* 剩余属性点 */"""
		print tabs+"[attr_addition]=%s"%(str(self.attr_addition))
		print tabs+"""/* 已配置属性点 */"""
		print tabs+"[attr_add]=%s"%(str(self.attr_add))
		print tabs+"""/* hp */"""
		print tabs+"[hp]=%s"%(str(self.hp))
		print tabs+"""/* mp */"""
		print tabs+"[mp]=%s"%(str(self.mp))
		print tabs+"""/* 地 */"""
		print tabs+"[earth]=%s"%(str(self.earth))
		print tabs+"""/* 水 */"""
		print tabs+"[water]=%s"%(str(self.water))
		print tabs+"""/* 火 */"""
		print tabs+"[fire]=%s"%(str(self.fire))
		print tabs+"""/* 风 */"""
		print tabs+"[wind]=%s"%(str(self.wind))
		print tabs+"""/* 受伤等级 */"""
		print tabs+"[injurylv]=%s"%(str(self.injurylv))
		print tabs+"""/* 出战状态 */"""
		print tabs+"[state]=%s"%(str(self.state))
		print tabs+"""/* hp上限 */"""
		print tabs+"[hpmax]=%s"%(str(self.hpmax))
		print tabs+"""/* mp上限 */"""
		print tabs+"[mpmax]=%s"%(str(self.mpmax))
		print tabs+"""/* 物攻 */"""
		print tabs+"[attack]=%s"%(str(self.attack))
		print tabs+"""/* 物防 */"""
		print tabs+"[defense]=%s"%(str(self.defense))
		print tabs+"""/* 敏捷 */"""
		print tabs+"[speed]=%s"%(str(self.speed))
		print tabs+"""/* 精神 */"""
		print tabs+"[spirit]=%s"%(str(self.spirit))
		print tabs+"""/* 回复 */"""
		print tabs+"[resume]=%s"%(str(self.resume))
		print tabs+"""/* 命中 */"""
		print tabs+"[hit]=%s"%(str(self.hit))
		print tabs+"""/* 闪避 */"""
		print tabs+"[avoid]=%s"%(str(self.avoid))
		print tabs+"""/* 必杀 */"""
		print tabs+"[bisha]=%s"%(str(self.bisha))
		print tabs+"""/* 反击 */"""
		print tabs+"[fightback]=%s"%(str(self.fightback))
		print tabs+"""/* 成长值 */"""
		print tabs+"[para_all]=%s"%(str(self.para_all))
		print tabs+"#宠物技能"
		print tabs+"[skills]=>{";
		for i in range(len(self.skills)):
			print tabs+"    [%d]=>{"%(i) 
			self.skills[i].echo(print_hex,tabs+"    "*2);
			print tabs+"    }\n"

		print tabs+'}'
		
		pass;
	




class stru_game_change_add :
	"""
参数列表:
	/*  */
	#类型:uint32
	public gameid;

	/* 米米号 */
	#类型:uint32
	public userid;

	/*  */
	#类型:uint32
	public logtime;

	/*  */
	#类型:uint32
	public opt_type;

	/*  */
	#类型:int32
	public ex_v1;

	/*  */
	#类型:int32
	public ex_v2;

	/*  */
	#类型:int32
	public ex_v3;

	/*  */
	#类型:int32
	public ex_v4;


	"""

	def read_from_buf(self, ba ):
		self.gameid=ba.read_uint32();
		if ( self.gameid== None): return False;
		self.userid=ba.read_uint32();
		if ( self.userid== None): return False;
		self.logtime=ba.read_uint32();
		if ( self.logtime== None): return False;
		self.opt_type=ba.read_uint32();
		if ( self.opt_type== None): return False;
		self.ex_v1=ba.read_int32();
		if ( self.ex_v1== None): return False;
		self.ex_v2=ba.read_int32();
		if ( self.ex_v2== None): return False;
		self.ex_v3=ba.read_int32();
		if ( self.ex_v3== None): return False;
		self.ex_v4=ba.read_int32();
		if ( self.ex_v4== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.gameid)):return False;
		if ( not ba.write_uint32(self.userid)):return False;
		if ( not ba.write_uint32(self.logtime)):return False;
		if ( not ba.write_uint32(self.opt_type)):return False;
		if ( not ba.write_int32(self.ex_v1)):return False;
		if ( not ba.write_int32(self.ex_v2)):return False;
		if ( not ba.write_int32(self.ex_v3)):return False;
		if ( not ba.write_int32(self.ex_v4)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[gameid]=%s"%(str(self.gameid))
		print tabs+"""/* 米米号 */"""
		print tabs+"[userid]=%s"%(str(self.userid))
		print tabs+"""/*  */"""
		print tabs+"[logtime]=%s"%(str(self.logtime))
		print tabs+"""/*  */"""
		print tabs+"[opt_type]=%s"%(str(self.opt_type))
		print tabs+"""/*  */"""
		print tabs+"[ex_v1]=%s"%(str(self.ex_v1))
		print tabs+"""/*  */"""
		print tabs+"[ex_v2]=%s"%(str(self.ex_v2))
		print tabs+"""/*  */"""
		print tabs+"[ex_v3]=%s"%(str(self.ex_v3))
		print tabs+"""/*  */"""
		print tabs+"[ex_v4]=%s"%(str(self.ex_v4))
		
		pass;
	




create_time='2011年05月09日 星期1 11:24:00'
cmd_map={ }
cmd_map[1009]=(1009,'cli_user_walk',cli_user_walk_in,cli_user_walk_out,0x40974c8b)
cmd_map[1041]=(1041,'cli_set_injury',cli_set_injury_in,cli_set_injury_out,0x08318153)
cmd_map[1565]=(1565,'reset_pet_add_attr',reset_pet_add_attr_in,reset_pet_add_attr_out,0xcdaebed9)
cmd_map[1704]=(1704,'cli_get_vip_item_cnt',None,cli_get_vip_item_cnt_out,0x40c1af1f)
cmd_map[2052]=(2052,'cli_start_fishing',None,None,0x514e22c4)
cmd_map[2053]=(2053,'cli_fishing_get_item',cli_fishing_get_item_in,cli_fishing_get_item_out,0x98111626)
cmd_map[2054]=(2054,'cli_finish_fishing',None,None,0x4b97b5fd)
cmd_map[2151]=(2151,'cli_engage_survey',cli_engage_survey_in,None,0x0ca358ea)
cmd_map[2152]=(2152,'cli_get_survey_info',None,cli_get_survey_info_out,0x38cc5954)
	
def init_cmd_map():
	pass

try:
	import mole2_online_proto_base
	base_type=mole2_online_proto_base.Cmole2_online_proto_base
except:
	base_type=object
	pass


class Cmole2_online_proto (base_type):
	def __init__(self,ip,port):
		mole2_online_proto_base.Cmole2_online_proto_base.__init__(self,ip,port)

	def cli_user_walk(self ,sx,sy,ex,ey,step,direction,type,nouse ):
		"""
用户走路:
	/*  */
	#类型:uint16
	public nouse;


	"""
		pri_in=cli_user_walk_in();

		pri_in.sx=sx
		pri_in.sy=sy
		pri_in.ex=ex
		pri_in.ey=ey
		pri_in.step=step
		pri_in.direction=direction
		pri_in.type=type
		pri_in.nouse=nouse
		self.sendmsg(1009, pri_in );
		pass

	def cli_set_injury(self ,injurylv ):
		"""
:
	/* 受伤等级 */
	#类型:uint32
	public injurylv;


	"""
		pri_in=cli_set_injury_in();

		pri_in.injurylv=injurylv
		self.sendmsg(1041, pri_in );
		pass

	def reset_pet_add_attr(self ,petid,itemid ):
		"""
重置宠物属性点:
	/* 重置需要的物品 */
	#类型:uint32
	public itemid;


	"""
		pri_in=reset_pet_add_attr_in();

		pri_in.petid=petid
		pri_in.itemid=itemid
		self.sendmsg(1565, pri_in );
		pass

	def cli_get_vip_item_cnt(self  ):
		"""
得到vip 装扮信息:

	"""
		pri_in=None;

		self.sendmsg(1704, pri_in );
		pass

	def cli_start_fishing(self  ):
		"""
开始钓鱼:

	"""
		pri_in=None;

		self.sendmsg(2052, pri_in );
		pass

	def cli_fishing_get_item(self ,itemid ):
		"""
钓鱼获得物品:
	/* 获得的物品ID */
	#类型:uint32
	public itemid;


	"""
		pri_in=cli_fishing_get_item_in();

		pri_in.itemid=itemid
		self.sendmsg(2053, pri_in );
		pass

	def cli_finish_fishing(self  ):
		"""
结束钓鱼:

	"""
		pri_in=None;

		self.sendmsg(2054, pri_in );
		pass

	def cli_engage_survey(self ,age_option,sex_option ):
		"""
调查:
	/* 性别选项 */
	#类型:uint32
	public sex_option;


	"""
		pri_in=cli_engage_survey_in();

		pri_in.age_option=age_option
		pri_in.sex_option=sex_option
		self.sendmsg(2151, pri_in );
		pass

	def cli_get_survey_info(self  ):
		"""
获取是否参与调查:

	"""
		pri_in=None;

		self.sendmsg(2152, pri_in );
		pass


