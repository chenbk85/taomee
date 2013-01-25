	--- 文件名称: batrserv_ai.lua
	--- 功    能: 摩尔勇士对战服务器ai(lua)脚本
	--- 作    者: 古红亮
	--- 加载方式: 后台每10分钟尝试加载一次ai脚本
	---           第一次加载脚本的时候记录ai脚本的文件大小和修改日期，
	---           随后每10分钟一次，通过比较这两个值来判断是否需要重新加载ai脚本。
	--- lua 版本: 5.1
	--- 特殊说明: 开发机上的lua通过make install方式安装。
	---           链接是采用静态库方式，不过需要注意的是，如果开发软件的编译参数中采用fpic,可能需要修改lua源码,
	---           具体参考编译时候的错误说明。
	--- 编    译: luac -o batrserv_ai.ai batrserv_ai.lua
	---           从安全性方面考虑，战服务器加载编译后的lua脚本
	--- 创建日期: 2010/11/15
	--- 修改履历:
	--- 修改日期: 2010/12/01
	--- 修改内容: 人物属性获取设定接口
	---           人物技能装备的属性值获取设定接口
	-------------------------------------------------------------------------
	--- 以g_开头的变量是系统全局变量
	--- 定义这些全局的变量的目的是为获取人物(包括人，宠，怪)属性
	--- 全局变量的值必须与后台的对应的值保证一致，否则接口的作用不可预期
	-------------------------------------------------------------------------
	---- 人数本身属性
	g_prop_hp              = 0;   -- 当前血量
	g_prop_hp_max          = 1;   -- 最大血量值
	g_prop_mp              = 2;   -- 当前魔法
	g_prop_mp_max          = 3;   -- 最大魔法值
	g_prop_weapon          = 4;   -- 武器类型
	g_prop_prof            = 5;   -- 职业
	g_prop_speed           = 6;   -- 速度
	g_prop_level           = 7;   -- 等级
	g_prop_typeid          = 8;   -- type id
	g_prop_injury          = 9;   -- 受伤
	g_prop_shield          = 10;  -- 盾牌
	g_prop_spirit          = 11;  -- 精神
	g_prop_resume          = 12;  -- resume
	g_prop_hit_rate        = 13;  -- 命中率(战斗中实际命中率的计算参数)
	g_prop_avoid_rate      = 14;  -- 回避率(注意实际战斗中回避率是通过命中率反向计算得出，这个值是否有用还得问online)
	g_prop_bisha           = 15;  -- 必杀(计算必杀概率的一个参数)
	g_prop_fight_back      = 16;  -- 反击
	g_prop_cloth_cnt       = 17;  -- 装备数量
	g_prop_in_front        = 18;  -- 前后排(宠物的前后排通过人的属性来判断)
	g_prop_earth           = 19;  -- 地属性
	g_prop_water           = 20;  -- 水属性
	g_prop_fire            = 21;  -- 火属性
	g_prop_wind            = 22;  -- 风属性
	g_prop_attack_val      = 23;  -- 基础物理攻击值
	g_prop_mattack_val     = 24;  -- 基础魔法攻击值
	g_prop_defense_val     = 25;  -- 物理防御值
	g_prop_mdefense_val    = 26;  -- 魔法防御值
	g_prop_race            = 27;  -- 种族
	g_prop_pet_cnt         = 28;  -- 宠物数量
	g_prop_catchable       = 29;  -- 宠物是否可抓
	g_prop_handbooklv      = 30;  -- 宠物图鉴
	g_prop_pet_contract_lv = 31;  -- 精灵契约
	g_prop_pet_state       = 32;  -- 宠物状态(等待，出战等等)
	g_prop_id              = 33;  -- id
	g_prop_pet_id          = 34;  -- 宠物id
	g_prop_skill_count     = 35;  -- 人物/宠物身上技能数量(对于人来说没有实际的意义，主要是怪物使用)
	g_prop_groupid 			= 40;  --teamid
	
	--- 技能属性
	g_skill_prop_id    = 0;  -- 技能id
	g_skill_prop_level = 1;  -- 技能等级
	g_skill_prop_rate  = 2;  -- 技能概率
	g_skill_prop_exp   = 3;  -- 技能经验
	
	--- 队伍属性
	g_mark_challenger = 0; 	 -- 挑战发起方
	g_mark_challengee = 1;   -- 挑战接受方
	
	--- 人物状态位属性
	g_shihua_bit				  = 0;
	g_yiwang_bit				  = 1;
	g_hunluan_bit				  = 2;
	g_zuzhou_bit				  = 3;
	g_zhongdu_bit				  = 4;
	g_hunshui_bit				  = 5;
	g_gongji_xishou_bit		= 6;
	g_gongji_fantan_bit		= 7;
	g_gongji_wuxiao_bit		= 8;
	g_mofa_xishou_bit			= 9;
	g_mofa_fantan_bit			= 10;
	g_mofa_wuxiao_bit			= 11; -- 魔法无效
	-----------------------------------------------------------------------
	---由于以上对应的技能目前还没有开发的原因，这些状态为目前还没有用到，
	---后台程序中有对应的处理，但是需要测试。
	-----------------------------------------------------------------------
	g_resume_hp1_bit			= 12; -- 再生之阵
	g_resume_hp2_bit			= 13; -- 再生领域
	g_resume_hp3_bit			= 14; -- 再生结界
	g_run_away_bit 			  = 32; -- 逃跑(已经)
	g_dead_bit	 			    = 33; -- 死亡
	g_attacked_out_bit	 	= 34; -- 打飞
	g_attacked_bit			  = 35; -- 被打(返回给客户端，服务器不保存)
	g_fangyu_bit				  = 36; -- 防御
	g_huibi_bit				    = 37; -- 回避(返回给客户端，服务器不保存)
	g_meiying_bit				  = 38; -- 魅影
	g_huandun_bit				  = 39; -- 幻盾
	g_mokang_bit				  = 40; -- 魔法防御
	g_stop_attack 			  = 41; -- 停止攻击
	g_bisha_bit				    = 42; -- 必杀(返回给客户端，服务器不保存)
	g_use_item_bit			  = 43; -- 使用药品(返回给客户端，服务器不保存)
	g_revive_bit				  = 44; -- 复活(返回给客户端，服务器不保存,复活后会清除g_dead_bit, g_attacked_out_bit标志)
	g_huiji_bit           = 45; -- 回击(使用回击技能后)
	g_pause_bit           = 46; -- 停止行动(使用先制技能后)
	g_catch_pet_succ      = 47; -- 抓宠(成功)
	g_person_atk_out			= 48; -- 人被打飞(设置在宠物状态上的标志)
	g_person_break_off		= 49; -- 人掉线(设置在宠物状态上的标志)
	
--
--
--
----下面是策划提供的发招技能等级计算公式
----根据人物等级获取技能等级
function lua_get_beast_atk_level(batid, mark, pos)
	local level = lua_get_player_prop(batid, mark, pos, 7);
	---0≤Lv怪＜10	1
	if level >= 0 and level < 10 then
		return 1;
	end
	---10≤Lv怪＜20	2
	if level >= 10 and level < 20 then
		return 2;
	end
	---20≤Lv怪＜30	3
	if level >= 20 and level < 30 then
		return 3;
	end
	---30≤Lv怪＜40	4
	if level >= 30 and level < 40 then
		return 4;
	end
	---40≤Lv怪＜50	5
	if level >= 40 and level < 50 then
		return 5;
	end
	---50≤Lv怪＜60	6
	if level >= 50 and level < 60 then
		return 6;
	end
	---60≤Lv怪＜70	7
	if level >= 60 and level < 70 then
		return 7;
	end
	---70≤Lv怪＜80	8
	if level >= 70 and level < 80 then
		return 8;
	end
	---80≤Lv怪＜90	9
	if level >= 80 and level < 90 then
		return 9;
	end
	---90≤Lv怪≤100	10
	if level >= 90 and level < 100 then
		return 10;
	end
	
	return 1;
end

-- 怪物发招技能的等级是根据自身等级来定
-- 所以这里写一个函数来调用后台API【lua_set_atk_skill】
-- 不用每次都传等级参数了
function lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, atk_type)
	return lua_set_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, atk_type, lua_get_beast_atk_level(batid, mark, pos));
end

----获取对方队伍标志
function lua_get_enemy_team_mark(selfmark)
	if selfmark == g_mark_challenger then
		return g_mark_challengee;
	end
	
	return g_mark_challenger;
end

--- 土豆泥王ai模块
function lua_beast_attack_2001_ai(batid, mark, pos, atk_seq, touxi)
	-- 获取对方队伍标志
	local enemy_mark = lua_get_enemy_team_mark(mark);
	
	-- 获取当前血量
	local hp_cur = lua_get_player_prop(batid, mark, pos, g_prop_hp);
	-- 获取最大血量
	local hp_max = lua_get_player_prop(batid, mark, pos, g_prop_hp_max);
	
	local hp_rate = (hp_cur/hp_max) * 100;
	
	---血量充足，使用
	---对任一敌方对象使用5010（20%）、7001（30%）、7005（30%）、7009（20%）。
	if hp_rate >= 50 then
		---随机从对方队伍中获取一个攻击对象
		local ene_pos = lua_get_player_by_rand(batid, enemy_mark);
		--- 对方队伍中已经没有可以攻击的对象
		if ene_pos == -1 then
			-- 认为其发招成功，因为无需在发招
			return 1;
		end
		
		local rand_num = lua_cert_rand_num(100);
		if rand_num >= 0 and rand_num < 20 then
			--- 设置怪物招数
			lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 5010);
			return 1;
		end
		
		if rand_num >= 20 and rand_num < 50 then
			--- 设置怪物招数
			lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 7001);
			return 1;
		end
		
		if rand_num >= 50 and rand_num < 80 then
			--- 设置怪物招数
			lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 7005);
			return 1;
		end

		--- 剩余的20%概率使用【7009】设置怪物招数
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 7009);
		return 1;
	end

	---血量不足20%时
	---对自己使用6001（40%），对任一敌方对象使用7001（20%）、7005（20%）、7009（20%）。
	if hp_rate < 20 then
		--- 40%的概率对自己使用使用【6001】
		if lua_calc_rand_rate(40) == 1 then
			--- 先检测怪物是否有这个技能
			if lua_has_atk_type(batid, mark, pos, 6001) == 1 then
				-- 设置怪物技能
				lua_set_beast_atk_skill(batid, mark, pos, mark, pos, atk_seq, 6001);
				return 1;
			end
		end
		
		--- 攻击对方 对任一敌方对象使用7001（20%）、7005（20%）、7009（20%）。
		local rand_num = lua_cert_rand_num(60) + 40;
		
		-- 在对方队伍中随机找一个可攻击的人
		local ene_pos = lua_get_player_by_rand(batid, enemy_mark);
		
		--- 对方队伍中已经没有可以攻击的对象
		if ene_pos == -1 then
			-- 认为其发招成功，因为无需在发招
			return 1;
		end
		
		if rand_num >= 40 and rand_num < 60 then
				-- 设置怪物技能
				lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 7001);
				return 1;
		end
		
		if rand_num >= 60 and rand_num < 80 then
				-- 设置怪物技能
				lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 7005);
				return 1;
		end
		
		-- 设置怪物技能
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 7009);
		return 1;
	end
	
	-- 血量在20%~50%之间
	-- 查看怪物是否有再生之阵标志位
	if lua_chk_player_state(batid, mark, pos, 12) == 0 then
		-- 没有的情况下100%概率使用【8001】
		lua_set_beast_atk_skill(batid, mark, pos, mark, pos, atk_seq, 8001);
		return 1;
	end
	
	--- 对任一敌方对象使用7001（35%）、7005（35%）、7009（30%）。
	local rand_num = lua_cert_rand_num(100);
	-- 在对方队伍中随机找一个可攻击的人
	local ene_pos = lua_get_player_by_rand(batid, enemy_mark);
		
	--- 对方队伍中已经没有可以攻击的对象
	if ene_pos == -1 then
		-- 认为其发招成功，因为无需在发招
		return 1;
	end
		
	if rand_num >= 0 and rand_num < 35 then
		-- 设置怪物技能
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 7001);
		return 1;
	end
	
	if rand_num >= 35 and rand_num < 70 then
		-- 设置怪物技能
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 7005);
		return 1;
	end
	
	-- 设置怪物技能
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 7009);
	return 1;
end

function lua_is_player_infangyu(batid, mark, pos, enemy_mark, enemy_pos)
  ----获取自己的速度
	local speed_self = lua_get_player_prop(batid, mark, pos, g_prop_speed);
	
	----获取对方的速度
	local speed_enemy = lua_get_player_prop(batid, enemy_mark, enemy_pos, g_prop_speed);
	
	----获取对方的第一次发招
	local atk_mark, atk_pos, atk_atk_type, atk_atk_level = lua_get_play_atk_target(batid, enemy_mark, enemy_pos, 0);
	----看对方是否使用了防御或幻盾
	if atk_atk_type == 5701 or atk_atk_type == 5703 then
		---- 对方处于防御状态
		return 1;
	end
	
	---- 己方速度比对方快，则不用考虑对方的防御状态
	if speed_self > speed_enemy then
		return 0;
	end
	
	---- 查看对方的第二次发招
	local atk_mark_1, atk_pos_1, atk_atk_type_1, atk_atk_level_1 = lua_get_play_atk_target(batid, enemy_mark, enemy_pos, 1);
	----看对方是否使用了防御或幻盾
	if atk_atk_type_1 == 5701 or atk_atk_type_1 == 5703 then
		---- 对方处于防御状态
		return 1;
	end
	
	return 0;
end

---野猪王发招-1
function lua_set_2002_atkskill_fangyu_1(batid, mark, pos, atk_seq, enemy_mark, ene_pos)
	---- 5103（40%）、5102（20%）、5106（20%）、5108（20%）。
	local skill_rate = lua_cert_rand_num(100);
	if skill_rate >= 0 and skill_rate < 40 then
		-- 设置怪物技能
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 5103);
		return 1;
	end
	
	if skill_rate >= 40 and skill_rate < 60 then
		-- 设置怪物技能
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 5102);
		return 1;
	end
	
	if skill_rate >= 60 and skill_rate < 80 then
		-- 设置怪物技能
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 5106);
		return 1;
	end
	
	-- 设置怪物技能
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 5108);
	return 1;
end

---野猪王发招-2
function lua_set_2002_atkskill_no_fangyu_1(batid, mark, pos, atk_seq, enemy_mark, ene_pos)
	---- 5010（30%）、5102（30%）、5106（20%）、5108（20%）；
	local skill_rate = lua_cert_rand_num(100);
	if skill_rate >= 0 and skill_rate < 30 then
		-- 设置怪物技能
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 5010);
		return 1;
	end
	
	if skill_rate >= 30 and skill_rate < 60 then
		-- 设置怪物技能
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 5102);
		return 1;
	end
	
	if skill_rate >= 60 and skill_rate < 80 then
		-- 设置怪物技能
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 5106);
		return 1;
	end
	
	-- 设置怪物技能
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 5108);
	return 1;
end

---野猪王发招-3
function lua_set_2002_atkskill_no_fangyu_2(batid, mark, pos, atk_seq, enemy_mark, ene_pos)
	---8701（40%）、5102（20%）、5106（20%）、5108（20%）。
	local skill_rate = lua_cert_rand_num(100);
	if skill_rate >= 0 and skill_rate < 40 then
		-- 设置怪物技能
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 8701);
		return 1;
	end
	
	if skill_rate >= 40 and skill_rate < 60 then
		-- 设置怪物技能
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 5102);
		return 1;
	end
	
	if skill_rate >= 60 and skill_rate < 80 then
		-- 设置怪物技能
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 5106);
		return 1;
	end
	
	-- 设置怪物技能
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, ene_pos, atk_seq, 5108);
	return 1;
end

---野猪王发招-4
function lua_set_2002_atkskill_fangyu_2(batid, mark, pos, atk_seq, enemy_mark, ene_pos)
	return lua_set_2002_atkskill_fangyu_1(batid, mark, pos, atk_seq, enemy_mark, ene_pos);
end

--- 野猪王ai模块
function lua_beast_attack_2002_ai(batid, mark, pos, atk_seq, touxi)
	-- 获取对方队伍标志
	local enemy_mark = lua_get_enemy_team_mark(mark);
	
	-- 获取当前血量
	local hp_cur = lua_get_player_prop(batid, mark, pos, g_prop_hp);
	-- 获取最大血量
	local hp_max = lua_get_player_prop(batid, mark, pos, g_prop_hp_max);
	
	local hp_rate = (hp_cur/hp_max) * 100;
	if hp_rate >= 30 then
		-- 找到血最多的人,近程攻击
		local ene_pos = lua_get_player_most_hp(batid, mark, pos, enemy_mark, 1);
		-- 不存在需最多的人，对方人已经全部死光
		if ene_pos == -1 then
			return 1;
		end
		
		if lua_is_player_infangyu(batid, mark, pos, enemy_mark, ene_pos) == 1 then
			return lua_set_2002_atkskill_fangyu_1(batid, mark, pos, atk_seq, enemy_mark, ene_pos);
		end
		return lua_set_2002_atkskill_no_fangyu_1(batid, mark, pos, atk_seq, enemy_mark, ene_pos);
	end
	
	-- 找到血最少的人,近程攻击
	local ene_pos = lua_get_player_least_hp(batid, mark, pos, enemy_mark, 1);
	-- 不存在需最少的人，对方人已经全部死光
	if ene_pos == -1 then
		return 1;
	end

	if lua_is_player_infangyu(batid, mark, pos, enemy_mark, ene_pos) == 1 then
		return lua_set_2002_atkskill_fangyu_2(batid, mark, pos, atk_seq, enemy_mark, ene_pos);
	end
	return lua_set_2002_atkskill_no_fangyu_2(batid, mark, pos, atk_seq, enemy_mark, ene_pos);
end

----骷髅王5个小弟
function lua_beast_attack_2004_ai_5_xiaodi(batid, mark, pos, atk_seq, touxi)
	--- 5010（20%）、5102（20%）、5106（20%）、5107（20%）、5108（20%）。
	local skill_rate = lua_cert_rand_num(100);
	local enemy_mark = lua_get_enemy_team_mark(mark);
	
	if skill_rate >= 0 and skill_rate < 20 then
		local enemy_pos = lua_get_player_by_rand_distance(batid, mark, pos, enemy_mark, 1);
		if enemy_pos == -1 then
			return 1;
		end
		
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5010);
		return 1;
	end
	
	-- zhuiji
	if skill_rate >= 20 and skill_rate < 40 then
		local enemy_pos = lua_get_player_by_rand_distance(batid, mark, pos, enemy_mark, 1);
		if enemy_pos == -1 then
			return 1;
		end
		
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5102);
		return 1;
	end
	
	-- kuangji
	if skill_rate >= 40 and skill_rate < 60 then
		local enemy_pos = lua_get_player_by_rand_distance(batid, mark, pos, enemy_mark, 1);
		if enemy_pos == -1 then
			return 1;
		end
		
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5106);
		return 1;
	end
	
	-- douqi
	if skill_rate >= 60 and skill_rate < 80 then
		local enemy_pos = lua_get_player_by_rand_distance(batid, mark, pos, enemy_mark, 1);
		if enemy_pos == -1 then
			return 1;
		end
		
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5107);
		return 1;
	end
	
	--- 随机获取一个近程内的攻击对象
	local enemy_pos = lua_get_player_by_rand_distance(batid, mark, pos, enemy_mark, 1);
	if enemy_pos == -1 then
		return 1;
	end
	
	---huishang
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5108);
	return 1;
end

--- 骷髅王，在小弟数量只有1~4的时候，攻击近身范围内的魔法(传教士)
function lua_beast_attack_2004_ai_1_4_atkjinshen(batid, mark, pos, atk_seq, enemy_mark, enemy_pos)
	--- 5010（20%）、5102（30%）、5106（25%）、5108（25%）。
	local skill_rate = lua_cert_rand_num(100);
	local enemy_mark = lua_get_enemy_team_mark(mark);
	
	--- 20%的概率使用普通攻击
	if skill_rate >= 0 and skill_rate < 20 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5010);
		return 1;
	end
	
	--- 30%的概率使用追击
	if skill_rate >= 20 and skill_rate < 50 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5102);
		return 1;
	end
	
	--- 25%的概率使用5106
	if skill_rate >= 50 and skill_rate < 75 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5106);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5108);
	return 1;
end

----骷髅王只有1~4个小弟
function lua_beast_attack_2004_ai_1_4_xiaodi(batid, mark, pos, atk_seq, touxi)
	--- 5010（20%）、5102（20%）、5106（20%）、5107（20%）、5108（20%）。
	---print("lua_beast_attack_2004_ai_1_4_xiaodi 1\n");
	local skill_rate = lua_cert_rand_num(100);
	local enemy_mark = lua_get_enemy_team_mark(mark);
	local enemy_pos = -1;
	---随机优先取一个魔法,传教士职业
	---print("lua_beast_attack_2004_ai_1_4_xiaodi 2\n");
	enemy_pos = lua_get_player_by_prof_rand(batid, enemy_mark, 3, 4);

	---print("lua_beast_attack_2004_ai_1_4_xiaodi 4\n");
	---存在需要攻击的传教士或魔法职业
	if enemy_pos ~= -1 then
		---print("lua_beast_attack_2004_ai_1_4_xiaodi 5\n");
		---查看是否是近身攻击
		local dis = lua_can_reach_distance(batid, mark, pos, enemy_mark, enemy_pos);
		-- 近身攻击
		if dis == 1 then
			---print("lua_beast_attack_2004_ai_1_4_xiaodi 6\n");
			--- 5010（20%）、5102（30%）、5106（25%）、5108（25%）。
			return lua_beast_attack_2004_ai_1_4_atkjinshen(batid, mark, pos, atk_seq, enemy_mark, enemy_pos)
		end
		
		---print("lua_beast_attack_2004_ai_1_4_xiaodi 7\n");
		---100% 使用斗气技能
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5107);
		return 1;
	end
	
	---print("lua_beast_attack_2004_ai_1_4_xiaodi 8\n");
	--- 不存在魔法或传教士职业的人要被攻击
	--- 5010（20%）、5102（20%）、5106（20%）、5107（20%）、5108（20%）。	
	--- 重新计算技能发招概率
	skill_rate = lua_cert_rand_num(100);	
	enemy_pos = lua_get_player_by_rand_distance(batid, mark, pos, enemy_mark, 1);
	if enemy_pos == -1 then
		return 1;
	end
	
	---print("lua_beast_attack_2004_ai_1_4_xiaodi 9\n");
	if skill_rate >= 0 and skill_rate < 20 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5010);
		return 1;
	end
	
	---print("lua_beast_attack_2004_ai_1_4_xiaodi 10\n");
	if skill_rate >= 20 and skill_rate < 40 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5102);
		return 1;
	end

	---print("lua_beast_attack_2004_ai_1_4_xiaodi 11\n");
	if skill_rate >= 40 and skill_rate < 60 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5106);
		return 1;
	end
	
	---print("lua_beast_attack_2004_ai_1_4_xiaodi 12\n");
	if skill_rate >= 60 and skill_rate < 80 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5107);
		return 1;
	end
	
	---print("lua_beast_attack_2004_ai_1_4_xiaodi 13\n");
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5108);
	return 1;
end

--- 5010（20%）、5102（20%）、5106（20%）、5107（20%）、5108（20%）。
function normal_attack_normal_player_infangyu(batid, mark, pos, enemy_mark, enemy_pos, atk_seq)
	--- 产生一个概率基数
	local skill_rate = lua_cert_rand_num(100);
	if skill_rate >= 0 and skill_rate < 20 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5010);
		return 1;
	end
	
	--- 使用追击
	if skill_rate >= 20 and skill_rate < 40 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5102);
		return 1;
	end
	
	if skill_rate >= 40 and skill_rate < 60 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5106);
		return 1;
	end
	
	if skill_rate >= 60 and skill_rate < 80 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5107);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5108);
	return 1;
end

--- 骷髅王，小弟已经全部死光的情况
function lua_beast_attack_2004_ai_no_xiaodi(batid, mark, pos, atk_seq, touxi)
	--- 对近身射程内未使用防御【5701】或幻盾【5703】的敌方对象中HP最少的使用5102（30%）、5107（35%）、8701（35%）。	
	local enemy_mark = lua_get_enemy_team_mark(mark);
	---print("lua_beast_attack_2004_ai_no_xiaodi -1\n");
	---找到近身攻击范围内血量最少的玩家
	local enemy_pos = lua_get_player_least_hp(batid, mark, pos, enemy_mark, 1);
	---print("lua_beast_attack_2004_ai_no_xiaodi -2\n");
	---- 场上玩家已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	---print("lua_beast_attack_2004_ai_no_xiaodi -3\n");
	--- 攻击对方处于防御状态
	if lua_is_player_infangyu(batid, mark, pos, enemy_mark, enemy_pos) == 1 then 
		--- 处于防御状态的对象攻击
		--- 5010（20%）、5102（20%）、5106（20%）、5107（20%）、5108（20%）。
		return normal_attack_normal_player_infangyu(batid, mark, pos, enemy_mark, enemy_pos, atk_seq);
	end

	---print("lua_beast_attack_2004_ai_no_xiaodi -4\n");
	--- 产生一个概率基数
	local skill_rate = lua_cert_rand_num(100);
	-- 5102（30%）、5107（35%）、8701（35%）
	if skill_rate >= 0 and skill_rate < 30 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5102);
		return 1;
	end
	
	---print("lua_beast_attack_2004_ai_no_xiaodi -5\n");
	if skill_rate >= 30 and skill_rate < 65 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5107);
		return 1;
	end
	
	---print("lua_beast_attack_2004_ai_no_xiaodi -6\n");
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 8701);
	return 1;
end

--- 骷髅王AI功能模块
function lua_beast_attack_2004_ai(batid, mark, pos, atk_seq, touxi)
	--- 获取己方人数，不考虑被打死打飞的
	local self_team_count = lua_get_player_count_alive(batid, mark);
	--- 扣除自己
	self_team_count = self_team_count - 1;
	
	--- 小弟数量超过(含)5个
	if self_team_count >= 5 then
		---print("lua_beast_attack_2004_ai-1\n");
		return lua_beast_attack_2004_ai_5_xiaodi(batid, mark, pos, atk_seq, touxi);
	end
	
	--- 小弟数量在[1, 5) 之间
	if self_team_count >= 1 and self_team_count < 5 then
		---print("lua_beast_attack_2004_ai-2\n");
		return lua_beast_attack_2004_ai_1_4_xiaodi(batid, mark, pos, atk_seq, touxi);
	end
	
	---print("lua_beast_attack_2004_ai-3\n");
	--- 小弟已经死光
	return lua_beast_attack_2004_ai_no_xiaodi(batid, mark, pos, atk_seq, touxi);
end

--- 随机获取一个非魔法职业的攻击对象
function lua_get_attack_player_except_mofa(batid, enemy_mark)
	return lua_get_player_except_prof_rand(batid, enemy_mark, 3);
end

--- 若敌方对象有多个非魔法师，按照以下规则随机攻击任一。对该对象使用6801（40%）、7010（30%）、7011（30%）。
function lua_beast_attack_3001_ai_gongji_feimo(batid, mark, pos, atk_seq, enemy_mark, enemy_pos)
	local rate_num = lua_cert_rand_num(100);
	if rate_num >= 0 and rate_num < 40 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 6801);
		return 1;
	end
	
	if rate_num >= 40 and rate_num < 70 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7010);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7011);
	return 1;
end

------------------------------------------------------------------------------------------------------------------------------
--- 若敌方对象有多个非魔法师，按照以下规则随机攻击任一。对该对象使用6801（40%）、7005（30%）、7008（30%）。
function lua_beast_attack_3001_ai_gongji_feimo_1(batid, mark, pos, atk_seq, enemy_mark, enemy_pos)
	local rate_num = lua_cert_rand_num(100);
	if rate_num >= 0 and rate_num < 40 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 6801);
		return 1;
	end
	
	if rate_num >= 40 and rate_num < 70 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7005);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7008);
	return 1;
end

------------------------------------------------------------------------------------------------------------------------------
--- 若敌方对象有多个非魔法师，按照以下规则随机攻击任一。对该对象使用6801（40%）、7001（15%）、7002（15%）、7003（15%）、7004（15%）。
function lua_beast_attack_3001_ai_gongji_feimo_2(batid, mark, pos, atk_seq, enemy_mark, enemy_pos)
	local rate_num = lua_cert_rand_num(100);
	if rate_num >= 0 and rate_num < 40 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 6801);
		return 1;
	end
	
	if rate_num >= 40 and rate_num < 55 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7001);
		return 1;
	end
	
	if rate_num >= 55 and rate_num < 70 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7002);
		return 1;
	end

	if rate_num >= 70 and rate_num < 85 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7003);
		return 1;
	end	
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7004);
	return 1;
end

--- 若敌方对象都是魔法师，对任一敌方对象使用6801（30%）、7010（35%）、7011（35%）。
function lua_beast_attack_3001_ai_gongji_mo(batid, mark, pos, atk_seq, enemy_mark, enemy_pos)
	local rate_num = lua_cert_rand_num(100);
	if rate_num >= 0 and rate_num < 30 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 6801);
		return 1;
	end
	
	if rate_num >= 30 and rate_num < 65 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7010);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7011);
	return 1;
end

------------------------------------------------------------------------------------------------
--- 若敌方对象都是魔法师，对任一敌方对象使用6801（30%）、7005（35%）、7008（35%）。
function lua_beast_attack_3001_ai_gongji_mo_1(batid, mark, pos, atk_seq, enemy_mark, enemy_pos)
	local rate_num = lua_cert_rand_num(100);
	if rate_num >= 0 and rate_num < 30 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 6801);
		return 1;
	end
	
	if rate_num >= 30 and rate_num < 65 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7005);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7008);
	return 1;
end

------------------------------------------------------------------------------------------------
--- 若敌方对象都是魔法师，对任一敌方对象使用6801（20%）、7001（20%）、7002（20%）、7003（20%）、7004（20%）。
function lua_beast_attack_3001_ai_gongji_mo_2(batid, mark, pos, atk_seq, enemy_mark, enemy_pos)
	local rate_num = lua_cert_rand_num(100);
	if rate_num >= 0 and rate_num < 20 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 6801);
		return 1;
	end
	
	if rate_num >= 20 and rate_num < 40 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7001);
		return 1;
	end
	
	if rate_num >= 40 and rate_num < 60 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7002);
		return 1;
	end
	
	if rate_num >= 60 and rate_num < 80 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7003);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7004);
	return 1;
end

--- 彼兔老师AI功能模块
function lua_beast_attack_3001_ai(batid, mark, pos, atk_seq, touxi)
	--- 获取对方队伍标志
	local enemy_mark = lua_get_enemy_team_mark(mark);
	--- 获取对方队伍人数(打飞打死不考虑)
	local enemy_team_count = lua_get_player_count_alive(batid, enemy_mark);



	-- 获取当前血量
	local hp_cur = lua_get_player_prop(batid, mark, pos, g_prop_hp);
	
	-- 获取最大血量
	local hp_max = lua_get_player_prop(batid, mark, pos, g_prop_hp_max);
	
	--- 血量比率
	local hp_rate = (hp_cur/hp_max) * 100;
	
	--- 血量比率超过30%
	if hp_rate >= 30 then
		--- 人数超过7个
		--- 对任一敌方对象使用7010（50%）、7011（50%）。
		if enemy_team_count >= 7 then
			local enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
			--- 对方已经全部死光
			if enemy_pos == -1 then
				return 1;
			end

			--- 50%的概率使用7010 或 7011
			if lua_cert_rand_num(2) == 1 then
				lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7010);
			else
				lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7011);
			end

			return 1;
		end
		
		--- 对方人数在[3, 7)个
		---- 对任一敌方对象使用7005（50%）、7008（50%）。
		if enemy_team_count >= 3 and enemy_team_count < 7 then
			local enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
			--- 对方已经全部死光
			if enemy_pos == -1 then
				return 1;
			end
			
			--- 50%的概率使用7005 或 7008
			if lua_cert_rand_num(2) == 1 then
				lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7005);
			else
				lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7008);
			end
			return 1;
		end
		
		--- 对方人数[1, 3)个
		--- 对任一敌方对象使用7001（25%）、7002（25%）、7003（25%）、7004（25%）。
		local enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
		--- 对方已经全部死光
		if enemy_pos == -1 then
			return 1;
		end

		local skill_rate = lua_cert_rand_num(100);
		if skill_rate >= 0 and skill_rate < 25 then
			lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7001);
			return 1;
		end
		
		if skill_rate >= 25 and skill_rate < 50 then
			lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7002);
			return 1;
		end
		
		if skill_rate >= 50 and skill_rate < 75 then
			lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7003);
			return 1;
		end
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7004);
		return 1;
	end --- end of 血量比率超过30%
	
	----血量少于30%
	if enemy_team_count >= 7 then
		-- 随机找一个非魔法职业的人
		local enemy_pos = lua_get_attack_player_except_mofa(batid, enemy_mark);
		if enemy_pos ~= -1 then
			---- 对该对象使用6801（40%）、7010（30%）、7011（30%）。
			return lua_beast_attack_3001_ai_gongji_feimo(batid, mark, pos, atk_seq, enemy_mark, enemy_pos);
		end
		
		-- 随机找一个人
		enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
		--- 对方已经全部死光
		if enemy_pos == -1 then
			return 1;
		end
		
		--- 全是魔法职业的人
		return lua_beast_attack_3001_ai_gongji_mo(batid, mark, pos, atk_seq, enemy_mark, enemy_pos);
	end
	
	--- 对方人数在[3, 7)
	if enemy_team_count >= 3 and enemy_team_count < 7 then
		-- 随机找一个非魔法职业的人
		local enemy_pos = lua_get_attack_player_except_mofa(batid, enemy_mark);
		if enemy_pos ~= -1 then
			return lua_beast_attack_3001_ai_gongji_feimo_1(batid, mark, pos, atk_seq, enemy_mark, enemy_pos);
		end
		
		-- 随机找一个人
		enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
		--- 对方已经全部死光
		if enemy_pos == -1 then
			return 1;
		end
		return lua_beast_attack_3001_ai_gongji_mo_1(batid, mark, pos, atk_seq, enemy_mark, enemy_pos);
	end
	
	--- 对方人数在[1, 3)之间
	-- 随机找一个非魔法职业的人
	local enemy_pos = lua_get_attack_player_except_mofa(batid, enemy_mark);
	if enemy_pos ~= -1 then
		return lua_beast_attack_3001_ai_gongji_feimo_2(batid, mark, pos, atk_seq, enemy_mark, enemy_pos);
	end
	-- 随机找一个人
	enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
	--- 对方已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	return lua_beast_attack_3001_ai_gongji_mo_2(batid, mark, pos, atk_seq, enemy_mark, enemy_pos);
end
---------------------------------------------------------------------------------------------------------
--- 杰生老师AI，当杰生老师的血量比超过30% 近身无人防御
function lua_beast_attack_3002_ai_jinshenwufanyu(batid, mark, pos, atk_seq, touxi)
	----若近身射程内没有敌方对象使用防御【5701】或幻盾【5703】，则任选其一使用5102（25%）、5106（25%）、5108（25%），或对自己使用5704（25%）。
	--- 获取对方队伍标志
	local enemy_mark = lua_get_enemy_team_mark(mark);
	local enemy_pos = lua_get_player_by_rand_distance(batid, mark, pos, enemy_mark, 1);
	--- 对方已经没有人可以攻击
	if enemy_pos == -1 then
		return 1;
	end
	
	---- 产生概率种子
	local skill_rate = lua_cert_rand_num(100);
	if skill_rate >= 0 and skill_rate < 25 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5102);
		return 1;
	end
	
	if skill_rate >= 25 and skill_rate < 50 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5106);
		return 1;
	end	
	
	if skill_rate >= 50 and skill_rate < 75 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5108);
		return 1;
	end	
	
	--- 对自己使用5704
	lua_set_beast_atk_skill(batid, mark, pos, mark, pos, atk_seq, 5704);
	return 1;
end

---------------------------------------------------------------------------------------------------------
--- 杰生老师AI，当杰生老师的血量比超过30% 近身多人防御
function lua_beast_attack_3002_ai_jinshenduorenfangyu(batid, mark, pos, enemy_mark, enemy_pos, atk_seq)
	--- 若近身射程内有多个敌方对象使用防御【5701】或幻盾【5703】，则任选其一使用5103（40%）、5102（20%）、5106（20%）、5108（20%）。
	---- 产生概率种子
	local skill_rate = lua_cert_rand_num(100);
	if skill_rate >= 0 and skill_rate < 40 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5103);
		return 1;
	end
	
	if skill_rate >= 40 and skill_rate < 60 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5102);
		return 1;
	end
	
	if skill_rate >= 60 and skill_rate < 80 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5106);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5108);
	return 1;
end

---------------------------------------------------------------------------------------------------------
--- 杰生老师AI，当杰生老师的血量比超过30% 近身1人防御
function lua_beast_attack_3002_ai_jinshenduo_onerenfangyu(batid, mark, pos, enemy_mark, enemy_pos, atk_seq)
	--- 若近身射程内只有一个敌方对象使用防御【5701】或幻盾【5703】，则对其使用5103（40%）、5102（20%）、5106（20%）、5108（20%）。
	---- 产生概率种子
	local skill_rate = lua_cert_rand_num(100);
	if skill_rate >= 0 and skill_rate < 40 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5103);
		return 1;
	end
	
	if skill_rate >= 40 and skill_rate < 60 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5102);
		return 1;
	end
	
	if skill_rate >= 60 and skill_rate < 80 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5106);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5108);
	return 1;
end

---------------------------------------------------------------------------------------------------------
--- 杰生老师AI，当杰生老师的血量比超过30%
function lua_beast_attack_3002_ai_hp_over30(batid, mark, pos, atk_seq, touxi)
	--- 获取对方队伍标志
	local enemy_mark = lua_get_enemy_team_mark(mark);
	--- 若近身射程内有多个敌方对象使用防御【5701】或幻盾【5703】，则任选其一使用5103（40%）、5102（20%）、5106（20%）、5108（20%）。
	--- 若近身射程内只有一个敌方对象使用防御【5701】或幻盾【5703】，则对其使用5103（40%）、5102（20%）、5106（20%）、5108（20%）。
	local count, enemy_pos = lua_get_player_fangyu_rand_distance(batid, mark, pos, enemy_mark, 1);
	---- 若近身射程内没有敌方对象使用防御【5701】或幻盾【5703】，则任选其一使用5102（25%）、5106（25%）、5108（25%），或对自己使用5704（25%）。
	if count <= 0 or enemy_pos == -1 then
		-- 无人防御
		return lua_beast_attack_3002_ai_jinshenwufanyu(batid, mark, pos, atk_seq, touxi);
	end
	
	if count > 1 then
		---- 近身多人防御
		return lua_beast_attack_3002_ai_jinshenduorenfangyu(batid, mark, pos, enemy_mark, enemy_pos, atk_seq);
	end
	
	---- 近身1人防御
	return lua_beast_attack_3002_ai_jinshenduo_onerenfangyu(batid, mark, pos, enemy_mark, enemy_pos, atk_seq);
end

---------------------------------------------------------------------------------------------------------
--- 杰生老师AI，当杰生老师的血量比不超过30%
function lua_beast_attack_3002_ai_hp_low30(batid, mark, pos, atk_seq, touxi)
	--- 获取对方队伍标志
	local enemy_mark = lua_get_enemy_team_mark(mark);
	--- 敌方全部对象使用使用防御【5701】或幻盾【5703】：
	--- 对自己使用6001（100%）。
	local count, enemy_pos = lua_get_player_fangyu_rand_distance(batid, mark, pos, enemy_mark, 1);
	
	--- 对方队伍中所有可以行动的人总数
	local enemy_count = lua_get_player_count_alive(batid, enemy_mark);
	
	--- 对方全部防御
	if count == enemy_count then
		---- 对自己使用6001（100%）。
		lua_set_beast_atk_skill(batid, mark, pos, mark, pos, atk_seq, 6001);
		return 1;
	end
	
	--- 对方不是全部防御
	---- 敌方有对象未使用使用防御【5701】或幻盾【5703】：
	---- 对自己使用6001（40%），对敌方任一对象使用5101（30%）、5102（30%）
	local skill_rate = lua_cert_rand_num(100);
	if skill_rate >= 0 and skill_rate < 40 then
		---- 对自己使用6001（100%）。
		lua_set_beast_atk_skill(batid, mark, pos, mark, pos, atk_seq, 6001);
		return 1;
	end
	
	enemy_pos = lua_get_player_by_rand_distance(batid, mark, pos, enemy_mark, 1);
	if enemy_pos == -1 then
		return 1;
	end

	if skill_rate >= 40 and skill_rate < 70 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5101);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5102);
	return 1;
end
---------------------------------------------------------------------------------------------------------
---杰生老师ai模块
function lua_beast_attack_3002_ai(batid, mark, pos, atk_seq, touxi)
	--- 获取对方队伍标志
	local enemy_mark = lua_get_enemy_team_mark(mark);

	-- 获取当前血量
	local hp_cur = lua_get_player_prop(batid, mark, pos, g_prop_hp);
	
	-- 获取最大血量
	local hp_max = lua_get_player_prop(batid, mark, pos, g_prop_hp_max);
	
	--- 血量比率
	local hp_rate = (hp_cur/hp_max) * 100;
	
	-- 血量超过30%
	if hp_rate >= 30 then
		return lua_beast_attack_3002_ai_hp_over30(batid, mark, pos, atk_seq, touxi);
	end
	
	-- 血量不超过30%
	return lua_beast_attack_3002_ai_hp_low30(batid, mark, pos, atk_seq, touxi);
end

---------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------
---瑞林老师ai模块
function lua_beast_attack_3003_ai_3_3_1(batid, mark, pos, enemy_mark, atk_seq)
	-- 随机找一个人
	local enemy_pos = lua_get_player_by_rand_distance(batid, mark, pos, enemy_mark, 0);
	-- 对方人已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	
	--- 产生一个随机种子
	local skill_rate = lua_cert_rand_num(100);
	---- 对任一敌方对象使用5109（60%）、5110（20%）、5111（20%）。
	if skill_rate >= 0 and skill_rate < 60 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5109);
		return 1;
	end
	
	if skill_rate >= 60 and skill_rate < 80 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5110);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5111);
	return 1;
end
---------------------------------------------------------------------------------------------------------
---瑞林老师ai模块
function lua_beast_attack_3003_ai_3_3_2(batid, mark, pos, enemy_mark, atk_seq)
	-- 随机找一个人
	local enemy_pos = lua_get_player_by_rand_distance(batid, mark, pos, enemy_mark, 0);
	-- 对方人已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	
	--- 产生一个随机种子
	local skill_rate = lua_cert_rand_num(100);
	---- 对任一敌方对象使用5110（35%）、5111（35%）、5112（30%）。
	if skill_rate >= 0 and skill_rate < 35 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5110);
		return 1;
	end
	
	if skill_rate >= 35 and skill_rate < 70 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5111);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5112);
	return 1;
end

---------------------------------------------------------------------------------------------------------
---瑞林老师ai模块
function lua_beast_attack_3003_ai_3_3_3(batid, mark, pos, enemy_mark, atk_seq)
	---- 对自己使用6001（50%），对敌方任一对象使用5109（50%）。
	--- 产生一个随机种子
	local skill_rate = lua_cert_rand_num(100);
	if skill_rate >= 0 and skill_rate < 50 then
		lua_set_beast_atk_skill(batid, mark, pos, mark, pos, atk_seq, 6001);
		return 1;
	end
	
	-- 随机找一个人
	local enemy_pos = lua_get_player_by_rand_distance(batid, mark, pos, enemy_mark, 0);
	-- 对方人已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5109);
	return 1;
end

---------------------------------------------------------------------------------------------------------
---瑞林老师ai模块
function lua_beast_attack_3003_ai_3_3_4(batid, mark, pos, enemy_mark, atk_seq)
	---- 对自己使用6001（30%），对敌方任一对象使用5110（20%）、5111（20%）、5112（30%）。
	--- 产生一个随机种子
	local skill_rate = lua_cert_rand_num(100);
	if skill_rate >= 0 and skill_rate < 30 then
		lua_set_beast_atk_skill(batid, mark, pos, mark, pos, atk_seq, 6001);
		return 1;
	end
	
	-- 随机找一个人
	local enemy_pos = lua_get_player_by_rand_distance(batid, mark, pos, enemy_mark, 0);
	-- 对方人已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	
	if skill_rate >= 30 and skill_rate < 50 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5110);
		return 1;
	end
	
	if skill_rate >= 50 and skill_rate < 70 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5111);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 5112);
	return 1;
end
---------------------------------------------------------------------------------------------------------
---瑞林老师ai模块
function lua_beast_attack_3003_ai(batid, mark, pos, atk_seq, touxi)
	--- 获取对方队伍标志
	local enemy_mark = lua_get_enemy_team_mark(mark);

	-- 获取当前血量
	local hp_cur = lua_get_player_prop(batid, mark, pos, g_prop_hp);
	
	-- 获取最大血量
	local hp_max = lua_get_player_prop(batid, mark, pos, g_prop_hp_max);
	
	--- 血量比率
	local hp_rate = (hp_cur/hp_max) * 100;
	
	-- 对方存活人数
	local enemy_count = lua_get_player_count_alive(batid, enemy_mark);
	
	-- HP≥30%，敌方对象≥3
	if hp_rate >= 30 and enemy_count >= 3 then
		return lua_beast_attack_3003_ai_3_3_1(batid, mark, pos, enemy_mark, atk_seq);
	end
	
	-- HP≥30%，敌方对象<3
	if hp_rate >= 30 and enemy_count < 3 then
		--- 对任一敌方对象使用5110（35%）、5111（35%）、5112（30%）。
		return lua_beast_attack_3003_ai_3_3_2(batid, mark, pos, enemy_mark, atk_seq);
	end
	
	-- HP＜30%，敌方对象≥3
	if hp_rate < 30 and enemy_count >= 3 then
		--- 对自己使用6001（50%），对敌方任一对象使用5109（50%）。
		return lua_beast_attack_3003_ai_3_3_3(batid, mark, pos, enemy_mark, atk_seq);
	end
	
	--- 对自己使用6001（30%），对敌方任一对象使用5110（20%）、5111（20%）、5112（30%）。
	return lua_beast_attack_3003_ai_3_3_4(batid, mark, pos, enemy_mark, atk_seq);
end

---------------------------------------------------------------------------------------------------------
---温蒂&瑞贝卡 老师ai模块
function lua_beast_attack_3004_ai_1(batid, mark, pos, enemy_mark, atk_seq, touxi)

	---- 产生一个随机概率种子
	local skill_rate = lua_cert_rand_num(100);
	---- 从敌方任选一个人
	local enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
	-- 对方人已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	
	if skill_rate >= 0 and skill_rate < 30 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7001);
		return 1;
	end
	
	if skill_rate >= 30 and skill_rate < 60 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7002);
		return 1;
	end
	
	if skill_rate >= 60 and skill_rate < 80 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7005);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7006);
	return 1;
end

---------------------------------------------------------------------------------------------------------
---温蒂&瑞贝卡 老师ai模块
function lua_beast_attack_3004_ai_2(batid, mark, pos, enemy_mark, atk_seq, touxi)
	---- 产生一个随机概率种子
	local skill_rate = lua_cert_rand_num(100);
	---- 从敌方任选一个人
	local enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
	-- 对方人已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	
	if skill_rate >= 0 and skill_rate < 50 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7001);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7002);
	return 1;
end

---------------------------------------------------------------------------------------------------------
---温蒂&瑞贝卡 老师ai模块
function lua_beast_attack_3004_ai_3(batid, mark, pos, enemy_mark, atk_seq, touxi)
	---- 产生一个随机概率种子
	local skill_rate = lua_cert_rand_num(100);
	---- 从敌方任选一个人
	local enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
	-- 对方人已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	
	if skill_rate >= 0 and skill_rate < 30 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7001);
		return 1;
	end
	
	if skill_rate >= 30 and skill_rate < 60 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7002);
		return 1;
	end
	
	if skill_rate >= 60 and skill_rate < 80 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7005);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7006);
	return 1;
end

---------------------------------------------------------------------------------------------------------
---温蒂&瑞贝卡 老师ai模块
function lua_beast_attack_3004_ai_4(batid, mark, pos, enemy_mark, atk_seq, touxi)
	---- 产生一个随机概率种子
	local skill_rate = lua_cert_rand_num(100);
	---- 从敌方任选一个人
	local enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
	-- 对方人已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
		
	if skill_rate >= 0 and skill_rate < 20 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7001);
		return 1;
	end
	
	if skill_rate >= 20 and skill_rate < 40 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7002);
		return 1;
	end
	
	if skill_rate >= 40 and skill_rate < 55 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7005);
		return 1;
	end
	
	if skill_rate >= 55 and skill_rate < 70 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7006);
		return 1;
	end
	
	--- 自救
	lua_set_beast_atk_skill(batid, mark, pos, mark, pos, atk_seq, 6002);
	return 1;
end

---------------------------------------------------------------------------------------------------------
---温蒂&瑞贝卡 老师ai模块
function lua_beast_attack_3004_ai_5(batid, mark, pos, enemy_mark, atk_seq, touxi)
	---- 产生一个随机概率种子
	local skill_rate = lua_cert_rand_num(100);
	---- 从敌方任选一个人
	local enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
	-- 对方人已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	
	if skill_rate >= 0 and skill_rate < 30 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7005);
		return 1;
	end
	
	if skill_rate >= 30 and skill_rate < 60 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7006);
		return 1;
	end
	
	if skill_rate >= 60 and skill_rate < 80 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7009);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, mark, pos, atk_seq, 7010);
	return 1;
end

---------------------------------------------------------------------------------------------------------
---温蒂&瑞贝卡 老师ai模块
function lua_beast_attack_3004_ai_6(batid, mark, pos, enemy_mark, atk_seq, touxi)
	---- 产生一个随机概率种子
	local skill_rate = lua_cert_rand_num(100);
	---- 从敌方任选一个人
	local enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
	-- 对方人已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	
	if skill_rate >= 0 and skill_rate < 50 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7005);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7006);
	return 1;
end

---------------------------------------------------------------------------------------------------------
---温蒂老师ai模块救人
function lua_beast_attack_3004_ai_jiuren(batid, mark, pos, ruibeika_pos, atk_seq, touxi)
	---- 产生一个随机概率种子
	local skill_rate = lua_cert_rand_num(100);
	if skill_rate >= 0 and skill_rate < 30 then
		lua_set_beast_atk_skill(batid, mark, pos, mark, ruibeika_pos, atk_seq, 6008);
		return 1;
	end
	
	--- 获取对方队伍标志
	local enemy_mark = lua_get_enemy_team_mark(mark);
	---- 从敌方任选一个人
	local enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
	-- 对方人已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	
	if skill_rate >= 30 and skill_rate < 50 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7001);
		return 1;
	end
	
	if skill_rate >= 50 and skill_rate < 70 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7002);
		return 1;
	end
	
	if skill_rate >= 70 and skill_rate < 85 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7005);
		return 1;
	end
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7006);
	return 1;
end

---------------------------------------------------------------------------------------------------------
---温蒂老师ai模块
function lua_beast_attack_3004_ai(batid, mark, pos, typeid, atk_seq, touxi)

	local groupid_cur = lua_get_player_prop(batid, mark, pos, g_prop_groupid);
	--3049、3059、3312
	if (not (groupid_cur==3049 or groupid_cur== 3059 or groupid_cur==3312    ))then
			return 0;
	end


	--- 获取瑞贝卡位置信息
	local ruibeika_pos = lua_get_beast_by_typeid(batid, mark, 3005);
	
	--- 检查瑞贝卡是否已死(33是"死"标志位)
	local is_ruibeika_dead = lua_chk_player_state(batid, mark, ruibeika_pos, 33);
	
	--- 接口内部有错误，可能原因是参数存在问题，
	--- 对战已经被释放，指定对象不存在等等
	if is_ruibeika_dead == -1 then
		return 1;
	end
	
	--- 检查瑞贝卡是否被打飞
	if lua_chk_player_state(batid, mark, ruibeika_pos, 34) == 1 then
		--- 已经被打飞，无法施救
		is_ruibeika_dead = 0;
	end
	
	--- 瑞贝卡已死
	if is_ruibeika_dead == 1 then
		return lua_beast_attack_3004_ai_jiuren(batid, mark, pos, ruibeika_pos, atk_seq, touxi);
	end

	
	--- 获取对方队伍标志
	local enemy_mark = lua_get_enemy_team_mark(mark);

	-- 获取温蒂当前血量
	local hp_cur = lua_get_player_prop(batid, mark, pos, g_prop_hp);
	-- 获取温蒂最大血量
	local hp_max = lua_get_player_prop(batid, mark, pos, g_prop_hp_max);
	--- 温蒂血量比率
	local wendi_hp_rate = (hp_cur/hp_max) * 100;
	
	--- 获取瑞贝卡当前血量
	hp_cur = lua_get_player_prop(batid, mark, ruibeika_pos, g_prop_hp);
	
	--- 获取瑞贝卡最大血量
	hp_max = lua_get_player_prop(batid, mark, ruibeika_pos, g_prop_hp_max);
	
	--- 温蒂血量比率
	local ruibeka_hp_rate = (hp_cur/hp_max) * 100;
	
	--- 瑞贝卡需要加血
	if wendi_hp_rate >= 30 and ruibeka_hp_rate < 30 then
		lua_set_beast_atk_skill(batid, mark, pos, mark, ruibeika_pos, atk_seq, 6002);
		return 1;
	end
	
	--- 获取敌方人数
	local enemy_count = lua_get_player_count_alive(batid, enemy_mark);
	--- 
	if wendi_hp_rate >= 30 and ruibeka_hp_rate >= 30 and enemy_count >= 7 then
		return lua_beast_attack_3004_ai_1(batid, mark, pos, enemy_mark, atk_seq, touxi);
	end
	
	if wendi_hp_rate >= 30 and ruibeka_hp_rate >= 30 then
		return lua_beast_attack_3004_ai_2(batid, mark, pos, enemy_mark, atk_seq, touxi);
	end
	
	if wendi_hp_rate < 30 and ruibeka_hp_rate < 30 and enemy_count >= 7 then
		return lua_beast_attack_3004_ai_3(batid, mark, pos, enemy_mark, atk_seq, touxi);
	end
	
	if wendi_hp_rate < 30 and ruibeka_hp_rate < 30 then
		return lua_beast_attack_3004_ai_4(batid, mark, pos, enemy_mark, atk_seq, touxi);
	end
	
	if wendi_hp_rate < 30 and ruibeka_hp_rate >= 30 and enemy_count >= 7 then
		return lua_beast_attack_3004_ai_5(batid, mark, pos, enemy_mark, atk_seq, touxi);
	end
	
	return lua_beast_attack_3004_ai_6(batid, mark, pos, enemy_mark, atk_seq, touxi);
end

---------------------------------------------------------------------------------------------------------
---瑞贝卡老师ai模块救人
function lua_beast_attack_3005_ai_jiuren(batid, mark, pos, wendi_pos, atk_seq, touxi)
	---- 产生一个随机概率种子
	local skill_rate = lua_cert_rand_num(100);
	---- 瑞贝卡对温蒂使用6008（30%），对任一敌方对象使用7003（20%）、7004（20%）、7007（15%）、7008（15%）。
	if skill_rate >= 0 and skill_rate < 30 then
		lua_set_beast_atk_skill(batid, mark, pos, mark, wendi_pos, atk_seq, 6008);
		return 1;
	end
	
	--- 获取对方队伍标志
	local enemy_mark = lua_get_enemy_team_mark(mark);
	---- 从敌方任选一个人
	local enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
	-- 对方人已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	
	if skill_rate >= 30 and skill_rate < 50 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7003);
		return 1;
	end
	
	if skill_rate >= 50 and skill_rate < 70 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7004);
		return 1;
	end	
	
	if skill_rate >= 70 and skill_rate < 85 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7007);
		return 1;
	end	
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7008);
	return 1;
end

---------------------------------------------------------------------------------------------------------
---瑞贝卡 老师ai模块
function lua_beast_attack_3005_ai_1(batid, mark, pos, enemy_mark, atk_seq, touxi)
	--- 瑞贝卡对任一敌方对象使用7003（30%）、7004（30%）、7007（20%）、7008（20%）。
	---- 产生一个随机概率种子
	local skill_rate = lua_cert_rand_num(100);
	---- 从敌方任选一个人
	local enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
	-- 对方人已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	
	if skill_rate >= 0 and skill_rate < 30 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7003);
		return 1;
	end
	
	if skill_rate >= 30 and skill_rate < 60 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7004);
		return 1;
	end
	
	if skill_rate >= 60 and skill_rate < 80 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7007);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7008);
	return 1;
end

---------------------------------------------------------------------------------------------------------
---温蒂&瑞贝卡 老师ai模块
function lua_beast_attack_3005_ai_2(batid, mark, pos, enemy_mark, atk_seq, touxi)
	--- 瑞贝卡对任一敌方对象使用7003（50%）、7004（50%）。
	---- 产生一个随机概率种子
	local skill_rate = lua_cert_rand_num(100);
	---- 从敌方任选一个人
	local enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
	-- 对方人已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	
	if skill_rate >= 0 and skill_rate < 50 then 
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7003);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7004);
	return 1;
end

---------------------------------------------------------------------------------------------------------
---温蒂&瑞贝卡 老师ai模块
function lua_beast_attack_3005_ai_3(batid, mark, pos, enemy_mark, atk_seq, touxi)
	--- 瑞贝卡对任一敌方对象使用7003（30%）、7004（30%）、7007（20%）、7008（20%）。
	---- 产生一个随机概率种子
	local skill_rate = lua_cert_rand_num(100);
	---- 从敌方任选一个人
	local enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
	-- 对方人已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	
	if skill_rate >= 0 and skill_rate < 30 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7003);
		return 1;
	end
	
	if skill_rate >= 30 and skill_rate < 60 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7004);
		return 1;
	end

	if skill_rate >= 60 and skill_rate < 80 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7007);
		return 1;
	end	
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7008);
	return 1;
end

---------------------------------------------------------------------------------------------------------
---温蒂&瑞贝卡 老师ai模块
function lua_beast_attack_3005_ai_4(batid, mark, pos, enemy_mark, atk_seq, touxi)
	--- 瑞贝卡对任一敌方对象使用7003（20%）、7004（20%）、7007（15%）、7008（15%），对自己使用6002（30%）。
	---- 产生一个随机概率种子
	local skill_rate = lua_cert_rand_num(100);
	---- 从敌方任选一个人
	local enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
	-- 对方人已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	
	if skill_rate >= 0 and skill_rate < 20 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7003);
		return 1;
	end
	
	if skill_rate >= 20 and skill_rate < 40 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7004);
		return 1;
	end

	if skill_rate >= 40 and skill_rate < 55 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7007);
		return 1;
	end	
	
	if skill_rate >= 55 and skill_rate < 70 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7008);
		return 1;
	end	
	
	lua_set_beast_atk_skill(batid, mark, pos, mark, pos, atk_seq, 6002);
	return 1;
end

---------------------------------------------------------------------------------------------------------
---瑞贝卡 老师ai模块
function lua_beast_attack_3005_ai_5(batid, mark, pos, enemy_mark, atk_seq, touxi)
	---- 瑞贝卡对任一敌方对象使用7007（30%）、7008（30%）、70011（20%）、7012（20%）。
	---- 产生一个随机概率种子
	local skill_rate = lua_cert_rand_num(100);
	---- 从敌方任选一个人
	local enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
	-- 对方人已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	
	if skill_rate >= 0 and skill_rate < 30 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7007);
		return 1;
	end
	
	if skill_rate >= 30 and skill_rate < 60 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7008);
		return 1;
	end

	if skill_rate >= 60 and skill_rate < 80 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7011);
		return 1;
	end	

	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7012);
	return 1;
end

---------------------------------------------------------------------------------------------------------
---瑞贝卡 老师ai模块
function lua_beast_attack_3005_ai_6(batid, mark, pos, enemy_mark, atk_seq, touxi)
	---- 瑞贝卡对任一敌方对象使用7007（50%）、7008（50%）。
	---- 产生一个随机概率种子
	local skill_rate = lua_cert_rand_num(100);
	---- 从敌方任选一个人
	local enemy_pos = lua_get_player_by_rand(batid, enemy_mark);
	-- 对方人已经全部死光
	if enemy_pos == -1 then
		return 1;
	end
	
	if skill_rate >= 0 and skill_rate < 50 then
		lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7007);
		return 1;
	end
	
	lua_set_beast_atk_skill(batid, mark, pos, enemy_mark, enemy_pos, atk_seq, 7008);
	return 1;
end

---------------------------------------------------------------------------------------------------------
---瑞贝卡 老师ai模块
function lua_beast_attack_3005_ai(batid, mark, pos, typeid, atk_seq, touxi)
	local groupid_cur = lua_get_player_prop(batid, mark, pos, g_prop_groupid);
	--3049、3059、3312,才处理
	if (not (groupid_cur==3049 or groupid_cur== 3059 or groupid_cur==3312    ))then
			return 0;
	end


	--- 获取温蒂位置信息
	local wendi_pos = lua_get_beast_by_typeid(batid, mark, 3004);
	
	--- 检查瑞贝卡是否已死(33是"死"标志位)
	local is_wendi_dead = lua_chk_player_state(batid, mark, wendi_pos, 33);
	
	--- 接口内部有错误，可能原因是参数存在问题，
	--- 对战已经被释放，指定对象不存在等等
	if is_wendi_dead == -1 then
		return 1;
	end
	
	--- 检查温蒂是否被打飞
	if lua_chk_player_state(batid, mark, wendi_pos, 34) == 1 then
		--- 已经被打飞，无法施救
		is_wendi_dead = 0;
	end
	
	--- 温蒂已死
	if is_wendi_dead == 1 then
		return lua_beast_attack_3005_ai_jiuren(batid, mark, pos, wendi_pos, atk_seq, touxi);
	end
	
	--- 获取对方队伍标志
	local enemy_mark = lua_get_enemy_team_mark(mark);

	-- 获取瑞贝卡当前血量
	local hp_cur = lua_get_player_prop(batid, mark, pos, g_prop_hp);
	-- 获取瑞贝卡最大血量
	local hp_max = lua_get_player_prop(batid, mark, pos, g_prop_hp_max);
	--- 瑞贝卡血量比率
	local ruibeka_hp_rate = (hp_cur/hp_max) * 100;
	
	--- 获取温蒂当前血量
	hp_cur = lua_get_player_prop(batid, mark, wendi_pos, g_prop_hp);
	--- 获取温蒂最大血量
	hp_max = lua_get_player_prop(batid, mark, wendi_pos, g_prop_hp_max);
	--- 温蒂血量比率
	local wendi_hp_rate = (hp_cur/hp_max) * 100;
	
	--- 温蒂需要加血
	if ruibeka_hp_rate >= 30 and wendi_hp_rate < 30 then
		lua_set_beast_atk_skill(batid, mark, pos, mark, wendi_pos, atk_seq, 6002);
		return 1;
	end
	
	--- 获取敌方人数
	local enemy_count = lua_get_player_count_alive(batid, enemy_mark);
	--- 
	if ruibeka_hp_rate >= 30 and wendi_hp_rate >= 30 and enemy_count >= 7 then
		return lua_beast_attack_3005_ai_1(batid, mark, pos, enemy_mark, atk_seq, touxi);
	end
	
	if ruibeka_hp_rate >= 30 and wendi_hp_rate >= 30 then
		return lua_beast_attack_3005_ai_2(batid, mark, pos, enemy_mark, atk_seq, touxi);
	end
	
	if ruibeka_hp_rate < 30 and wendi_hp_rate < 30 and enemy_count >= 7 then
		return lua_beast_attack_3005_ai_3(batid, mark, pos, enemy_mark, atk_seq, touxi);
	end
	
	if ruibeka_hp_rate < 30 and wendi_hp_rate < 30 then
		return lua_beast_attack_3005_ai_4(batid, mark, pos, enemy_mark, atk_seq, touxi);
	end
	
	if ruibeka_hp_rate < 30 and wendi_hp_rate >= 30 and enemy_count >= 7 then
		return lua_beast_attack_3005_ai_5(batid, mark, pos, enemy_mark, atk_seq, touxi);
	end
	
	return lua_beast_attack_3005_ai_6(batid, mark, pos, enemy_mark, atk_seq, touxi);
end
--
---------------------------------------------------------------------------------------------------------
--摩尔勇士ai(lua)接口说明
--后台batserv仅调用一下两个接口来进行怪物发招
--函数: lua_beast_need_2_attack
--参数: typeid
--说明: 根据typeid 判断怪物是否需要两次发招
--返回: 1需要两次发招，0 不需要，则后台默认怪物仅需要发招1次
---------------------------------------------------------------------------------------------------------
--函数: lua_beast_attack_ai
--参数: batid 对战对象id，回调的时候通过该id寻找对战对象，字符传类型(后台为64bit 整形)
--参数: mark 队伍标志(挑战方/接受方)
--参数: pos 怪物在队伍中的位置索引，不采用id的原因是，采用索引不需要进行查询，从而避免查询定位带来的效率损失
--      缺点就是理解起来有点不方便。但是总归来说是利大于弊
--参数: typeid 怪物类型
--参数: atk_seq 发招次序
--参数: touxi 怪物偷袭人
--返回: 1表示lua_beast_attack_ai已经设置发招，否则后台会自动给怪物随机一个发招技能
--注意:
--ai书写者，不要在当前接口中一次性设置两次发招，
--如果有怪物需要发两次招请在 lua_beast_need_2_attack 接口中设定
--如果lua_beast_need_2_attack 返回1表示接口需要发送两次招,
--后台会调用lua_beast_attack_ai接口连续两次，参数atk_seq 分别为0，1
--如果lua_beast_need_2_attack返回0，则系统默认怪物需要发招一次，也仅调用lua_beast_attack_ai接口一次，
--参数atk_seq为零。
--再就是lua_beast_attack_ai接口仅仅是用于设置怪物发招，并不产生实际的攻击效果(例如攻击速度，可能先设置发招后出招)。
---------------------------------------------------------------------------------------------------------
---
---
---
---
---
---
---
---
---
------------------对战服务器将调用lua_beast_attack_ai通过lua脚本来设置怪物技能
------参数说明
------batid 对战模型id
------mark 对战模型中队伍标志，具体定义参考上面的文档
------pos 当前怪物在对战场景中己方队伍的位置
------typeid 怪物类型
------atk_seq 第几次发招 合法的值仅0/1
------------------------------------------------------------------------------------------------------------
------touxi 是否是怪物偷袭人 1 :是
------至于人偷袭怪，则对战服务器就不会调用到该接口，lua这一层接口不用考虑
function lua_beast_attack_ai (batid, mark, pos, typeid, atk_seq, touxi)
	-- 土豆泥王ai模块
	if typeid == 2001 then
		return lua_beast_attack_2001_ai(batid, mark, pos, atk_seq, touxi);
	end

	-- 野猪王ai模块
	if typeid == 2002 then
		return lua_beast_attack_2002_ai(batid, mark, pos, atk_seq, touxi);
	end	

	-- 骷髅王ai模块
	if typeid == 2004 then
		return lua_beast_attack_2004_ai(batid, mark, pos, atk_seq, touxi);
	end	
	
	-- 彼兔老师ai模块
	if typeid == 3001 then
		return lua_beast_attack_3001_ai(batid, mark, pos, atk_seq, touxi);
	end

	-- 杰生老师AI模块
	if typeid == 3002 then
		return lua_beast_attack_3002_ai(batid, mark, pos, atk_seq, touxi);
	end
	
	--- 瑞玲老师AI模块
	if typeid == 3003 then
		return lua_beast_attack_3003_ai(batid, mark, pos, atk_seq, touxi);
	end
	
	--- 温蒂&瑞贝卡AI模块-1
	if typeid == 3004 then
		return lua_beast_attack_3004_ai(batid, mark, pos, typeid, atk_seq, touxi);
	end
	
	--- 温蒂&瑞贝卡AI模块-2
	if typeid == 3005 then
		return lua_beast_attack_3005_ai(batid, mark, pos, typeid, atk_seq, touxi);
	end
	
	---- ai模块没有对怪物做任何发招处理
	---- 后台对战服务器会给怪物一个随机技能
	---- 所以这个地方返回0
	---- 如果不需要后台服务器给怪物随机技能，这里返回1 便可。
	return 0;
end

-------------------对战服务器通过调用该接口来判断，是否需要发招2次
function lua_beast_need_2_attack (typeid)
	return 0;
end
