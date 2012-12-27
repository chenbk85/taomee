#include <cstdio>
#include <list>
using namespace std;
#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include <libtaomee++/random/random.hpp>
#include "stage.hpp"

using namespace taomee;

extern "C" 
{
#include <glib.h>
#include <assert.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>
#include <libtaomee/bitmanip/bitmanip.h>
}

#include "battle_impl.hpp"
#include "battle_logic.hpp"
#include "player.hpp"
#include "player_status.hpp"
#include "./ai/MonsterAI.hpp"


SpecialExpDoubleTimeMrg g_special_double_time_mrg;


typedef struct call_monsters_data
{
	call_monsters_data()
	{
		memset(this, 0, sizeof(call_monsters_data));
	}	
	uint32_t monster_id[3];
	uint32_t x[3];
	uint32_t y[3];
}call_monsters_data;


call_monsters_data datas[200];



void init_call_monsters_order_data()
{
	//第一波
	datas[0].monster_id[0]= 11305;
	datas[0].x[0] = 930;
	datas[0].y[0] = 230;
	
	datas[0].monster_id[1]= 11305;
	datas[0].x[1] = 930;
	datas[0].y[1] = 501;
	//第二波
	datas[10].monster_id[0]= 11305;
	datas[10].x[0] = 930;
	datas[10].y[0] = 365;
	//第三波
	datas[20].monster_id[0]= 11305;
	datas[20].x[0] = 930;
	datas[20].y[0] = 234;

	datas[21].monster_id[0]= 11305;
	datas[21].x[0] = 930;
	datas[21].y[0] = 234;
	//第四波
	datas[30].monster_id[0]= 11305;
    datas[30].x[0] = 930;
    datas[30].y[0] = 501;

	datas[31].monster_id[0]= 11305;
	datas[31].x[0] = 930;
	datas[31].y[0] = 501;
	//第五波
	datas[40].monster_id[0]= 11305;
    datas[40].x[0] = 930;
    datas[40].y[0] = 234;

	datas[40].monster_id[1]= 11305;
	datas[40].x[1] = 930;
	datas[40].y[1] = 365;

		
	datas[41].monster_id[0]= 11305;
	datas[41].x[0] = 930;
	datas[41].y[0] = 234;

	datas[41].monster_id[1]= 11305;
	datas[41].x[1] = 930;
	datas[41].y[1] = 365;
	//第六波
	datas[50].monster_id[0]= 11305;
    datas[50].x[0] = 930;
    datas[50].y[0] = 234;

	datas[50].monster_id[1]= 11306;
	datas[50].x[1] = 930;
	datas[50].y[1] = 365;

	datas[51].monster_id[0]= 11305;
	datas[51].x[0] = 930;
	datas[51].y[0] = 234;

	datas[52].monster_id[0]= 11305;
	datas[52].x[0] = 930;
	datas[52].y[0] = 234;
	//第七波
	datas[60].monster_id[0]= 11306;
    datas[60].x[0] = 930;
    datas[60].y[0] = 365;

	datas[60].monster_id[1]= 11306;
    datas[60].x[1] = 930;
    datas[60].y[1] = 501;

	datas[61].monster_id[0]= 11306;
    datas[61].x[0] = 930;
    datas[61].y[0] = 501;
	//第八波
	datas[70].monster_id[0]= 11305;
	datas[70].x[0] = 930;
	datas[70].y[0] = 234;

	datas[71].monster_id[0]= 11305;
	datas[71].x[0] = 930;
	datas[71].y[0] = 234;

	datas[72].monster_id[0]= 11305;
	datas[72].x[0] = 930;
	datas[72].y[0] = 234;
	//第九波
	datas[80].monster_id[0]= 11307;
	datas[80].x[0] = 930;
	datas[80].y[0] = 234;

	datas[80].monster_id[1]= 11306;
	datas[80].x[1] = 930;
	datas[80].y[1] = 365;

	datas[80].monster_id[2]= 11306;
	datas[80].x[2] = 930;
	datas[80].y[2] = 501;

	datas[81].monster_id[0]= 11306;

	datas[81].x[0] = 930;
	datas[81].y[0] = 501;
	//第十波
	datas[90].monster_id[0]= 11305;
	datas[90].x[0] = 930;
	datas[90].y[0] = 365;

	datas[91].monster_id[0]= 11305;
	datas[91].x[0] = 930;
	datas[91].y[0] = 365;

	datas[92].monster_id[0]= 11305;
	datas[92].x[0] = 930;
	datas[92].y[0] = 365;
	//第十一波
	datas[100].monster_id[0]= 11307;
    datas[100].x[0] = 930;
    datas[100].y[0] = 234;

	datas[100].monster_id[1]= 11306;
    datas[100].x[1] = 930;
    datas[100].y[1] = 365; 

	datas[101].monster_id[0]= 11307;
    datas[101].x[0] = 930;
    datas[101].y[0] = 234; 

    datas[101].monster_id[1]= 11306;
    datas[101].x[1] = 930;
    datas[101].y[1] = 365;	

	//第十二波
	datas[110].monster_id[0]= 11306;
	datas[110].x[0] = 930;
	datas[110].y[0] = 234; 

	datas[110].monster_id[1]= 11305;
	datas[110].x[1] = 930;
	datas[110].y[1] = 501;

	datas[111].monster_id[1]= 11305;
    datas[111].x[1] = 930;
    datas[111].y[1] = 501;

	datas[112].monster_id[1]= 11305;
    datas[112].x[1] = 930;
    datas[112].y[1] = 501;	

	//第十三波
	datas[120].monster_id[0]= 11305;
    datas[120].x[0] = 930;
    datas[120].y[0] = 234;

	datas[121].monster_id[0]= 11305;
    datas[121].x[0] = 930;
    datas[121].y[0] = 234;	

	datas[122].monster_id[0]= 11305;
	datas[122].x[0] = 930;
	datas[122].y[0] = 234;

	datas[120].monster_id[1]= 11306;
	datas[120].x[1] = 930;
	datas[120].y[1] = 501;

	datas[121].monster_id[1]= 11306;
	datas[121].x[1] = 930;
	datas[121].y[1] = 501;

	datas[122].monster_id[1]= 11306;
	datas[122].x[1] = 930;
	datas[122].y[1] = 501;
	
	//最后一波
	datas[130].monster_id[0]= 11306;
	datas[130].x[0] = 930;
	datas[130].y[0] = 234;

	datas[131].monster_id[0]= 11307;
	datas[131].x[0] = 930;
	datas[131].y[0] = 234;

	datas[130].monster_id[1]= 11305;
	datas[130].x[1] = 930;
	datas[130].y[1] = 365;

	datas[130].monster_id[2]= 11305;
	datas[130].x[2] = 930;
	datas[130].y[2] = 501;

	datas[131].monster_id[2]= 13083;
	datas[131].x[2] = 930;
	datas[131].y[2] = 501;

	

}

void process_battle_45_logic(Battle* btl, struct timeval  next_tm)
{
	if( btl->battle_section_data == 0){
		uint32_t buff_id = ranged_random(22, 24);
		for (PlayerVec::iterator it = btl->players_.begin(); it != btl->players_.end(); ++it)
		{
			Player* p = *it;
			if( p->cur_map->id != 1004507) continue;
			if( is_player_buff_exist(p, 22) )continue;
			if( is_player_buff_exist(p, 23) )continue;
			if( is_player_buff_exist(p, 24) )continue;
			add_player_buff(p, buff_id, 0);
		}
	}
}

void process_battle_925_init_logic(Battle* btl, void* para)
{
	Player* p = (Player*)para;
	add_player_buff(p, 28, 0, 0, false);
}

void process_battle_925_logic(Battle * btl, struct timeval next_tm)
{
	uint32_t map_id = 1092501;//药师后花园场景地图ID
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (m_map && btl->battle_section_data % 2 == 0) {
	//	bool mechanism_type[3]; //机关类型
		bool mechanism_pos[5]; //空挡
		//统计现有机关信息
		for (int i = 0; i < 5; i++) {
			mechanism_pos[i] = true;
		}

	    	
		for (PlayerSet::iterator it = m_map->pre_barriers.begin(); it != m_map->pre_barriers.end(); ++it) 
		{
			if ((*it)->role_type >= 30002 && (*it)->role_type <= 30004) {
				uint32_t x_pos = ((*it)->pos().x() - 250) / 150;
				mechanism_pos[x_pos] = false;
			}
		}
		int cnt =0;

		for (int i = 0; i < 5; i++) {
			if (mechanism_pos[i] == false) {
				cnt ++;
			}
		}

		if (cnt < 3) {
			int role_type = 30002 +  rand() %3;
			int pos_x = 0;
			int x = rand() % 5;
			if ( x < 2) {
				for (int j = 0; j < 5; ++j) {
					if (mechanism_pos[j] == true) {//改位置可以刷新机关
						pos_x = j * 150 + 250;
						mechanism_pos[j] = false;
						break;
					}
				}
			} else {
				for (int j = 4; j > 0  ; --j) {
					if (mechanism_pos[j] == true) {//改位置可以刷新机关
						pos_x = j * 150 + 250;
						mechanism_pos[j] = false;
						break;
					}
				}
			}
			call_monster_to_map(m_map, btl, role_type, pos_x, 150, neutral_team_1); 
		}
	}

	//刷出核桃怪
	if (btl->battle_section_data < sizeof(datas)/sizeof(datas[0]))
	{
		call_monsters_data* p = &datas[ btl->battle_section_data ];
		if( p->monster_id[0])
		{
			call_monster_to_map(m_map, btl, p->monster_id[0], p->x[0], p->y[0], 3);
		}	
		if( p->monster_id[1])
		{
			call_monster_to_map(m_map, btl, p->monster_id[1], p->x[1], p->y[1], 3);
		}
		if( p->monster_id[2])
		{
			call_monster_to_map(m_map, btl, p->monster_id[2], p->x[2], p->y[2], 3);
		}
	}
	btl->battle_section_data++;		
}

void process_battle_926_logic(Battle* btl, struct timeval  next_tm)
{
	for (int i = 0; i < 3; i++ ) {
		map_t * map = btl->get_map(i);
		for (PlayerSet::iterator it = map->pre_barriers.begin(); it != map->pre_barriers.end();
				++it) {
			if (((*it)->role_type >= 30005) && ((*it)->role_type <= 30006) && (*it)->mp) {
				(*it)->mp = 0;
				int idx = sizeof(btl_proto_t);
				pack_h(pkgbuf, 0, idx);
				pack_h(pkgbuf, (*it)->id, idx);
				pack_h(pkgbuf, 0, idx);
				init_btl_proto_head(pkgbuf, btl_change_mechanism, idx);
				map->send_to_map(pkgbuf, idx);
			}
		}
	}
}







void process_battle_927_logic(Battle *btl, struct timeval next_tm)
{
	timeval tv;
	gettimeofday(&tv, NULL);
	if (tv.tv_sec - btl->btl_start_tm > 120) {
		btl->on_btl_over(0, true); //2分钟时间用尽战斗结束
		return;
	} else if (tv.tv_sec - btl->btl_start_tm == 110) { //10s倒计时同步

	}	

	uint32_t map_id = 1092701;//场景地图ID
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (!m_map) {
		ERROR_LOG("MAP ID IS NOT EXIST!");
		return;
	}

	if ((btl->battle_section_data % 1000) % 3 == 0 ) { //时间 
		if (!m_map->get_monster_cnt()) { //地图怪物结束等待刷怪
			if ((btl->battle_section_data / 1000)  < 9) {
				int pos_x = 350;
				int pos_y = 450;
				if ((btl->battle_section_data / 1000) % 2 == 1) { //奇数波
					pos_x = pos_x + 700;
				} 

				for ( int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j ++) {
						call_monster_to_map(m_map, btl, 11311, pos_x + i* 50, pos_y + j *50); 
					}
				}
				call_monster_to_map(m_map, btl, 11311, pos_x, 450);

				btl->battle_section_data += 1000; //波数
			}
		}
	}

	if (btl->battle_section_data / 1000 == 9 ) { //最终boss
		if (!m_map->get_monster_cnt()) {  
			call_monster_to_map(m_map, btl, 13088, 500, 380);
		}
	}

	btl->battle_section_data++;

}


void process_battle_928_logic(Battle *btl, struct timeval next_tm)
{
	timeval tv;
	gettimeofday(&tv, NULL);
	uint32_t map_id = 1092701;//场景地图ID
	int y_ex = -40;
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (!m_map) {
		return;
	}	

	btl->battle_section_data++;

	if (btl->battle_section_data % 3 == 0) {
		if (m_map->get_monster_cnt() < 5) {
			call_monster_to_map(m_map, btl, 11312, 450, 350); 
		}
	}

		
	uint32_t game_time = tv.tv_sec - btl->btl_start_tm;
	Player * player = m_map->get_one_monster(11313);

	if (!player) {
		return;
	}


	if (game_time <= 60) { //第一阶段
		switch ((game_time / 5) % 6 ) {
			case 0://123
				for (int i = 0; i< 6; i++) {
					player->call_map_summon(16, 80 + i*150, 166 - y_ex, false);
				}
				break;
			case 1://456
				for (int i = 0; i< 6; i++) {
					player->call_map_summon(16, 80 + i*150, 299 - y_ex, false);
				}
				break;
			case 2: //789
				for (int i = 0; i< 6; i++) {
					player->call_map_summon(16, 80 + i*150, 433 - y_ex, false);
				}
				break;
			case 3://147
				for (int i = 0; i < 3; i++) {
					player->call_map_summon(16, 80, 133*i + 167 - y_ex, false);
					player->call_map_summon(16, 230, 133*i + 167 - y_ex, false);
				}
				break;
			case 4://258
				for (int i = 0; i < 3; i++) {
					player->call_map_summon(16, 380, 133*i + 167 - y_ex, false);
					player->call_map_summon(16, 530, 133*i + 167 - y_ex, false);
				}
				break;
			case 5://369
				for (int i = 0; i < 3; i++) {
					player->call_map_summon(16, 680, 133*i + 167 - y_ex, false);
					player->call_map_summon(16, 830, 133*i + 167 - y_ex, false);
				}
				break;
			default:
				break;
		}

	} else if (game_time > 60 && game_time <= 120){
		
		switch (((game_time - 60) / 5) % 6 ) {
		//钉板
			case 0://159
				for (int i = 0; i < 3; i++) {
					player->call_map_summon(16, 80 + 300 * i, 167 + i*133 - y_ex, false); 
					player->call_map_summon(16, 230 + 300 * i, 167 + i*133 - y_ex, false); 
				}
				break;
			case 1://753
				for (int i = 0; i < 3; i++) {
					player->call_map_summon(16, 80 + 300 * i, 433 - i*133 - y_ex, false); 
					player->call_map_summon(16, 230 + 300 * i, 433 - i*133 - y_ex, false); 
				}
				break;
			case 2://2468
				player->call_map_summon(16, 380, 167 - y_ex, false); //2 
				player->call_map_summon(16, 530, 167 - y_ex, false);
				player->call_map_summon(16, 680, 290 - y_ex, false); //6
				player->call_map_summon(16, 830, 290 - y_ex, false);
				player->call_map_summon(16, 80, 290 - y_ex, false); //4
				player->call_map_summon(16, 230, 290 - y_ex, false);
				player->call_map_summon(16, 380, 433 - y_ex, false); //8
				player->call_map_summon(16, 530, 433 - y_ex, false);
				break;
			case 3://147
				for (int i = 0; i < 3; i++) {
					player->call_map_summon(16, 80, 133*i + 167 - y_ex, false);
					player->call_map_summon(16, 230, 133*i + 167 - y_ex, false);
				}
				break;
			case 4://369
				for (int i = 0; i < 3; i++) {
					player->call_map_summon(16, 680, 133*i + 167 - y_ex, false);
					player->call_map_summon(16, 830, 133*i + 167 - y_ex, false);
				}
					
				break;
			case 5://2468
				player->call_map_summon(16, 380, 167 - y_ex, false); //2
				player->call_map_summon(16, 530, 167 - y_ex, false);
				player->call_map_summon(16, 680, 290 - y_ex, false);//6
				player->call_map_summon(16, 830, 290 - y_ex, false);
				player->call_map_summon(16, 80, 29 - 29 , false);//4
				player->call_map_summon(16, 230, 290 - y_ex, false);
				player->call_map_summon(16, 380, 433 - y_ex, false);//8
				player->call_map_summon(16, 530, 433 - y_ex, false);
				break;
			default:
				break;
		}	

	} else if (game_time > 120 && game_time <= 180) {
		switch (((game_time - 120) / 5) % 6 ) {
			case 0://13579
				for (int i = 0; i < 3; i++) { //159
					player->call_map_summon(16, 80 + 300 * i, 167 + i*133 - y_ex, false); 
					player->call_map_summon(16, 230 + 300 * i, 167 + i*133 - y_ex, false); 
				}
				player->call_map_summon(16, 680, 167 - y_ex, false);//3
				player->call_map_summon(16, 830, 167 - y_ex, false);
				player->call_map_summon(16, 80, 433 - y_ex, false);//7
				player->call_map_summon(16, 230, 433 - y_ex, false);
				break;
			case 1://2468
				player->call_map_summon(16, 380, 167 - y_ex, false); //2 
				player->call_map_summon(16, 530, 167 - y_ex, false);
				player->call_map_summon(16, 680, 290 - y_ex, false);//6
				player->call_map_summon(16, 830, 290 - y_ex, false);
				player->call_map_summon(16, 80, 290 - y_ex, false);//4
				player->call_map_summon(16, 230, 290 - y_ex, false);
				player->call_map_summon(16, 380, 433 - y_ex, false);//8
				player->call_map_summon(16, 530, 433 - y_ex, false);
				
				break;
			case 2://124578
				for (int i = 0; i < 3; i++) { //159
					player->call_map_summon(16, 100 + 300 * i, 167 + i*133 - y_ex, false); 
					player->call_map_summon(16, 250 + 300 * i, 167 + i*133 - y_ex, false); 
				}
				player->call_map_summon(16, 380, 167 - y_ex, false); //2 
				player->call_map_summon(16, 530, 163 - y_ex, false);
				player->call_map_summon(16, 80, 290 - y_ex, false);//4
				player->call_map_summon(16, 230, 290 - y_ex, false);
				player->call_map_summon(16, 380, 433 - y_ex, false);//8
				player->call_map_summon(16, 530, 433 - y_ex, false);
				break;
			case 3://456789
				for (int i = 0; i < 3; i++) {
					for (int j = 1; j < 3; j++) {
						player->call_map_summon(16, 80 + 300 *i, 167 + 133 *j - y_ex, false);
						player->call_map_summon(16, 230 + 300 *i, 167 + 133 *j - y_ex, false);
					}
				}
				break;
			case 4://235689
				for (int i = 1; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						player->call_map_summon(16, 80 + 300 *i, 167 + 133 *j - y_ex, false);
						player->call_map_summon(16, 230 + 300 *i, 167 + 133 *j - y_ex, false);
					}
				}
				break;
			case 5://123456
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 2; j++) {
						player->call_map_summon(16, 80 + 300 *i, 167 + 133 *j - y_ex, false);
						player->call_map_summon(16, 230 + 300 *i, 167 + 133 *j - y_ex, false);
					}
				}
				break;
			default:
				break;
		}
	}
}


void process_battle_930_logic(Battle *btl, struct timeval next_tm)
{
	uint32_t cur_time = get_now_tv()->tv_sec;
	uint32_t map_id = 1092701;//场景地图ID
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (!m_map) {
		return;
	}	

	if (cur_time - btl->btl_start_tm > 180) { //时间用完失败
		btl->on_btl_over(0, true); //3分钟时间用尽战斗结束
		return;
	} else if (btl->common_flag3_ == 2 &&  !m_map->get_monster_cnt()) { //3波怪全部打死,胜利
		btl->on_btl_over(0);
		return;
	}

	if (!m_map->get_monster_cnt() && btl->battle_section_data % 4 == 0) { //地图上无怪，刷怪
		if (btl->common_flag3_ == 0) { //第2波
			for (int i = 0; i < 2; ++i) {
				for (int j = 0; j < 2; ++j) {
					call_monster_to_map(m_map, btl, 11316, 600 + 70 * i, 300 + 70 *j);
				}
			}

		} else if (btl->common_flag3_ == 1) {//第3波
			for (int i = 0; i < 2; ++i) {
				for (int j = 0; j < 2; ++j) {
					call_monster_to_map(m_map, btl, 11316, 600 + 70 * i, 270 + 70 *j);
				}
			}
			call_monster_to_map(m_map, btl, 11317, 600, 390);//boss
		}
		btl->common_flag3_ ++;
	}	

	if (btl->common_flag1_  && btl->common_flag1_ + 10 < cur_time) { //镜子1, 旋转时间到停止旋转
		uint32_t mechanism_id =	m_map->get_mechanism_id_by_type(35000);
		m_map->mechansim_status_changed(mechanism_id, 0, 0);
		btl->common_flag1_ = 0;
	} 	

	if (btl->common_flag2_ && btl->common_flag2_ + 10 < cur_time) { //镜子2, 旋转时间到停止旋转
		uint32_t mechanism_id = m_map->get_mechanism_id_by_type(34999);
		m_map->mechansim_status_changed(mechanism_id, 0, 0);
		btl->common_flag2_ = 0;
	}

	btl->battle_section_data++;

}

uint32_t judge_939_victory_logic(Battle *btl)
{
    uint32_t winner_team = 0;
    Player *p = btl->get_some_player();

    //Three section
    float team1_hp = 0.0, team2_hp = 0.0; 
    for (PlayerSet::iterator mit = p->cur_map->monsters.begin(); mit != p->cur_map->monsters.end(); ++mit) {
        if ( (*mit)->role_type == 13095 ) {
            team1_hp = (float)((*mit)->hp) / (float)((*mit)->maxhp);
            //ERROR_LOG("xxxxxxxx [%d %d]", (*mit)->hp, (*mit)->maxhp);
        }
        if ( (*mit)->role_type == 13096 ) {
            team2_hp = (float)((*mit)->hp) / (float)((*mit)->maxhp);
            //ERROR_LOG("xxxxxxxx [%d %d]", (*mit)->hp, (*mit)->maxhp);
        }
    }

    if (team1_hp < team2_hp) {
        winner_team = 2;
    } else if (team1_hp > team2_hp){
        winner_team = 1;
    }
    //ERROR_LOG("Third section winner team [%u][%f %f]", winner_team, team1_hp, team2_hp);

    //First section
    if (winner_team == 0) {
        team1_hp = 0.0, team2_hp = 0.0; 
        for (PlayerSet::iterator mit = p->cur_map->monsters.begin(); mit != p->cur_map->monsters.end(); ++mit) {
            if ( (*mit)->role_type == 11341 )
                team1_hp = (float)((*mit)->hp) / (float)((*mit)->maxhp);
            if ( (*mit)->role_type == 11342 )
                team2_hp = (float)((*mit)->hp) / (float)((*mit)->maxhp);
        }
        if (team1_hp < team2_hp) {
            winner_team = 2;
        } else if (team1_hp > team2_hp) {
            winner_team = 1;
        }
        //ERROR_LOG("First section winner team [%u][%f %f]", winner_team, team1_hp, team2_hp);
    }

    //Second section
    if (winner_team == 0) {
        team1_hp = 0.0, team2_hp = 0.0; 
        for (PlayerSet::iterator mit = p->cur_map->monsters.begin(); mit != p->cur_map->monsters.end(); ++mit) {
            if ( (*mit)->role_type == 11343 )
                team1_hp = (float)((*mit)->hp) / (float)((*mit)->maxhp);
            if ( (*mit)->role_type == 11344 )
                team2_hp = (float)((*mit)->hp) / (float)((*mit)->maxhp);
        }
        if (team1_hp < team2_hp) {
            winner_team = 2;
        } else if (team1_hp > team2_hp) {
            winner_team = 1;
        }
        //ERROR_LOG("Second section winner team [%u][%f %f]", winner_team, team1_hp, team2_hp);
    }

    return winner_team;
/*
    for (PlayerVec::iterator pit = players_.begin(); pit != players_.end(); ++pit) {
        Player * p = pit;
    }*/
}
void process_battle_939_logic(Battle *btl, struct timeval next_tm)
{
	uint32_t cur_time = get_now_tv()->tv_sec;
    /*
	uint32_t map_id = 1093901;//场景地图ID
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (!m_map) {
		return;
	}	*/

	if (cur_time - btl->btl_start_tm > 600) { //时间用完关卡结束
        uint32_t winner_team = judge_939_victory_logic(btl);
		//btl->on_btl_over(0, true); //3分钟时间用尽战斗结束
        KDEBUG_LOG(0, "BTL TIME OVR\t[win=%u ]", winner_team);
        btl->over_pvp_939_stage(winner_team);
		return;
	}

}

void process_battle_941_final_game_logic(Battle *btl, struct timeval next_tm)
{
    if (btl->pvp_lv() != pvp_contest_last) {
        return;
    }
	uint32_t cur_time = get_now_tv()->tv_sec;
    
    Player *p = btl->get_dead_player();
    if ( p && (p->dead_start_tm != 0) && (cur_time > p->dead_start_tm + 30) ) {
        KDEBUG_LOG(0, "BTL TIME OVR\t[loser=%u ]", p->id);
        //on_pvp_player_dead(btl, p);
        end_pvp_on_player_dead(btl, p);
    }
}

void process_battle_929_logic(Battle * btl, struct timeval next_tm)
{
	static uint32_t pos_929_x[] = {281, 479, 671, 245, 469, 705, 197, 480, 760};
	static uint32_t pos_929_y[] = {245, 256, 257, 352, 352, 350, 479, 482, 480};

	timeval tv;
	
	gettimeofday(&tv, NULL);
	btl->battle_section_data++;

	uint32_t *para = (uint32_t*)btl->btl_logic_data.data;
	if( para == NULL)return;

	if( para[1] == 1) return;

	Player* p = btl->get_some_player();
	if(p == NULL)return;

	int32_t duration = taomee::timediff2(tv, btl->start_tm) - 5000;
	if(duration <= 0)return ;


	if(para[0] >= 40){
		p->call_monster(19142, 753, 480, barrier_team);	
		para[1] = 1;
	}

	if (duration > 280000) 
	{
		if(para[0] < 40)
		{
			btl->on_btl_over(0, true); //4分钟时间用尽战斗结束
		}
		return;
	}

	uint32_t index = ranged_random(0, 8);
	uint32_t monster_id =  (index < 7)? 11314: 11315;

	if(para[0] < 10 &&  btl->battle_section_data % 4 == 0 )
	{	
		p->call_monster(monster_id, pos_929_x[index], pos_929_y[index], 3, 1009);	
	}
	else if( para[0] >= 10 && para[0] < 25 && btl->battle_section_data % 3 == 0)
	{
		p->call_monster(monster_id, pos_929_x[index], pos_929_y[index], 3, 1010);
	}
	else if( para[0] >= 25 && para[0] < 40 && btl->battle_section_data % 2 == 0)
	{
		p->call_monster(monster_id, pos_929_x[index], pos_929_y[index], 3, 1011);
	}

}
void* battle_929_data_alloc()
{
	uint32_t* para = new uint32_t[2];
	para[0] = 0; //当前杀死怪物积分数
	para[1] = 0; //标志
	return (void*)para;
}

void battle_929_data_release(void* para)
{
	uint32_t * pdata = (uint32_t*)(para);
	delete [] pdata;
}

//前n-1个数中随机选取m个
void  ran_pos(int * ptr, int n, int m) 
{
	int k = 0;
	int j = 0;
	for (int i = 0; i < n; i++) {
		k = ptr[i];
		j = rand()%(n - i) + i;
		ptr[i] = ptr[j];
		ptr[j] = k;
	}
}
//10波前红色情况
void proc_1_rand(int &x, int & y) 
{
	x = rand() % 100;
	y = rand() % 100;
	if (x >= 50) {
		x = 4;
	} else if (x < 35) {
		x = 3;
	} else {
		x = 2;
	}

	if (y >= 50) {
		y = 2;
	} else if (y < 35) {
		y = 3;
	} else {
		y = 4;
	}
}

void proc_2_rand(int &x, int & y)
{
	x = rand() % 100;
	y = rand() % 100;
	if (x >= 60) {
		x = 2;
	} else if ( x < 20) {
		x = 4;
	} else {
		x = 2;
	}

	if (y >= 40) {
		y = 4;
	} else if ( y < 10) {
		y = 2;
	} else {
		y = 3;
	}

}

void proc_3_rand(int &x, int &y, int &z)
{
	x = rand() % 100;
	y = rand() % 100;
	z = rand() % 100;

	if (x >= 70) {
		x = 3;
	} else if (x < 40) {
		x = 1;
	} else {
		x = 2;
	}

	if (y >= 80) {
		y = 3;
	} else if (y < 45) {
		y = 1;
	} else {
		y = 2;
	}

	if (z >= 80) {
		z = 3;
	} else if (z < 45) {
		z = 1;
	} else {
		z = 2;
	}
}

void proc_4_rand(int &x, int &y, int &z)
{
	x = rand() % 100;
	y = rand() % 100;
	z = rand() % 100;
	if (x >= 90) {
		x = 3;
	} else if (x < 55) {
		x = 1;
	} else {
		x = 2;
	}

	if (y >= 90) {
		y = 1;
	} else if (y < 55) {
		y = 3;
	} else {
		y = 2;
	} 

	if (z >= 90) {
		z = 1;
	} else if (z < 55) {
		z = 3;
	} else {
		z = 2;
	}
}

void swap_x(int & x, int & y, int &z)
{
	int k1 = x;
	int k2 = y;
	x = z;
	y = k1;
	z = k2;
}

void process_battle_932_logic(Battle * btl, struct timeval next_tm)
{
	uint32_t cur_time = get_now_tv()->tv_sec;
	uint32_t map_id = 1093201;//场景地图ID
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (!m_map) {
		return;
	}	

	//battle_section_data 当前波数	
	//common_flag1_ 当前目标颜色
	//common_flag2_ 错误一次+100, 正确一起+1
	//common_flag3_ 上一波刷新时间， 超过3s就刷下一波
	//
	if ((btl->common_flag2_ / 100 >= 3) ||
		   	(btl->battle_section_data >= 40 && (btl->common_flag3_ + 3 < cur_time))) { //error more than 3 times
		btl->on_btl_over(0, true);
	//	ERROR_LOG("BATTLE 932 OVER! !");
		return;
	}

	if (btl->common_flag2_ % 100 >= 30) {
		if (!m_map->barriers.size()) {
		//抹去上轮机关
			for (PlayerSet::iterator iter =  m_map->pre_barriers.begin(); iter != m_map->pre_barriers.end(); ++ iter) {
				Player * p = (*iter);
				notify_delete_player_obj(p);
				delete p;
			}	
			m_map->pre_barriers.clear();

			call_monster_to_map(m_map, btl, 19143, 490, 350, barrier_team);
		}	
		return;
	}

	if (btl->common_flag3_ == 0) { //初始化
		btl->common_flag3_ = cur_time + 2;
		return;
	}



	if (btl->common_flag3_ + 3 < cur_time) {//超过3s,下一轮

		btl->common_flag3_ = cur_time;

		int p_pos[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
		int x = 0;
		int y = 0;
		int z = 0;

		//抹去上轮机关
		for (PlayerSet::iterator iter =  m_map->pre_barriers.begin(); iter != m_map->pre_barriers.end(); ++ iter) {
			Player * p = (*iter);
			notify_delete_player_obj(p);
			delete p;
		}	
		m_map->pre_barriers.clear();


		
		int x_pos[12];

		for (int i = 0; i < 12; i++) {
			x_pos[i] = 0;
		}

		if (btl->battle_section_data < 20) {
			btl->common_flag1_ = rand() % 4; //0 1  红色 3 4 蓝色
			if (btl->common_flag1_  == 2) {
				btl->common_flag1_ = 4;
			}
			if (btl->battle_section_data < 10) {
				proc_1_rand(x, y);
			} else {
				proc_2_rand(x, y);
			}

			ran_pos(p_pos, 12, x + y);//随机出位置
			switch(btl->common_flag1_ / 2 ) {
				case 0://红色
					break;
				case 1:
					swap(x, y);//蓝色
					break;
				default:
					break;
			}

			ran_pos(p_pos, x + y, x);//随机出红


			for (int i = 0; i < x + y; i++) { //刷新机关
				int k = p_pos[i];
				if (i < x) { //红
					x_pos[k] = 1;
				} else { //蓝 
					x_pos[k] = 2; 
				}
			}
			

		} else if (btl->battle_section_data >= 20 ) {
			btl->common_flag1_ = rand() % 9;//0 1 2  红色  3 4 5  蓝色 6 7 8 紫色

			if (btl->battle_section_data < 30) {
				proc_3_rand(x, y, z);
			} else {
				proc_4_rand(x, y, z);
			}
			ran_pos(p_pos, 12, x + y + z);
			switch(btl->common_flag1_ / 3) {
				case 0:
					break;
				case 1:
					swap_x(x, y, z);
					break;
				case 2:
					swap_x(x, y, z);
					swap_x(x, y, z);
					break;
				default:
					break;
			}
			ran_pos(p_pos, x + y + z, x);//红 
			ran_pos(p_pos + x, y + z, y);//蓝

			for (int i = 0; i < (x + y + z); i++) {
				int k = p_pos[i];
				if (i < x) {//红
					x_pos[k] = 1;
				} else if ( x <= i && i < x + y) {//蓝
					x_pos[k] = 2;

				} else { //紫
					x_pos[k] = 3;
				}
			}

		} 

		static uint32_t x_x[12] = {217, 389, 566, 738, 182, 380, 574, 769, 136, 364, 591, 818};
		static uint32_t y_y[12] = {228, 228, 228, 228, 324, 324, 324, 324, 445, 445, 445, 445};

		for (int i = 0; i < 12; i++) {
			uint32_t monster_id = 30007;
			monster_id += x_pos[i];
			uint32_t pos_x = x_x[i];
			uint32_t pos_y = y_y[i];
			call_monster_to_map(m_map, btl, monster_id, pos_x, pos_y);
		//	ERROR_LOG("CALL MONSTER %u %u", monster_id, i);
		}

		btl->battle_section_data++;
		Player * p = m_map->get_one_player();
		btl->battle_extra_logic_msg(p);
	}

}

int extra_mechanism_logic(Battle * btl, Player * mechanism)
{
	uint32_t cur_time = get_now_tv()->tv_sec;
	if (btl->stage()->id == 932) {
		if (btl->common_flag1_ > 9) {
			return 0;
		}

		int x = 1;

		if (mechanism->role_type == 30007) {
			btl->common_flag2_ += 100;
			x = 3;
		} else if (mechanism->role_type == 30008) {
			if (btl->common_flag1_ <= 2) {
				btl->common_flag2_ ++;
			} else if (btl->common_flag1_ < 9) {
				btl->common_flag2_ += 100;
				x = 3;
			}
		} else if (mechanism->role_type == 30009) {
			if (btl->common_flag1_ > 2 && btl->common_flag1_ <= 5) {
				btl->common_flag2_ ++;
			} else if(btl->common_flag1_ < 9) {
				btl->common_flag2_ += 100;
				x = 3;
			}
		} else if (mechanism->role_type == 30010) {//彩人堂 ， 紫色机关
			if (btl->common_flag1_ > 5 && btl->common_flag1_ <= 8 ) {
				btl->common_flag2_ ++;
			} else if (btl->common_flag1_ < 9) {
				btl->common_flag2_ += 100;
				x = 3;
			}
		}
		btl->common_flag3_ = cur_time - 2;
		btl->common_flag1_ = 10;
		return x;
	} 

	if (btl->stage()->id == 934) {
		int x = 1;
		static int m_x[12] = {199, 344, 500, 653, 157, 319, 491, 661, 104, 280, 479, 673};
		if (btl->common_flag3_ <= 0 || btl->common_flag3_ > 20) {
			return 0; //
		}
		for (int i = 0; i < 12; i++) {
			if (mechanism->pos().x() == m_x[i]) {
				if (test_bit_on32(btl->common_flag2_, i + 1)) {
					//ERROR_LOG("SET FLAG2 %u bit 0", i + 1);
					btl->common_flag2_ = set_bit_off32(btl->common_flag2_, i + 1);
					btl->common_flag3_ --;
					if (btl->common_flag2_ == 0) {
						btl->battle_section_data ++;
						btl->common_flag3_ = cur_time;
					}
				} else {
				//	ERROR_LOG("FLAG2 %u is not 0", i + 1);
					btl->common_flag2_ = 0;
					btl->common_flag3_ = cur_time;
					btl->battle_section_data += 100;
					x = 3;
				}
				break;
			}
		}
		return x;
	}
	return 0;
}
		
void process_battle_933_logic(Battle * btl, struct timeval next_tm)
{
	uint32_t map_id = 1093301;//场景地图ID
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (!m_map) {
		return;
	}	


	btl->battle_section_data++; //时间


	if (btl->common_flag1_ >= 20) {
		if (!m_map->get_one_monster(11324) && !m_map->get_one_barriers(19144)) {
			call_monster_to_map(m_map, btl, 19144, 490, 350, barrier_team);
		}
		return;
	}




	if (btl->battle_section_data % 5 == 0) {
		if (m_map->get_monster_cnt() < 9) {

			Player *p = m_map->get_one_player();
			if (p) {
				btl->battle_extra_logic_msg(p);
			}

			int pos_a[4] = {0, 1, 2, 3};
			int x2 = rand() % 2 + 2;
			int x_p[4] = {152, 356, 600, 806};
			ran_pos(pos_a, 4, x2);
			for (int i = 0; i < x2; i++) {
				call_monster_to_map(m_map, btl, 11324, x_p[pos_a[i]], 233);
			}
			btl->common_flag1_ ++;
		}
	}


	if ((btl->battle_section_data + 1 ) % 11 == 0) {
		
		int x1 = rand()%2 + 1;
		static int x_x[5] = {283, 690, 874, 485, 90};
		static int y_y[5] = {268, 268, 390, 460, 390};
		int pos[5] = {0, 1, 2, 3, 4};
		int p_pox[4] = {0, 1, 2, 3};
		ran_pos(p_pox, 4, x1);//随机种类
		ran_pos(pos, 5, x1);//随机位置
		for (int i = 0; i < x1; i++) {
			call_monster_to_map(m_map, btl, p_pox[i] + 11325, x_x[(pos[i])], y_y[(pos[i])],
				   neutral_team_1);	
		}
	} 

}


void process_battle_934_logic(Battle * btl, struct timeval next_tm)
{
	uint32_t map_id = 1093401;//场景地图ID
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (!m_map) {
		return;
	}	
	Player * m_barriers = m_map->get_one_barriers(19145);
	if (m_barriers) {
		if (btl->battle_section_data % 100 < 10) {
			if (!is_player_buff_exist(m_barriers, 28)) {
				add_player_buff(m_barriers, 28, 0, 300);
			}

		} else {
			if (is_player_buff_exist(m_barriers, 28)) {
				del_player_buff(m_barriers, 28);
			}
		}
	}


	static int x_x[12] = {199, 344, 500, 653, 157, 319, 491, 661, 104, 280, 479, 673};
	static int y_y[12] = {246, 246, 246, 246,  340, 340, 340, 340, 446, 446, 446, 446};

	if ((btl->common_flag3_ > 0 && btl->common_flag3_ < 20) || btl->common_flag2_ != 0 ) { //改轮未结束
		return;
	} 

	uint32_t cur_time = get_now_tv()->tv_sec;
	if (btl->common_flag3_ == 0) {
		btl->common_flag3_ = cur_time + 3;
		return;
	}

	if (btl->common_flag3_ + 2 > cur_time) {
		return;
	} else {
		btl->common_flag3_ = 0;
	}


	if (!m_map->pre_barriers.empty()) {

		for (PlayerSet::iterator it = m_map->pre_barriers.begin(); it != m_map->pre_barriers.end();
				++it) {
			Player * p = (*it);
			notify_delete_player_obj(p);
			delete p;
		}
		m_map->pre_barriers.clear();
	}

	if (btl->battle_section_data % 100 >= 10) {
		return;
	}

	if (btl->battle_section_data / 100 >= 6) {
		btl->on_btl_over(0, true);
	}


	//开启下一轮
	if (btl->common_flag1_ < 15)  {
		int p_pos[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
		ran_pos(p_pos, 12, 5); 
		if (btl->common_flag1_ < 3) {
			btl->common_flag3_ = 3;
		} else if (btl->common_flag1_ < 7) {
			btl->common_flag3_ = 4;
		} else if (btl->common_flag1_ < 11) {
			btl->common_flag3_ = 5;
		} else if (btl->common_flag1_ < 15) {
			btl->common_flag3_ = 6;
		}

		for (int i = 0; i < 12; i ++) {
			call_monster_to_map(m_map, btl, 30011, x_x[i], y_y[i]);
		}

		btl->common_flag2_ = 0;
		for (uint32_t i = 0; i < btl->common_flag3_; i++) {
			btl->common_flag2_ = set_bit_on32(btl->common_flag2_, p_pos[i] + 1);
			//ERROR_LOG("SET FLAG2 bit %d be 1", p_pos[i] + 1);
		}	

		btl->common_flag1_ ++;
		Player *p = m_map->get_one_player();
		if (p) {
			btl->battle_extra_logic_msg(p);
		}
	} else {
		btl->on_btl_over(0, true);
	}

}


void process_battle_942_logic(Battle * btl, struct timeval next_tm)
{
	uint32_t map_id = 1094201;//场景地图ID
	uint32_t index = get_stage_map_id(map_id);
    map_t * m_map = btl->get_map(index);	   
	if(m_map == NULL)return;

	Player* p = m_map->get_one_player();

	//uint32_t mon_ids[] = { 11348, 11349, 11351, 11350};
	uint32_t mechanism_ids[] = {11352, 11353, 11354, 11355};
	uint32_t pos_x[] = { 500, 700, 900};


	if(btl->battle_section_data <200)
	{
		uint32_t count = btl->battle_section_data;
		if(count % 20 == 0)
		{
			uint32_t index = ranged_random(0, 3);
			uint32_t x_index = ranged_random(0, 2);

			p->call_monster( mechanism_ids[index], pos_x[x_index], 206, 1, 0, 8);		
			
			index = ranged_random(0, 3);
			x_index = ranged_random(0, 2);
			p->call_monster( mechanism_ids[index], pos_x[x_index], 319, 1, 0 ,8);

			index = ranged_random(0, 3);	
			x_index = ranged_random(0, 2);
			p->call_monster( mechanism_ids[index], pos_x[x_index], 433, 1, 0, 8);
		}

		
		switch(count)
		{
			case 0://第一波
			{
				 p->call_monster(11348, 1413, 206);
				 p->call_monster(11348, 1413, 319);
				 p->call_monster(11348, 1413, 433);
			}
			break;

			case 20://第二波
			{
				p->call_monster(11349, 1413, 206);
				p->call_monster(11349, 1413, 319);
				p->call_monster(11349, 1413, 433);
			}
			break;

			case 40://第三波
			{
				p->call_monster(11349, 1413, 206);
				p->call_monster(11348, 1413, 319);
				p->call_monster(11349, 1413, 433);
			}
			break;	

			case 60://第四波第一排
			{
				p->call_monster(11348, 1413, 206);
				p->call_monster(11348, 1413, 319);
				p->call_monster(11348, 1413, 433);	  
			}
			break;

			case 63://第四波第二排
			{
				p->call_monster(11349, 1413, 206);
				p->call_monster(11349, 1413, 319);
				p->call_monster(11349, 1413, 433);
			}
			break;

			case 80://第五波第一排
			{
				p->call_monster(11350, 1413, 206);
				p->call_monster(11350, 1413, 319);
				p->call_monster(11350, 1413, 433); 
			}
			break;

			case 83://第五波第二排
			{
				p->call_monster(11348, 1413, 206);
				p->call_monster(11348, 1413, 319);
				p->call_monster(11348, 1413, 433); 
			}
			break;


			case 100://第六波第一排
			{
				p->call_monster(11350, 1413, 206);
				p->call_monster(11350, 1413, 319);
				p->call_monster(11350, 1413, 433); 
			}
			break;

			case 103://第六波第二排
			{
				p->call_monster(11349, 1413, 206);
				p->call_monster(11349, 1413, 319);
				p->call_monster(11349, 1413, 433);
			}
			break;

			case 120://第七波第一排
			case 123://第七波第二排
			case 126://第七波第三排
			case 129://第七波第四排
			{
				p->call_monster(11350, 1413, 206);
				p->call_monster(11350, 1413, 319);
				p->call_monster(11350, 1413, 433);
			}
			break;

			case 140://第八波第一排
			{
				p->call_monster(11351, 1413, 206);
				p->call_monster(11351, 1413, 319);
				p->call_monster(11351, 1413, 433);
			}
			break;

			case 143://第八波第二排
			{
				p->call_monster(11348, 1413, 206);
				p->call_monster(11348, 1413, 319);
				p->call_monster(11348, 1413, 433); 
			}
			break;

			case 146://第八波第三排
			{
				p->call_monster(11349, 1413, 206);
				p->call_monster(11349, 1413, 319);
				p->call_monster(11349, 1413, 433); 
			}
			break;


			case 160://第九波第一排
			{
				p->call_monster(11350, 1413, 206);
				p->call_monster(11350, 1413, 319);
				p->call_monster(11350, 1413, 433); 
			}
			break;

			case 163://第九波第二排
			{
				p->call_monster(11351, 1413, 206);
				p->call_monster(11351, 1413, 319);
				p->call_monster(11351, 1413, 433); 
			}
			break;

			case 166://第九波第三排
			{
				p->call_monster(11348, 1413, 206);
				p->call_monster(11348, 1413, 319);
				p->call_monster(11348, 1413, 433); 
			}
			break;

			case 169://第九波第四排
			{
				p->call_monster(11349, 1413, 206);
				p->call_monster(11349, 1413, 319);
				p->call_monster(11349, 1413, 433); 
			}
			break;

			case 180://第十波第一排
			case 183://第十波第二排
			{
				p->call_monster(11350, 1413, 206);
				p->call_monster(11350, 1413, 319);
				p->call_monster(11350, 1413, 433);
			}
			break;

			case 186://第十波第三排
			{
				p->call_monster(13097, 1413, 319);
			}
			break;
		}
	}

	btl->battle_section_data++; //时间
}

void process_battle_952_logic(Battle * btl, struct timeval next_tm)
{
	Player* p = btl->get_some_player();
	if(p->cur_map->monsters.size() == 0)
	{
		btl->on_btl_over(NULL);
		p->call_monster(30012, 680, 240, 1);	
	}
}

//天外飞石
void process_battle_953_logic(Battle *btl, struct timeval next_tm)
{
	uint32_t map_id = 1095301;//场景地图ID
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (!m_map) {
		return;
    }

    static int x_x[20] = {232, 360, 480, 595, 720,
                          200, 345, 480, 611, 752,
                          160, 320, 480, 633, 793,
                          125, 297, 480, 653, 828};
	static int y_y[20] = {270, 270, 270, 270, 270,
                          343, 343, 343, 343, 343,
                          422, 422, 422, 422, 422,
                          505, 505, 505, 505, 505};


    if (btl->common_flag1_ >= 15) {
        //Player * soldier = m_map->get_one_player();
        btl->on_btl_over(0, false);
    }

	uint32_t cur_time = get_now_tv()->tv_sec;
	if (btl->common_flag3_ == 0 || btl->common_flag3_ + 6 <= cur_time) {
		btl->common_flag3_ = cur_time + 1;
        btl->common_flag1_ += 1;
        btl->battle_section_data = 0;
        //ERROR_LOG("INIt 000000000000");

        if (!m_map->pre_barriers.empty()) {

            for (PlayerSet::iterator it = m_map->pre_barriers.begin(); it != m_map->pre_barriers.end();
                ++it) {
                Player * p = (*it);
                //ERROR_LOG("DELETE %u XXXXXXXXX", p->role_type);
                notify_delete_player_obj(p);
                delete p;
            }
            m_map->pre_barriers.clear();
        }

        return;
    }

    //ERROR_LOG("SET XXXXX [%u %u]", btl->common_flag3_, cur_time);
	if (cur_time >= btl->common_flag3_ && cur_time < btl->common_flag3_ + 1) {
        if (btl->common_flag2_ == 0) {
            //Just select point
            //ERROR_LOG("SET 1111111111111111");
            int pos[20] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
            ran_pos(pos, 20, 13);
            for (uint32_t i = 0; i < 13; i++) {
                btl->common_flag2_ = set_bit_on32(btl->common_flag2_, pos[i] + 1);
                //ERROR_LOG("SET FLAG2 bit %d be 1", p_pos[i] + 1);
            }	

            Player *p = m_map->get_one_player();
            if (p) {
                btl->battle_extra_logic_msg(p);
            }
        }
    } else if ( cur_time >= btl->common_flag3_ + 1 && cur_time < btl->common_flag3_ + 2) {
        if (btl->common_flag2_ != 0){
            //Call summon monster
            //ERROR_LOG("CALL 22222222222222");
            int cnt = 0;
            for (uint32_t i = 1; i < 21; i++) {
                if (test_bit_on32(btl->common_flag2_, i)) {
                    if (cnt++ == 6) {
                        call_monster_to_map(m_map, btl, 34998, x_x[i-1], y_y[i-1]);
                    } else {
                        call_monster_to_map(m_map, btl, 35001, x_x[i-1], y_y[i-1]);
                    }
                }
            }
            btl->common_flag2_ = 0;
        }
    } else {
        //ERROR_LOG("XXXXXXXX %u XXXXXXXXXX", btl->battle_section_data);
        if (btl->battle_section_data == 0 && !m_map->pre_barriers.empty()) {
            for (PlayerSet::iterator it = m_map->pre_barriers.begin(); it != m_map->pre_barriers.end(); ++it) {
                Player * p = (*it);
                //if (p->role_type == 35001) {
                    //ERROR_LOG("FUCK 33333333333333333");
                    p->call_map_summon(29, p->pos().x(), p->pos().y() - 50, false);
                    //notify_delete_player_obj(p);
                    //delete p;
                    //m_map->pre_barriers.erase(it++);
                //} else {
                //    ++it;
                //}
                btl->battle_section_data = 1;
            }
        }
    }
    return;
}

void process_battle_978_logic(Battle *btl, struct timeval next_tm)
{
	uint32_t map_id = 1097801;//场景地图ID
	uint32_t cur_time = get_now_tv()->tv_sec;
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	Player * player = btl->get_some_player();
	if (!m_map || !player) {
		return;
    }

	if (player->is_dead()) {
		btl->common_flag3_ = cur_time;
		return;
	}

    static int x_x[20] = {232, 360, 480, 595, 720,
                          200, 345, 480, 611, 752,
                          160, 320, 480, 633, 793,
                          125, 297, 480, 653, 828};
	static int y_y[20] = {270, 270, 270, 270, 270,
                          343, 343, 343, 343, 343,
                          422, 422, 422, 422, 422,
                          505, 505, 505, 505, 505};


    if (btl->common_flag1_ == 15) {
        //Player * soldier = m_map->get_one_player();
        call_monster_to_map(m_map, btl, 11478, 480, 422);
		btl->common_flag1_++;
    } else if (btl->common_flag1_ > 15) {
    	return;
    }

	if (btl->common_flag3_ == 0 || btl->common_flag3_ + 6 <= cur_time) {
		btl->common_flag3_ = cur_time + 1;
        btl->common_flag1_ += 1;
        btl->battle_section_data = 0;
        //ERROR_LOG("INIt 000000000000");

        if (!m_map->pre_barriers.empty()) {

            for (PlayerSet::iterator it = m_map->pre_barriers.begin(); it != m_map->pre_barriers.end();
                ++it) {
                Player * p = (*it);
                //ERROR_LOG("DELETE %u XXXXXXXXX", p->role_type);
                notify_delete_player_obj(p);
                delete p;
            }
            m_map->pre_barriers.clear();
        }

        return;
    }

    //ERROR_LOG("SET XXXXX [%u %u]", btl->common_flag3_, cur_time);
	if (cur_time >= btl->common_flag3_ && cur_time < btl->common_flag3_ + 1) {
        if (btl->common_flag2_ == 0) {
            //Just select point
            //ERROR_LOG("SET 1111111111111111");
            int pos[20] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
            ran_pos(pos, 20, 13);
            for (uint32_t i = 0; i < 13; i++) {
                btl->common_flag2_ = set_bit_on32(btl->common_flag2_, pos[i] + 1);
                //ERROR_LOG("SET FLAG2 bit %d be 1", p_pos[i] + 1);
            }	

            Player *p = m_map->get_one_player();
            if (p) {
                btl->battle_extra_logic_msg(p);
            }
        }
    } else if ( cur_time >= btl->common_flag3_ + 1 && cur_time < btl->common_flag3_ + 2) {
        if (btl->common_flag2_ != 0){
            //Call summon monster
            //ERROR_LOG("CALL 22222222222222");
            //int cnt = 0;
            for (uint32_t i = 1; i < 21; i++) {
                if (test_bit_on32(btl->common_flag2_, i)) {
                    //if (cnt++ == 6) {
                       // call_monster_to_map(m_map, btl, 34998, x_x[i-1], y_y[i-1]);
                    //} else {
                        call_monster_to_map(m_map, btl, 35001, x_x[i-1], y_y[i-1]);
                    //}
                }
            }
            btl->common_flag2_ = 0;
        }
    } else {
        if (btl->battle_section_data == 0 && !m_map->pre_barriers.empty()) {
            for (PlayerSet::iterator it = m_map->pre_barriers.begin(); it != m_map->pre_barriers.end(); ++it) {
                Player * p = (*it);
                    p->call_map_summon(29, p->pos().x(), p->pos().y() - 50, false);
                btl->battle_section_data = 1;
            }
        }
    }
    return;
}


void SpecialExpDoubleTimeMrg::init()
{
	ev_mgr.add_event(*this, &SpecialExpDoubleTimeMrg::reset_exp_double_time_flag, *get_now_tv(), 3000, -1);
	ev_mgr.add_event(*this, &SpecialExpDoubleTimeMrg::reset_exp_double_time_2_flag, *get_now_tv(), 3000, -1);
	ev_mgr.add_event(*this, &SpecialExpDoubleTimeMrg::reset_exp_double_time_flag_which_can_use_double_tm_item, *get_now_tv(), 3000, -1);
}

int SpecialExpDoubleTimeMrg::reset_exp_double_time_flag()
{
	if (is_in_time(5, get_now_tv()->tv_sec)) {
		double_tm_flag = true;
	} else {
		double_tm_flag = false;
	}
	return 0;
}

int SpecialExpDoubleTimeMrg::reset_exp_double_time_2_flag()
{
	if (is_in_time(8, get_now_tv()->tv_sec)) {
		double_tm_flag_2 = true;
	} else {
		double_tm_flag_2 = false;
	}
	return 0;
}

int SpecialExpDoubleTimeMrg::reset_exp_double_time_flag_which_can_use_double_tm_item()
{
	if (is_in_time(7, get_now_tv()->tv_sec)) {
		double_tm_flag_whitch_can_still_use_double_tm_item = true;
	} else {
		double_tm_flag_whitch_can_still_use_double_tm_item = false;
	}
	return 0;
}		
void process_battle_977_logic(Battle * btl, struct timeval next_tm)
{
	uint32_t wuseng_role_type = 11474;
	uint32_t guisewuseng_role_type = 11475;
	uint32_t black_dargon_role_type = 11477;

	uint32_t map_id = 1097701;//场景地图ID
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);

	if (!m_map) {
		return;
	}	


	//rand pos for wuseng ang guisewuseng
	if (!btl->common_flag1_ ) {
		static int wuseng_x_pos[3] = { 800, 1050, 1300};
		static int wuseng_y_pos[3] = { 307, 600, 357};
		int pos_info[3] = {0, 1, 2};
		ran_pos(pos_info, 3, 3);

		call_monster_to_map(m_map,
			   				btl, 
							wuseng_role_type,
						   	wuseng_x_pos[(pos_info[0])],
			   				wuseng_y_pos[(pos_info[0])],
							monster_team);

		call_monster_to_map(m_map,
			   				btl, 
							guisewuseng_role_type,
						   	wuseng_x_pos[(pos_info[1])],
			   				wuseng_y_pos[(pos_info[1])],
							monster_team);

		call_monster_to_map(m_map,
			   				btl, 
							guisewuseng_role_type,
						   	wuseng_x_pos[(pos_info[2])],
			   				wuseng_y_pos[(pos_info[2])],
							monster_team);

		btl->common_flag1_ = 1;
	}

	if (btl->common_flag3_ ) {
        Player * atker = btl->get_one_player();
        if (atker) {
           btl->notify_client_player_killed_monster(atker->id, 13129);
        }
		btl->on_btl_over(0);
	}

	if (!m_map->get_monster_cnt(wuseng_role_type)) 
	{
		btl->on_btl_over(0, true);
	}	


	if ( !m_map->get_one_monster(black_dargon_role_type)
			&& !m_map->get_one_monster(guisewuseng_role_type) ) {
		btl->common_flag2_ = 1;
	}


}

void process_battle_979_logic(Battle *btl, struct timeval next_tm)
{
 	if (btl->pvp_lv() != pvp_ghost_game) {
		ERROR_LOG("USER NOT IN PVP GHOST GAME %u", btl->pvp_lv());
        return;
    }

	uint32_t cur_time = get_now_tv()->tv_sec;

	if (btl->common_flag1_ == 0) {
		btl->common_flag1_  = cur_time; 
	}



    
    Player *p = btl->get_dead_player();
    if ( p && (p->dead_start_tm != 0) && (cur_time > p->dead_start_tm + 10) ) {
        KDEBUG_LOG(0, "BTL TIME OVR\t[loser=%u ]", p->id);
        //on_pvp_player_dead(btl, p);
        end_pvp_on_player_dead(btl, p);
		return;
    }

}

void process_battle_984_logic(Battle *btl, struct timeval next_tm)
{
	uint32_t cur_time = get_now_tv()->tv_sec;
	uint32_t map_id = 1092701;//场景地图ID
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (!m_map) {
		return;
	}	

	if (cur_time - btl->btl_start_tm > 180) { //时间用完失败
		btl->on_btl_over(0, true); //3分钟时间用尽战斗结束
		return;
	} else if (btl->common_flag3_ == 2 &&  !m_map->get_monster_cnt()) { //3波怪全部打死,胜利
		btl->on_btl_over(0);
		return;
	}

	if (!m_map->get_monster_cnt() && btl->battle_section_data % 4 == 0) { //地图上无怪，刷怪
		if (btl->common_flag3_ == 0) { //第2波
			for (int i = 0; i < 2; ++i) {
				for (int j = 0; j < 2; ++j) {
					call_monster_to_map(m_map, btl, 11316, 600 + 70 * i, 300 + 70 *j);
				}
			}

		} else if (btl->common_flag3_ == 1) {//第3波
			for (int i = 0; i < 2; ++i) {
				for (int j = 0; j < 2; ++j) {
					call_monster_to_map(m_map, btl, 11316, 600 + 70 * i, 270 + 70 *j);
				}
			}
			call_monster_to_map(m_map, btl, 11494, 600, 390);//boss
		}
		btl->common_flag3_ ++;
	}	

	if (btl->common_flag1_  && btl->common_flag1_ + 10 < cur_time) { //镜子1, 旋转时间到停止旋转
		uint32_t mechanism_id =	m_map->get_mechanism_id_by_type(35000);
		m_map->mechansim_status_changed(mechanism_id, 0, 0);
		btl->common_flag1_ = 0;
	} 	

	if (btl->common_flag2_ && btl->common_flag2_ + 10 < cur_time) { //镜子2, 旋转时间到停止旋转
		uint32_t mechanism_id = m_map->get_mechanism_id_by_type(34999);
		m_map->mechansim_status_changed(mechanism_id, 0, 0);
		btl->common_flag2_ = 0;
	}

	btl->battle_section_data++;
}

void process_battle_985_logic(Battle * btl, struct timeval next_tm)
{
	uint32_t map_id = 1098501;//场景地图ID
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (!m_map) {
		return;
	}	
	Player * yaoshi = m_map->get_one_monster(13131);
	if (!yaoshi || yaoshi->hp < 10) {
		btl->on_btl_over(0);
	}
}

void process_battle_986_logic(Battle * btl, struct timeval next_tm)
{
	uint32_t map_id = 1098601;//场景地图ID
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (!m_map) {
		return;
	}	

	uint32_t cur_time = get_now_tv()->tv_sec;

	Player * player = m_map->get_one_player();
	if (!btl->common_flag1_) {	
		if (player && (player->summon_info.mon_type / 10) == 100 )  { 
			call_monster_to_map(m_map, btl, 13106, 600, 400);
			btl->common_flag1_ = 13106;
		} else {
			call_monster_to_map(m_map, btl, 13117, 600, 400);
			btl->common_flag1_ = 13117;
		}
	}

	Player * summon  = m_map->get_one_monster(btl->common_flag1_);
	uint32_t to_tm = cur_time - btl->btl_start_tm;

	if ((!summon || (to_tm >= 90)) //杀死宠物 或者 超过 90s 没有杀死
		   	&& !btl->common_flag3_) { 
		btl->common_flag3_ = 1;
		call_monster_to_map(m_map, btl, 13118, 600, 270);
		call_monster_to_map(m_map, btl, 13119, 400, 370);
		call_monster_to_map(m_map, btl, 13116, 800, 270);
		call_monster_to_map(m_map, btl, 13111, 1000, 570);
		call_monster_to_map(m_map, btl, 13108, 800, 470);
	}

	if (summon && (to_tm >= 60 && to_tm < 90)) { // 60 到 90 s 将宠物达到10%
		if (summon->max_hp() / summon->hp >= 10) {
			btl->on_btl_over(0);
		}
	}

	if (!m_map->get_monster_cnt()){
		btl->on_btl_over(0);
	}
}

void process_battle_988_logic(Battle * btl, struct timeval next_tm)
{
	uint32_t map_id = 1098801;//场景地图ID
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (!m_map) {
		return;
	}	

	Player * boss = m_map->get_one_monster(13137);
	if (boss) {
		if (!btl->common_flag1_) {
			btl->common_flag1_ = m_map->get_monster_cnt();
			btl->common_flag3_ = m_map->get_monster_cnt();
			btl->common_flag2_ = boss->atk;
		} else if (btl->common_flag1_ > m_map->get_monster_cnt() && btl->common_flag3_ > m_map->get_monster_cnt()) {
			int atk_cnt = btl->common_flag1_ - m_map->get_monster_cnt();
			//int num_cnt = btl->common_flag3_ - m_map->get_monster_cnt();
//			boss->hp += (num_cnt * boss->hp * 3) / 10  ;
//			boss->maxhp += num_cnt * boss->maxhp;
			boss->atk = btl->common_flag2_ * (atk_cnt / 2);  
			boss->noti_hpmp_to_btl();
			btl->common_flag3_ = m_map->get_monster_cnt();
		}
	}

}
//风龙逻辑
void process_battle_989_logic(Battle * btl, struct timeval next_tm)
{
	uint32_t map_id = 1098901;//场景地图ID
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (!m_map) {
		return;
	}	

	for (PlayerSet::iterator it = m_map->players.begin(); it != m_map->players.end();
			++ it) {
		Player * player = *(it);
		if (!player->is_dead() && !is_player_buff_exist(player, 1319)) {
			add_player_buff(player, 1319, 0);
		}
		if (player->my_summon && !player->my_summon->is_dead() 
				&& !is_player_buff_exist(player->my_summon, 1319)) {
			add_player_buff(player->my_summon, 1319, 0);
		}
	}

}
//土龙逻辑
void process_battle_990_logic(Battle * btl, struct timeval next_tm)
{
	uint32_t map_id = 1099801;//场景地图ID
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (!m_map) {
		return;
	}	
	uint32_t cur_time = get_now_tv()->tv_sec;
	if (btl->common_flag1_ == 0) {
		btl->common_flag1_ = cur_time;
		return;
	//call map summon per 10 seconds
	} else if (btl->common_flag1_ + 5 < cur_time) {

		Player * boss = m_map->get_one_monster(13145);
		if (boss) {
			static int ptr[5] = {1, 2, 3, 4, 5};
			static int pos_x[5] = {200, 600, 1000, 1400, 1800};
			static int pos_y[5] = {500, 400, 600, 400, 500};
			ran_pos(ptr, 5, 3);
			for (int i = 0; i < 3; i++) {
				boss->call_map_summon(39, pos_x[(ptr[i])],  pos_y[(ptr[i])], false);
			}
		}
		btl->common_flag1_ = cur_time;
	}

}
//雷龙关卡
void process_battle_991_logic(Battle * btl, struct timeval next_tm)
{
	uint32_t map_id = 1099101;//场景地图ID
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (!m_map) {
		return;
	}	
	uint32_t cur_time = get_now_tv()->tv_sec;
	if (btl->common_flag1_ == 0) {
		btl->common_flag1_ = cur_time;
		return;
	//call map summon per 10 seconds
	} else if (btl->common_flag1_ + 10 < cur_time) {
			Player * player = m_map->get_one_player();
			uint32_t pos_x = 0;
			uint32_t pos_y = 0;
			Player * boss = m_map->get_one_monster(13146);
			if (!boss) return;

		
			if (player && !player->is_dead()) {
				pos_x = player->pos().x();	
				pos_y = player->pos().y();
				boss->call_map_summon(48, pos_x, pos_y, false);
			} 

			if (player && player->my_summon && !player->my_summon->is_dead()) {
				pos_x = player->my_summon->pos().x();
				pos_y = player->my_summon->pos().y();
				boss->call_map_summon(48, pos_x, pos_y, false);
			} 

			boss->call_map_summon(40, 860, 660, false);
			boss->call_map_summon(41, 340, 576, false);
			boss->call_map_summon(41, 477, 680, false);
			boss->call_map_summon(42, 1650, 1570, false);
			boss->call_map_summon(43, 1165, 390, false);
			boss->call_map_summon(44, 240, 380, false);
			boss->call_map_summon(45, 1150, 400, false);
			boss->call_map_summon(46, 758, 424, false);
			boss->call_map_summon(47, 1280, 600, false);
			btl->common_flag1_ = cur_time;
	}
}
//冰龙
void process_battle_993_logic(Battle * btl, struct timeval next_tm)
{
	uint32_t map_id = 1099301;//场景地图ID
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (!m_map) {
		return;
	}	

	Player *p = m_map->get_one_player();
	if (!p) {
		return;
	}
	
	uint32_t cur_time = get_now_tv()->tv_sec;
	if (btl->common_flag2_ == 0) {
		btl->common_flag2_ = cur_time;
		btl->common_flag1_ = 100;
		btl->battle_extra_logic_msg(p);
		return;
	} 

	if (btl->common_flag1_ <= 70) {
		Player * mechanism = m_map->get_mechanism_by_type(34997);
		if (mechanism && mechanism->mp > 1) {
			m_map->mechansim_status_changed(mechanism->id, 0, 0);
		}
	}

	if (btl->common_flag1_ == 0) {
		uint32_t buff_id = 1315;
		add_player_buff(p, buff_id, 0);
		return;
	}

	if (cur_time - btl->common_flag2_ > 1) {
		btl->common_flag1_ -= 5;
		btl->common_flag2_ = cur_time;
		btl->battle_extra_logic_msg(p);
		return;
	}
	
}

int get_dici_pos_x(int idx)
{
	if ( idx < 8) {
		return 200 * (idx + 1);
	} else if (idx < 17) {
		return 200 * (idx - 9) + 135;
	} else {
		return 75 + 200 * (idx - 18);
	}
}
int get_dici_pos_y(int idx)
{
	if ( idx < 8) {
		return 320;
	} else if (idx < 17) {
		return 470;
	} else {
		return 600;
	}

}

int get_yunshi_pos_x(int idx)
{
	if (idx < 8) {
		return 140 + idx * 200;
	} else {
		return 120 + (idx - 8) * 200;
	}
}

int get_yunshi_pos_y(int idx)
{
	if (idx < 8) {
		return 400;
	} else {
		return 550;
	}
}
void process_battle_995_logic(Battle * btl, struct timeval next_tm)
{
  uint32_t map_id = 1099501;//场景地图ID
  map_id = get_stage_map_id(map_id);
  map_t * m_map = btl->get_map(map_id);
  if (!m_map) {
    return;
  }
  static int pos_idx_1[27] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	9, 10, 11, 12, 13, 14, 15, 16, 17,
	18, 19, 20, 21, 22, 23, 24, 25, 26
  }; 


  uint32_t cur_time = get_now_tv()->tv_sec;
  if (btl->common_flag1_ == 0) {
	btl->common_flag1_ = cur_time;
	btl->common_flag2_ = cur_time;
	return;
  }

  Player * boss = m_map->get_one_monster(13150);
  if (!boss) {
	  return;
  }

 int dici_num = 16;
 int yunshi_num = 16; 

 if (btl->common_flag1_ + 4 < cur_time) {
	 if (((cur_time - btl->common_flag2_) / 4) % 2 == 0) {
		 ran_pos(pos_idx_1, 27, dici_num);
		 for (int i = 0; i < dici_num; i++) 
		 {
			 int idx = pos_idx_1[i];
			 int pos_x = get_dici_pos_x(idx);
			 int pos_y = get_dici_pos_y(idx);
			 boss->call_map_summon(49, pos_x, pos_y,  false);
		 }
	 } else {
		 ran_pos(pos_idx_1, 27, yunshi_num);
		 for (int i = 0; i < yunshi_num; i++) {
			 int idx = pos_idx_1[i];
			 int pos_x = get_dici_pos_x(idx);
			 int pos_y = get_dici_pos_y(idx);
			 boss->call_map_summon(50, pos_x, pos_y,  false);
		 }
	 }
	 btl->common_flag1_ = cur_time;
 }
 
}


void process_battle_996_logic(Battle * btl, struct timeval next_tm)
{
  uint32_t map_id = 1099601;//场景地图ID
  map_id = get_stage_map_id(map_id);
  map_t * m_map = btl->get_map(map_id);
  if (!m_map) {
    return;
  }

  Player * boss = m_map->get_one_monster(13151);
  if (boss && !boss->is_dead()) {
	  if (is_player_buff_exist(boss, 1322)) {
		  for (PlayerSet::iterator it = m_map->players.begin(); it != m_map->players.end();
				  ++it) {
			  Player * p = *it;
			  if (p && !p->is_dead() && 
					  is_player_buff_exist(boss, 1322)) {
				  int hp = (int)(p->hp * 0.1);
				  player_hpmp_chg_info_t info;
				  info.hp_chg = hp;
				  boss->hpmp_chg(info);
				  boss->noti_hpmp_to_btl();
				  p->hp = (int)(p->hp * 0.8);
				  p->noti_hpmp_to_btl();
			  }
		  }
	  }
  }
 
}

void process_battle_997_logic(Battle * btl, struct timeval next_tm)
{
  uint32_t map_id = 1099701;//场景地图ID
  map_id = get_stage_map_id(map_id);
  map_t * m_map = btl->get_map(map_id);
  if (!m_map) {
    return;
  }

  Player * boss = m_map->get_one_monster(13152);
  if (boss->hp *100 / boss->max_hp() < 30) {
//	ERROR_LOG("BOSS HP %u %u", boss->hp, boss->max_hp());
	for (PlayerSet::iterator it = m_map->barriers.begin(); it != m_map->barriers.end(); ++it) { 
		Player * shizhu = *it;
		if (shizhu && !shizhu->is_dead()) {
//			ERROR_LOG("SHI ZHU SUICIDE()");
			shizhu->suicide();
			return;
		}
	}
  }
}

void process_battle_702_logic(Battle * btl, struct timeval next_tm)
{
	Player* p = btl->get_one_player();
	if (p) {

		uint32_t cur_time = get_now_tv()->tv_sec;
		if (btl->common_flag1_ == 0) {
			btl->common_flag1_ = cur_time;
			uint32_t map_id = 1070201;
			map_id = get_stage_map_id(map_id);
			map_t * m_map = btl->get_map(map_id);
			if (!m_map) {
				return;
			}
			Player* boss = m_map->get_one_monster(13155);
			if (boss) {
				add_player_buff(boss, 1323, 0, 6000, false);
			}
			return;
		} else if (btl->common_flag1_ + 6 < cur_time) {
			btl->common_flag1_ = cur_time;

			uint32_t map_id = 1070201;
			map_id = get_stage_map_id(map_id);
			map_t * m_map = btl->get_map(map_id);
			if (!m_map) {
				return;
			}
			if (m_map->get_dead_pre_barriers_cnt(30017) > 10) {
				return;
			}
			uint32_t x = ranged_random(265, 640);
			uint32_t y = ranged_random(250, 470);
			p->call_monster(30017, x, y);

			x = ranged_random(265, 640);
			y = ranged_random(250, 470);
			p->call_monster(30017, x, y);
		}
	}
}
	
void process_battle_703_logic(Battle * btl, struct timeval next_tm)
{
	uint32_t map_id = 1070301;
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (!m_map) {
		return;
	}
	if (!(m_map->get_monster_cnt(11513))){
		Player* p = btl->get_one_player();
		if (p && !(m_map->get_monster_cnt(13154))) {
			p->call_monster(13154, p->pos().x(), p->pos().y());
		}
	}
}

void process_battle_701_room_1_logic(Battle *btl, struct timeval next_tm)
{
	uint32_t now_time = get_now_tv()->tv_sec;
	if (!(btl->common_flag5_)) {
		btl->common_flag5_ = now_time;
	}
	if (!(btl->common_flag6_)) {
		btl->common_flag6_ = 1;
	}
	if (btl->common_flag5_ + 5 < now_time && !(btl->common_flag4_)) {
	// in 
		Player* p = btl->get_one_player();
		Player* boss = p->cur_map->get_one_monster(13139);
		if (boss) {
			btl->common_flag4_ = now_time;
			uint32_t tmp_n = rand() % 2;
			if (tmp_n) {
				boss->teleport(300, 210);
			} else {
				boss->teleport(800, 430);
			}
			boss->i_ai->reset();
			boss->i_ai->change_state(WAIT_STATE);
		}
	}
	if (!(btl->common_flag4_)) {
		return;
	}

	uint32_t map_id = 1070101;//场景地图ID
	map_id = get_stage_map_id(map_id);
	map_t * m_map = btl->get_map(map_id);
	if (!m_map) {
		return;
	}

	static int x_x[20] = {232, 360, 480, 595, 720,
						  200, 345, 480, 611, 752,
						  160, 320, 480, 633, 793,
						  125, 297, 480, 653, 828};
	static int y_y[20] = {270, 270, 270, 270, 270,
						  343, 343, 343, 343, 343,
						  422, 422, 422, 422, 422,
						  505, 505, 505, 505, 505};


	if (btl->common_flag1_ >= 3) {
		btl->common_flag4_ = 0;
		btl->common_flag1_ = 0;
		btl->common_flag5_ = now_time;
		return;
	}

	uint32_t cur_time = get_now_tv()->tv_sec;
	if (btl->common_flag3_ == 0 || btl->common_flag3_ + 6 <= cur_time) {
		btl->common_flag3_ = cur_time + 1;
		btl->common_flag1_ += 1;
		btl->battle_section_data = 0;	
		if (!m_map->pre_barriers.empty()) {

			for (PlayerSet::iterator it = m_map->pre_barriers.begin(); it != m_map->pre_barriers.end();
				++it) {
				Player * p = (*it);
				notify_delete_player_obj(p);
				delete p;
			}
			m_map->pre_barriers.clear();
		}
		return;
	}

	if (cur_time >= btl->common_flag3_ && cur_time < btl->common_flag3_ + 1) {
		if (btl->common_flag2_ == 0) {
			//Just select point
			int pos[20] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
			ran_pos(pos, 20, 13);
			for (uint32_t i = 0; i < 6; i++) { //13
				btl->common_flag2_ = set_bit_on32(btl->common_flag2_, pos[i] + 1);
			}	

			Player *p = m_map->get_one_player();
			if (p) {
				btl->battle_extra_logic_msg(p);
			}
		}
	} else if ( cur_time >= btl->common_flag3_ + 1 && cur_time < btl->common_flag3_ + 2) {
		if (btl->common_flag2_ != 0){
			//Call summon monster
			int cnt = 0;
			for (uint32_t i = 1; i < 21; i++) {
				if (test_bit_on32(btl->common_flag2_, i)) {
					if (cnt++ == 6) {
						call_monster_to_map(m_map, btl, 35002, x_x[i-1], y_y[i-1]);
					} else {
						call_monster_to_map(m_map, btl, 35002, x_x[i-1], y_y[i-1]);
					}
				}
			}
			btl->common_flag2_ = 0;
		}
	} else {
		if (btl->battle_section_data == 0 && !m_map->pre_barriers.empty()) {
			for (PlayerSet::iterator it = m_map->pre_barriers.begin(); it != m_map->pre_barriers.end(); ++it) {
				Player * p = (*it);
				p->call_map_summon(29, p->pos().x(), p->pos().y() - 30, false);
				btl->battle_section_data = 1;
			}
		}
	}
	return;

}

void process_battle_701_room_2_logic(Battle *btl, struct timeval next_tm)
{
}

void process_battle_701_room_3_logic(Battle *btl, struct timeval next_tm)
{
	Player * p = btl->get_one_player();
	Player * stone = p->cur_map->get_one_barriers(19165);
	if (!stone || stone->is_dead()) {
		if (!p->cur_map->get_one_monster(13142) 
				&& !p->cur_map->get_one_monster(13143)) {
			btl->on_btl_over(0);
		}
	}
}


//天外飞石
void process_battle_701_logic(Battle *btl, struct timeval next_tm)
{
	Player* p = btl->get_one_player();
	if (p && p->cur_map ) {
		if (p->cur_map->id == 1070101) {
			return process_battle_701_room_1_logic(btl, next_tm);
		} else if (p->cur_map->id == 1070102) {
			return process_battle_701_room_2_logic(btl, next_tm);
		} else if (p->cur_map->id == 1070103) {
			return process_battle_701_room_3_logic(btl, next_tm);
		}
	}
}


void process_battle_964_logic(Battle * btl, struct timeval next_tm)
{
	if (btl->players_.size() == 2) {
		Player * player_1 = btl->get_one_player();
		const Player * player_2 = btl->get_one_other_player(player_1);
		assert(player_1 != NULL && player_2 != NULL);
		if (btl->common_flag3_ == 0) {
			static int monster_id[5] = {11432, 11416, 11437, 11431, 11430};	
			int idx = (player_1->role_tm + player_2->role_tm) % 10;
			if (idx < 5) {
				call_monster_to_map(player_1->cur_map, player_1->btl, monster_id[idx], player_1->pos().x() + 300 , player_1->pos().y());
				btl->common_flag3_ = monster_id[idx]; 
			} else {
				btl->common_flag3_ = get_now_tv()->tv_sec;
			}

		} 

		if (btl->common_flag2_ == 0) {
			btl->common_flag2_ = get_now_tv()->tv_sec;
		} else if (btl->common_flag2_ + 300 < get_now_tv()->tv_sec) {
			double x_1 = (double)player_1->hp / (double) player_1->max_hp(); 
			double x_2 = (double)player_2->hp / (double) player_2->max_hp();
			if (x_1 >= x_2) {
				notify_pvp_over_2_switch(btl->id(), player_1->id, pvp_16_contest, btl->common_flag1_);
			} else {
				notify_pvp_over_2_switch(btl->id(), player_2->id, pvp_16_contest, btl->common_flag1_);
			}
		}
	}

}

void process_battle_705_logic(Battle * btl, struct timeval next_tm)
{
	Player* p = btl->get_one_player();
	if (p && (is_player_buff_exist(p, 1215) && is_player_buff_exist(p, 1216))) {
		p->suicide();
	}
}

void process_battle_719_logic(Battle * btl, struct timeval next_tm)
{
    //ERROR_LOG(" ------------------------------------");
    uint32_t map_id = 1071901;//场景地图ID
    map_id = get_stage_map_id(map_id);
    map_t * m_map = btl->get_map(map_id);
    if (!m_map) {
        return;
    }

    static uint32_t pos[5][2] = {{471, 379}, {466, 379}, {476, 379}, {471, 374}, {471, 384}};
    Player * boss = m_map->get_one_monster(11534);
    if (boss) {
        if ( boss->hp * 100 < boss->max_hp() * 15) {
            //ERROR_LOG(" 111 --> hp[%u/%u]", boss->hp, boss->max_hp());
            //boss->teleport(1500, 300);
            add_player_buff(boss, 1337, 0);
            if (btl->common_flag1_ == 0) {
                btl->common_flag1_ = boss->hp;
                boss->monster_speaks(1);
            }
            if (btl->common_flag2_ == 0) {
                btl->common_flag2_ = get_now_tv()->tv_sec + 10;
            }
            if (btl->common_flag3_ == 0 && boss->hp < btl->common_flag1_) {
                //ERROR_LOG(" 222 --> hp[%u/%u]", boss->hp, boss->max_hp());
                for (uint32_t i = 0; i < 5; i++) {
                    call_monster_to_map(boss->cur_map, boss->btl, 11535, pos[i][0], pos[i][1]);
                }
                boss->suicide();
                btl->common_flag2_ = 0;
                btl->common_flag3_ = 1;
            } else {
                if (get_now_tv()->tv_sec > btl->common_flag2_) {
                    //ERROR_LOG(" 333 --> hp[%u/%u]", boss->hp, boss->max_hp());
                    btl->on_btl_over(0);
                }
            }
        }
    } else {
        if (btl->common_flag3_ == 0) {
            for (uint32_t i = 0; i < 5; i++) {
                call_monster_to_map(m_map, btl, 11535, pos[i][0], pos[i][1]);
            }
            btl->common_flag3_ = 1;
        }
    }

    if (m_map->get_monster_cnt() == 0) {
        btl->on_btl_over(0);
    }
}

void process_battle_721_logic(Battle * btl, struct timeval next_tm)
{
    //ERROR_LOG(" ------------------------------------");
    uint32_t map_id = 1072101;//场景地图ID
    map_id = get_stage_map_id(map_id);
    map_t * m_map = btl->get_map(map_id);
    if (!m_map) {
        return;
    }

    if (btl->common_flag4_ + 3 > get_now_tv()->tv_sec) {
        return;
    }

    static uint32_t boss[6] = {11536, 11537, 11538, 11539, 11540, 11541};

    Player * monster = m_map->get_one_monster(boss[btl->common_flag2_]);
    if (!monster) {
        if (btl->common_flag2_ == 6) {
            btl->on_btl_over(0);
        }

        if (btl->common_flag2_ < 5) {
            btl->common_flag2_++;
            btl->common_flag5_ = 0;
            btl->common_flag4_ = get_now_tv()->tv_sec;
            //ERROR_LOG(" ------ %u %u", btl->common_flag2_, btl->common_flag5_);
        }
        if (m_map->get_monster_cnt() == 0) {
            btl->common_flag2_++;
        }
        return;
    } 

    //ERROR_LOG(" ====== %u %u", btl->common_flag2_, btl->common_flag5_);
    if (btl->common_flag1_ == 0) {
        btl->common_flag1_ = get_now_tv()->tv_sec;
        for (uint32_t i = 0; i < 6; i++) {
            Player * m = m_map->get_one_monster(boss[i]);
            if (m) {
                add_player_buff(m, 28, 0);
                add_player_buff(m, 1337, 0);
            }
        }
        monster->monster_speaks(1);
    }

    if (btl->common_flag1_ + 3 < get_now_tv()->tv_sec ) {
        if (btl->common_flag3_ == 0) {
            for (uint32_t i = 1; i < 6; i++) {
                Player * m = m_map->get_one_monster(boss[i]);
                if (m) {
                    m->teleport(2000, 2000);
                }
            }
            btl->common_flag3_ = 1;
            del_player_buff(monster, 28);
            del_player_buff(monster, 1337);
        }
        //return;
    }

    if (btl->common_flag5_ == 0 && btl->common_flag2_ != 0) {
        monster->teleport(930, 400);
        del_player_buff(monster, 28);
        del_player_buff(monster, 1337);
        monster->monster_speaks(1);
        btl->common_flag5_ = 1;
        if (monster->role_type == 11541) {
            call_monster_to_map(monster->cur_map, monster->btl, 11542, 950, 450);
        }
        //ERROR_LOG("cccccc %u pos[%u, %u]", monster->role_type, monster->pos().x(), monster->pos().y());
    }
}

void call_lamp_monster(Battle * btl, uint32_t section_cnt) 
{
	if (section_cnt > 50 || !section_cnt) {
		return;
	}
	Player * player_1 = btl->get_one_player();
	static int monster_id[10] = {11523, 11524, 11525, 11526, 11527, 11528, 11529, 11530, 11531, 11532};	
	static int boss_id[10] = {13165, 13166, 13167, 13168, 13169, 13170, 13171, 13172, 13173, 13174};

	if (section_cnt % 5 == 0) {
		call_monster_to_map(player_1->cur_map, player_1->btl, boss_id[section_cnt / 5 - 1], 1295 , 600);	
	}

	uint32_t tmp_rand = rand() % 3;

	uint32_t tmp_idx = (section_cnt - 1) / 5;

//	uint32_t add_cnt = (section_cnt - 1) / 10;
	if (tmp_rand == 0) {
		call_monster_to_map(player_1->cur_map, player_1->btl, monster_id[tmp_idx], 2385 , 273);	
		call_monster_to_map(player_1->cur_map, player_1->btl, monster_id[tmp_idx], 2410 , 284);	
		call_monster_to_map(player_1->cur_map, player_1->btl, monster_id[tmp_idx], 2420 , 294);	
		call_monster_to_map(player_1->cur_map, player_1->btl, monster_id[tmp_idx], 2430 , 304);	

//		for (uint32_t i = 0; i < add_cnt; i++) {
//			call_monster_to_map(player_1->cur_map, player_1->btl, monster_id[tmp_idx], 2430 , 304);	
//		}
	} else if (tmp_rand == 1) {
		call_monster_to_map(player_1->cur_map, player_1->btl, monster_id[tmp_idx], 205 , 485);	
		call_monster_to_map(player_1->cur_map, player_1->btl, monster_id[tmp_idx], 205 , 500);	
		call_monster_to_map(player_1->cur_map, player_1->btl, monster_id[tmp_idx], 205 , 515);	
		call_monster_to_map(player_1->cur_map, player_1->btl, monster_id[tmp_idx], 205 , 530);	
//		for (uint32_t i = 0; i < add_cnt; i++) {
//			call_monster_to_map(player_1->cur_map, player_1->btl, monster_id[tmp_idx], 205 , 530);	
//		}
	} else {
		call_monster_to_map(player_1->cur_map, player_1->btl, monster_id[tmp_idx], 2385 , 273);	
		call_monster_to_map(player_1->cur_map, player_1->btl, monster_id[tmp_idx], 2410 , 284);	
		call_monster_to_map(player_1->cur_map, player_1->btl, monster_id[tmp_idx], 2420 , 294);	
//		call_monster_to_map(player_1->cur_map, player_1->btl, monster_id[tmp_idx], 205 , 485);	
		call_monster_to_map(player_1->cur_map, player_1->btl, monster_id[tmp_idx], 205 , 500);
		call_monster_to_map(player_1->cur_map, player_1->btl, monster_id[tmp_idx], 205 , 515);
//		for (uint32_t i = 0; i < add_cnt; i++) {
//			call_monster_to_map(player_1->cur_map, player_1->btl, monster_id[tmp_idx], 2430 , 304);	
//		}
	}
	
	
}

void process_battle_718_logic(Battle * btl, struct timeval next_tm)
{
	//btl->common_flag1_ : section number
	//btl->common_flag2_ : time stamp
	//btl->common_flag3_ : section started

	//failed 
	Player * player_1 = btl->get_one_player();
	if (!player_1) {
		return;
	}
	Player* lamp_mon = player_1->cur_map->get_one_monster(11533);
	if (!lamp_mon || lamp_mon->hp <=0 ) {
		btl->on_btl_over(player_1, 1);
		return;
	}
	//
	uint32_t now_tm = get_now_tv()->tv_sec;
	if (btl->common_flag1_ == 0) {
		if (!(btl->common_flag2_)) {
			btl->common_flag2_ = now_tm;
			add_player_aura(lamp_mon, 26, 0);
			btl->battle_extra_logic_msg(player_1);
			TRACE_LOG("TEST NEXT SECTION READY %u", btl->common_flag1_);
		} else if (btl->common_flag3_ == 0) {
			if (now_tm - btl->common_flag2_ > 2) {
				btl->common_flag3_ = 1;
				btl->common_flag1_ ++;
				btl->common_flag2_ = now_tm;
				//call monsters
				TRACE_LOG("TEST NEXT SECTION START %u", btl->common_flag1_);
				call_lamp_monster(btl, btl->common_flag1_);
			}
		}
	} else if (btl->common_flag1_ > 0) {
		if (btl->common_flag3_) {
		// section started
			if (btl->common_flag1_ == 50) {
				if (player_1->cur_map->get_monster_cnt() == 1) {
				//successed
					btl->common_flag1_ ++;
					btl->on_btl_over(player_1);
					return;
				} else {
					return;
				}
			}
			if (now_tm - btl->common_flag2_ > 30 || player_1->cur_map->get_monster_cnt() == 1) {
			//if the section lasted 10 sec or all monsters be killed,  start next section, 
				btl->common_flag3_ = 0;
				btl->common_flag2_ = now_tm;
				btl->battle_extra_logic_msg(player_1);
				btl->common_flag1_ ++;
				TRACE_LOG("TEST NEXT SECTION READY %u", btl->common_flag1_);
			}
		} else {
		// section not started
			if (now_tm - btl->common_flag2_ > 2) {
			//call monsters and start section
				TRACE_LOG("TEST NEXT SECTION START %u", btl->common_flag1_);
				btl->common_flag3_ = 1;
				btl->common_flag2_ = now_tm;
				call_lamp_monster(btl, btl->common_flag1_);
			}
		}
	}
}

void process_battle_734_logic(Battle * btl, struct timeval next_tm) 
{
	if (btl->pvp_lv() == pvp_dragon_ship) {
		//failed 
		Player * player_1 = btl->get_one_player();

		if (player_1 && player_1->my_summon) {
			if (player_1->pos().x() + 10 >= player_1->my_summon->pos().x() &&
					player_1->my_summon->i_ai->common_flag1_) {
				end_pvp_on_player_dead(btl, player_1, player_1->team);
					player_1->my_summon->i_ai->common_flag1_ = 0;
				return;
			}
		}

		Player* player_2 =  btl->get_one_other_player(player_1);
		if (player_2 && player_2->my_summon) {
			if (player_2->pos().x() + 10 >= player_2->my_summon->pos().x() && 
					player_2->my_summon->i_ai->common_flag1_) {
				player_2->my_summon->i_ai->common_flag1_ = 0;
				end_pvp_on_player_dead(btl, player_2, player_2->team);
				return;
			}
		}
	}
}
