#include <async_serv/async_serv.h>

#include "mc_proto.h"
#include "pet.h"
#include "items.h"
#include "mail.h"
#include "util.h"
#include "task_new.h"
#include "skill.h"
#include "battle.h"
#include "picsrv.h"
#include "npc.h"
#include "beast.h"
#include "honor.h"
#include "activity.h"
#include "mall.h"

int handle_mc_return(reload_proto_t* pkg, int len)
{
	KDEBUG_LOG(0, "MC RETURN\t[%u %u]", pkg->online_id, pkg->type);
	if (!pkg->online_id || pkg->online_id == get_server_id()) {
		switch (pkg->type) {
		case reload_type_item:
			load_xmlconf("./conf/items.xml", load_items);
            load_xmlconf("./conf/handbook.xml", load_handbook);
			load_xmlconf("./conf/npcShop.xml", load_shop_item);
			load_xmlconf("./conf/tasks_new.xml", load_tasks);
			load_xmlconf("./conf/tasks_new.xml", load_task_loops);
			load_xmlconf("./conf/fishGame.xml", load_fish_info);
			KDEBUG_LOG(0, "RELOAD CONF items.xml SUCC");
			break;
		case reload_type_cloth:
			load_xmlconf("./conf/clothes.xml", load_clothes);
			load_xmlconf("./conf/suits.xml", load_suit);
            load_xmlconf("./conf/handbook.xml", load_handbook);
			load_xmlconf("./conf/npcShop.xml", load_shop_item);
			KDEBUG_LOG(0, "RELOAD CONF clothes.xml SUCC");
			break;
		case reload_type_beast:
			load_xmlconf("./conf/beasts.xml", load_beasts);
			load_xmlconf("./conf/pet_exchange.xml", load_pet_exchange);
			load_xmlconf("./conf/pet_exchange_egg.xml", load_pet_exchange_egg);
			load_xmlconf("./conf/gplan.xml", load_rare_beasts);
			KDEBUG_LOG(0, "RELOAD CONF beasts.xml SUCC");
			break;
		case reload_beast_grp:
			load_xmlconf("./conf/beastgrp.xml", load_beast_grp);
			load_xmlconf("./conf/fishGame.xml", load_fish_info);
			KDEBUG_LOG(0, "RELOAD CONF beastgrp.xml SUCC");
			break;
		case reload_type_rand:
			load_xmlconf("./conf/rand_item.xml", load_rand_item);
			KDEBUG_LOG(0, "RELOAD CONF rand_item.xml SUCC");
			break;
		case reload_type_mail:
			load_xmlconf("./conf/mail.xml", load_sys_mail);
			KDEBUG_LOG(0, "RELOAD CONF mail.xml SUCC");
			break;
		case reload_type_task:
			load_xmlconf("./conf/tasks_new.xml", load_tasks);
			load_xmlconf("./conf/tasks_new.xml", load_task_loops);
			KDEBUG_LOG(0, "RELOAD CONF tasks_new.xml SUCC");
			break;
		case reload_type_map:
			load_xmlconf("./conf/maps.xml", reload_maps);
			KDEBUG_LOG(0, "RELOAD CONF maps.xml SUCC");
			break;
		case reload_skills:
			load_xmlconf("./conf/skills_price.xml", load_all_skills);
			load_xmlconf("./conf/npcSkills.xml", load_shop_skill);
			KDEBUG_LOG(0, "RELOAD CONF skill_price.xml SUCC");
			break;
		case reload_shop_item:
			load_xmlconf("./conf/npcShop.xml", load_shop_item);
			KDEBUG_LOG(0, "RELOAD CONF npcShop.xml SUCC");
			break;
		case reload_shop_skill:
			load_xmlconf("./conf/npcSkills.xml", load_shop_skill);
			KDEBUG_LOG(0, "RELOAD CONF npcSkill.xml SUCC");
			break;
		case reload_holiday:
			load_xmlconf("./conf/holiday.xml", load_holiday_factor);
			KDEBUG_LOG(0, "RELOAD CONF holiday.xml SUCC");
			break;
		case reload_picsrv:
			load_xmlconf("./conf/picsrv.xml", load_picsrv_config);
			KDEBUG_LOG(0, "RELOAD CONF picsrv.xml SUCC");
			break;
		case reload_suit:
			load_xmlconf("./conf/suits.xml", load_suit);
			KDEBUG_LOG(0, "RELOAD CONF suits.xml SUCC");
			break;
		case reload_title:
			load_xmlconf("./conf/titles.xml", load_honor_titles);
			KDEBUG_LOG(0, "RELOAD CONF titles.xml SUCC");
			break;
		case reload_sysinfo:
			load_xmlconf("./conf/sysinfo.xml", load_sys_info);
			KDEBUG_LOG(0, "RELOAD CONF sysinfo.xml SUCC");
			break;
		case reload_init_prof:
			load_xmlconf("./conf/professtion.xml", load_init_prof_info);
			KDEBUG_LOG(0, "RELOAD CONF professtion.xml SUCC");
			break;
		case reload_exchanges:
			load_xmlconf("./conf/exchanges.xml", load_exchange_info);
			KDEBUG_LOG(0, "RELOAD CONF exchanges.xml SUCC");
			break;
		case reload_commodity:
			load_xmlconf("./conf/commodity.xml", load_products);
			KDEBUG_LOG(0, "RELOAD CONF commodity.xml SUCC");
			break;
		}	
	}
	return 0;
}


