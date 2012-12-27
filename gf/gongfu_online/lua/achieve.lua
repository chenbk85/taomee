--PVE关卡
east_stages = {1, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35}

south_stages = {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50}

sand_stages = {51, 52, 53, 54}

north_stages = {55, 56}

--史诗关卡
epic_stages = {13, 21, 41, 49, 50, 51, 52, 53, 54, 55, 56}

function is_east_stage(stageid)
	for i = 1, table.getn(east_stages) do
		if east_stages[i] == stageid then
			return true
		end
	end
	return false
end

function is_north_stage(stageid)
	for i = 1, table.getn(north_stages) do
		if stageid == north_stages[i] then
			return true
		end
	end
	return false
end

function is_sand_stage(stageid)
	for i = 1, table.getn(sand_stages) do
		if stageid == sand_stages[i] then
			return true
		end
	end
	return false
end

function is_south_stage(stageid)
	for i = 1, table.getn(south_stages) do
		if stageid == south_stages[i] then
			return true
		end
	end
	return false
end

function player_passed_all_east_stage(p)
	for i = 1, table.getn(east_stages) do
		if is_player_pass_pve_stage(p, east_stages[i], 1) == false then
			return false
		end
	end
	return true
end

function player_passed_all_north_stage(p)
--	for i = 1, table.getn(north_stages) do
--		if is_player_pass_pve_stage(p, north_stages[i], 1) == false then
--			return false
--		end
--	end
	return false
end

function player_passed_all_south_stage(p)
	for i = 1, table.getn(south_stages) do
		if is_player_pass_pve_stage(p, south_stages[i], 1) == false then
			return false
		end
	end
	return true
end

function player_passed_all_sand_stage(p)
	--for i = 1, table.getn(sand_stages) do
	--	if  is_player_pass_pve_stage(p, sand_stages[i], 1) == false then
	--		return false
	--	end
	--end
	return false
end

function player_finish_all_epic_stage(p)
	for i = 1, table.getn(epic_stages) do
		if is_player_pass_pve_stage(p, epic_stages[i], 6) == false then
			return false
		end
	end
	return true
end

function handle_pve_over_event(p, event)
	stageid = event_flag(event, 1)
	btl_type = event_flag(event, 2)
	score   = event_flag(event, 3)
	diff = event_flag(event, 4)

	-- 东部主宰
	if is_east_stage(stageid) == true and player_passed_all_east_stage(p) == true then
		player_gain_achieve(p, 1)
	end

	--冰之原主宰
	if is_north_stage(stageid) == true and player_passed_all_north_stage(p) == true then
		player_gain_achieve(p, 4)
	end
	
	--沙之州主宰
	if is_sand_stage(stageid) == true and player_passed_all_sand_stage(p) == true then
		player_gain_achieve(p, 3)
	end

	--泽之国主宰
	if is_south_stage(stageid) == true and player_passed_all_south_stage(p) == true then
		player_gain_achieve(p, 2)
	end

--pve组队
	local multi_player_btl = 1
	if btl_type == multi_player_btl then
	--金兰之义
		player_gain_achieve(p, 9)	

		multi_pve_stat_id = 2
		add_player_stat(p, multi_pve_stat_id, 1)
		if get_player_stat(p, multi_pve_stat_id) >= 100 then
			--莫逆之交
			player_gain_achieve(p, 13)
		end
	end
	
   local pve_score_s = 1
   -- pve关卡得分相关成就检测
   if score == pve_score_s then
	   --巅峰绝杀
	   player_gain_achieve(p, 8)
   end

   local pve_epic_diff = 6
   if diff == pve_epic_diff  and player_finish_all_epic_stage(p) == true then
	   --披荆斩棘
	   player_gain_achieve(p, 5)
   end

	if  player_clothes_cnt(p) == 0  then
		--破釜沉舟
		player_gain_achieve(p, 14)
	end


end

function handle_kill_monster_event(p, event) 
	local cur_cnt = event_flag(event, 3)
	local player_kill_stat = get_player_stat(p, 1) + cur_cnt
	--凛然正气
	if player_kill_stat >= 10000 then
		player_gain_achieve(p, 12)
	end
end

function handle_task_finish_event(p, event)
	task_type = event_flag(event, 1)
	task_id = event_flag(event, 2)

	if task_id == 89 then 
		--百尺杆头完成任务时检查
		player_gain_achieve(p, 7)
		return
	end

	if task_id == 1766 then
		player_gain_achieve(p, 213)
		return
	end


	--百尺杆头 登陆时检查
	if player_has_finish_task(p, 89) == true then
		player_gain_achieve(p, 7)
	end

	if player_has_finish_task(p, 1766) == true then
		player_gain_achieve(p, 213)
	end

end


function handle_use_item_event(p, event)
	item_id = event_flag(event, 1)
	cnt = event_flag(event, 2)
	channel = event_flag(event, 3)
	if item_id == 1302000 or item_id == 1302001 then
		add_player_stat(p, 3, cnt)
		if get_player_stat(p, 3) >= 99 then
			--九死一生
			player_gain_achieve(p, 201)
		end
	end
	
	local summon_skill_scorll = 4
	--一技之长
	if channel == summon_skill_scorll then
		player_gain_achieve(p, 212)
	end

	local normal_use_item = 5
	if item_id == 1300811 and channel == normal_use_item then
		--大粽子
		player_gain_achieve(p, 214)
	end

end

function handle_gain_item_event(p, event)
	item_id = event_flag(event, 1)
	item_cnt = event_flag(event, 2)
	channel = event_flag(event, 3)
	swap_id = event_flag(event, 4)

	local from_vip_shop = 1
	local from_swap = 2
	--以物换物
	if swap_id == 1312 and channel == from_swap then  
--	print "check achieve for swap"
		player_gain_achieve(p, 202)
	end

	if swap_id == 1219 and channel == from_swap then
--		print "master check"
		add_player_stat(p, 4, 1)
		if get_player_stat(p, 4) then
			--春风化雨
			player_gain_achieve(p, 203)
		end
	end
	
	if channel == from_vip_shop then
--		print "vip shop item achieve"
		add_player_stat(p, 5, item_cnt)
		if get_player_stat(p, 5) >= 100 then
			--投桃报李
			player_gain_achieve(p, 207)
		end
	end

end


function handle_clothes_info_event(p, event)
	local epic_quality = 5
	if player_clothes_quality(p) >= epic_quality and player_clothes_cnt(p) == 10 then
		--全副武装
		player_gain_achieve(p, 205)
	end
end

function handle_pvp_win_event(p, event)
	pvp_win_stat = 6
	add_player_stat(p, pvp_win_stat, 1)

	--百战不殆
	if get_player_stat(p, pvp_win_stat) >= 100 then
		player_gain_achieve(p, 108)
	end

	pvp_lv = event_flag(event, 1)

	local pvp_2v2_lv = 22
	local pvp_2v2_win_stat = 7
	if pvp_lv == pvp_2v2_lv then
		--团结之力
		player_gain_achieve(p, 106)
		add_player_stat(p, pvp_2v2_win_stat, 1)
		--其力断金
		if get_player_stat(p, pvp_2v2_win_stat) >= 10 then
			player_gain_achieve(p, 109)
		end
	end

	local pvp_contest_final = 8
	if pvp_lv == pvp_contest_final then
		--小试牛刀
		player_gain_achieve(p, 103)
	end

	local pvp_team_score = 13 
	if pvp_lv == pvp_team_score  then
		--胜利之师
		player_gain_achieve(p, 104)
	end

	local pvp_fight_lv = 2
	local pvp_summon_2 = 16
	if pvp_lv == pvp_fight_lv or pvp_lv == pvp_summon_2 then
		--暂露头角
		player_gain_achieve(p, 105);
	end

end


function handle_ranker_range_event(p, event)
	ranker_type = event_flag(event, 1)
	self_rank = event_flag(event, 3)
	

	local ranker_hunter_top = 1
	--伏魔圣手
	if ranker_type == ranker_hunter_top and self_rank == 0 then
		player_gain_achieve(p, 6)
	end
	
	local ranker_contest_final = 6
	--独步武林
	if ranker_type == ranker_contest_final and self_rank == 0 then
		player_gain_achieve(p, 101)
	end

	--一战成名
	if ranker_type == ranker_contest_final and self_rank < 100 then
		player_gain_achieve(p, 102)
	end

end


function handle_team_info_event(p, event)
	--闻达天下
	if player_team_lv(p) >= 7 then
		player_gain_achieve(p, 209)
	end
	--一掷千金
	if player_team_t_coin(p) >= 1000000 then
		player_gain_achieve(p, 210)
	end
	--群策群力
	if player_team_t_exp(p) >= 10000 then
	   player_gain_achieve(p, 211)
	end
end


function handle_summon_info_event(p, event)
	--家大业大
	if player_summon_cnt(p) >= 10 then
		player_gain_achieve(p, 204)
	end

	--训练有方
	if player_super_summon_cnt(p) >= 3 then
		player_gain_achieve(p, 208)
	end
end


function handle_gain_clothes_event(p, event)
	clothes_id = event_flag(event, 1);
	lv = event_flag(event, 2)
	channel = event_flag(event, 3)

	local equip_weapon = 4
	local strengthen_update = 3
	if clothes_equippart(clothes_id) == equip_weapon and lv == 12 and channel == strengthen_update then
		--绝世利器
		player_gain_achieve(p, 206)
	end 

end

event_pass_stage = 1
event_task_finish = 2
event_ranker_range = 3
event_kill_monster = 4
event_pvp_win  = 5
event_use_item = 6
event_gain_item = 7
event_gain_clothes = 8
event_summon_info = 9
event_team_info = 10
event_clothes_info = 11

function handle_achieve_event(p, event)
	if event_type(event) == event_pass_stage then
		handle_pve_over_event(p, event)
	end

	if event_type(event) == event_kill_monster then
		handle_kill_monster_event(p, event)
	end

	if event_type(event) == event_task_finish then
		handle_task_finish_event(p, event)
	end

	if event_type(event) == event_pvp_win then
		handle_pvp_win_event(p, event)
	end

	if event_type(event) == event_use_item then
		handle_use_item_event(p, event)
	end

	if event_type(event) == event_ranker_range then
		handle_ranker_range_event(p, event)
	end

	if event_type(event) == event_gain_item then
		handle_gain_item_event(p, event);
	end

	if event_type(event) == event_team_info then
		handle_team_info_event(p, event)
	end

	if event_type(event) == event_summon_info then
		handle_summon_info_event(p, event)
	end

	if event_type(event) == event_clothes_info then
		handle_clothes_info_event(p, event)
	end

	if event_type(event) == event_gain_clothes then
		handle_gain_clothes_event(p, event);
	end

end



