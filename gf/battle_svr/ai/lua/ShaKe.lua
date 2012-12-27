function mon_wait(mon)
	if (mon.i_ai.flag == 0) then
	  mon.i_ai:set_event_tm(5000,1)
		mon.i_ai.flag = 1
--		print("moves very carefully!\n")
		return true
	end
	if (mon.i_ai:check_event_tm(1) == 1) then
		mon.i_ai.flag1 = mon.i_ai.flag1 + 1
		if (mon.i_ai.flag1 % 2 == 0) then
			mon.i_ai.ready_skill_id = 4120037
--			print("select skill 4120037!\n")
		else
			mon.i_ai.ready_skill_id = 4120038
--			print("select skill 4120038!\n")
		end
		mon.i_ai:set_event_tm(10000,1)
		return true
	end
	return false
end

function mon_attack(mon)
	return false
end

function mon_stuck(mon, mon_ai)
	if (mon.super_armor == true) then
		mon.super_armor = false
		super_armor_skill = mon:select_super_armor_skill()
		if (super_armor_skill ~= 0) then
			mon_ai:monster_attack(mon, super_armor_skill, 0, 0)
--			print("select skill super_armor_skill!\n")
			mon.i_ai:change_state(1)
			return true
		end
	end
	return false
end

register_monster_ai_func(13017, monster_wait_func, "mon_wait")
register_monster_ai_func(13017, monster_attack_func, "mon_attack")
register_monster_ai_func(13017, monster_stuck_func, "mon_stuck")

--register_monster_ai_func(13017, 0, "mon_wait")
--register_monster_ai_func(13017, 4, "mon_attack")
--register_monster_ai_func(13017, 5, "mon_stuck")
