/*
 * =====================================================================================
 *
 *       Filename:  final_boss_2011.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/15/2011 03:02:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include "central_online.h"
#include "final_boss_2011.h"

#define FB_BOSS_MAX_HP			20000
#define FB_BOSS_UPDATE_TIME		120
#define FB_BOSS_ATTACK_TIME		180
#define FB_NOTIFY_BOSS_HP_TIME	5
#define FB_BOMB_TURRET_SIZE		4
#define FB_BOSS_ANI_TIME		9
#define ITEM_ID_BLUE_BOMB		190876
#define ITEM_ID_BOSS_AWARD		190877

static int			boss_hp;
static list_head_t	timer_list;

static sprite_t*	magic_turret[WT_WATER_BOMB];
static sprite_t*	bomb_turret[FB_BOMB_TURRET_SIZE];
static int			bomb_member_size;

static timer_struct_t*	timer_boss_update;
static timer_struct_t*	timer_boss_attack;
static timer_struct_t*	timer_notify_boss_hp;
static timer_struct_t*	timer_boss_atk_ani;
static int is_in_boss_atk_ani;
static int ani_begin_time;

static weapon_info_t fb_weapon_info[WT_MAX] = {	{150,5,NULL}, {150,5,NULL}, {300,5,NULL}, {150,5,NULL}, {40,2,NULL}, {1000,5,NULL} };

int fb_boss_attack(void* param1, void* param2);
int fb_boss_update(void* param1, void* param2);
int fb_notify_boss_hp(void* force, void* param2);
int fb_turret_event(void* turretID, void* param);
int fb_ani_over_event(void* param1, void* param2);


inline static int is_boss_alive()
{
	if( boss_hp > 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int fb_initialize()
{
	memset( magic_turret, 0, sizeof(magic_turret) );
	memset( bomb_turret, 0, sizeof(bomb_turret) );
	bomb_member_size = 0;
	boss_hp = FB_BOSS_MAX_HP;
	INIT_LIST_HEAD(&timer_list);
	is_in_boss_atk_ani = 0;
	timer_boss_update = NULL;
	timer_boss_atk_ani = NULL;
	//begin timer to attack
	timer_boss_attack = add_event( &timer_list, fb_boss_attack , NULL, NULL,
					get_now_tv()->tv_sec + FB_BOSS_ATTACK_TIME, 0 );
	//begin timer to notify boss HP
	timer_notify_boss_hp = add_event( &timer_list, fb_notify_boss_hp , NULL, NULL,
					get_now_tv()->tv_sec + FB_NOTIFY_BOSS_HP_TIME, 0 );
	DEBUG_LOG("Final Boss 2011 Initialize OK");
	return 0;
}

int fb_boss_update(void* param1, void* param2)
{
	DEBUG_LOG("Final Boss Update");
	//reset HP
	boss_hp = FB_BOSS_MAX_HP;
	is_in_boss_atk_ani = 0;
	timer_boss_update = NULL;
	timer_boss_atk_ani = NULL;
	//begin timer to attack
	timer_boss_attack = add_event( &timer_list, fb_boss_attack , NULL, NULL,
					get_now_tv()->tv_sec + FB_BOSS_ATTACK_TIME, ADD_EVENT_REPLACE_UNCONDITIONALLY );
	int len = sizeof(protocol_t);
	init_proto_head( msg, PROTO_FB_NOTIFY_BOSS_REVIVE, len );
	send_to_map3( FB_MAP_ID, msg, len );
	//begin timer to notify boss HP
	timer_notify_boss_hp = add_event( &timer_list, fb_notify_boss_hp , NULL, NULL,
		get_now_tv()->tv_sec + FB_NOTIFY_BOSS_HP_TIME, 0 );
	return 0;
}

static void attack_boss(int damage)
{
	boss_hp -= damage;
	if( boss_hp < 0 )
	{
		boss_hp = 0;
	}
	if( 0 == boss_hp )
	{
		timer_boss_update = add_event( &timer_list, fb_boss_update , NULL, NULL,
			get_now_tv()->tv_sec + FB_BOSS_UPDATE_TIME, 0 );
		int* force = malloc(sizeof(int));
		*force = 1;
		fb_notify_boss_hp( force, NULL );
		int len = sizeof(protocol_t);
		init_proto_head( msg, PROTO_FB_NOTIFY_BOSS_DIE, len );
		send_to_map3( FB_MAP_ID, msg, len );
		len = 0;
		uint8_t buff[1024];
		PKG_H_UINT32( buff, 0, len );
		PKG_H_UINT32( buff, 1, len );
		PKG_H_UINT32( buff, 0, len );
		PKG_H_UINT32( buff, 0, len );

		int itemType = 0;
		PKG_H_UINT32( buff, itemType, len );
		PKG_H_UINT32( buff, ITEM_ID_BOSS_AWARD, len );
		PKG_H_UINT32( buff, 5, len );
		PKG_H_UINT32( buff, 99999, len );

		const map_t* tile = get_map(FB_MAP_ID);
		list_head_t* p;
		list_for_each(p, &tile->sprite_list_head)
		{
			sprite_t* l = list_entry(p, sprite_t, map_list);
			if ( !IS_NPC_ID(l->id) )
			{
				send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, len, buff, l->id );
			}
		}
	}
}

int fb_notify_boss_hp(void* force, void* param2)
{
	if( !force && 0 == is_boss_alive() )
	{
		return 0;
	}

	free(force);

	int len = sizeof( protocol_t );
	PKG_UINT32( msg, boss_hp, len );
	init_proto_head( msg, PROTO_FB_NOTIFY_BOSS_HP, len );
	send_to_map3( FB_MAP_ID, msg, len );
	//begin timer to notify boss HP
	timer_notify_boss_hp = add_event( &timer_list, fb_notify_boss_hp , NULL, NULL,
		get_now_tv()->tv_sec + FB_NOTIFY_BOSS_HP_TIME, 0 );
	return 0;
}

int fb_boss_attack(void* param1, void* param2)
{
	if( 0 == is_boss_alive() )
	{
		return 0;
	}
	DEBUG_LOG( "BOSS ATTACK" );
	//set timer for attack ani
	timer_boss_atk_ani = add_event( &timer_list, fb_ani_over_event , NULL, NULL,
		get_now_tv()->tv_sec + FB_BOSS_ANI_TIME, 0 );
	is_in_boss_atk_ani = 1;
	ani_begin_time = get_now_tv()->tv_sec;
	//reset timer for attack
	DEBUG_LOG("Being New Attack Timer");
	timer_boss_attack = add_event( &timer_list, fb_boss_attack , NULL, NULL,
		get_now_tv()->tv_sec + FB_BOSS_ATTACK_TIME, ADD_EVENT_REPLACE_UNCONDITIONALLY );
	//boss攻击时所有玩家从炮台/魔法阵上掉落,并且不能进入攻击状态
	memset( magic_turret, 0, sizeof(magic_turret) );
	memset( bomb_turret, 0, sizeof(bomb_turret) );
	bomb_member_size = 0;
	int len = sizeof( protocol_t );
	init_proto_head( msg, PROTO_FB_NOTIFY_BOSS_ATTACK, len );
	send_to_map3( FB_MAP_ID, msg, len );
	return 0;
}

void fb_water_bomb_attack(sprite_t*p, uint32_t x, uint32_t y)
{
	if( !p ||							//玩家不存在
		is_boss_alive() == 0 ||			//Boss已经挂了
		( p->tiles && FB_MAP_ID != p->tiles->id ) ||	//不在Boss地图
		is_in_boss_atk_ani == 1 ||		//Boss在攻击状态
		x < 720 || x >800 ||
		y < 120 || y > 200
		)
	{
		return;
	}
	if( get_now_tv()->tv_sec - p->water_bomb_time > fb_weapon_info[WT_WATER_BOMB].cd )
	{
		attack_boss(fb_weapon_info[WT_WATER_BOMB].damage);
		p->water_bomb_time = get_now_tv()->tv_sec;
	}
}

static void blue_bomb_attack()
{
	//fire
	attack_boss(fb_weapon_info[WT_BLUE_BOMB].damage);
	//begin cold down
	int* weaponID = malloc(sizeof(int));
	*weaponID = WT_BLUE_BOMB;
	fb_weapon_info[WT_BLUE_BOMB].timer = add_event( &timer_list, fb_turret_event , weaponID, NULL,
		get_now_tv()->tv_sec + fb_weapon_info[WT_BLUE_BOMB].cd, 0 );
	//remove a blue bomb
	int len = 0;
	uint8_t buff[1024];
	PKG_H_UINT32( buff, 1, len );
	PKG_H_UINT32( buff, 0, len );
	PKG_H_UINT32( buff, 0, len );
	PKG_H_UINT32( buff, 0, len );
	int itemType = 0;
	PKG_H_UINT32( buff, itemType, len );
	PKG_H_UINT32( buff, ITEM_ID_BLUE_BOMB, len );
	PKG_H_UINT32( buff, 1, len );
	int i;
	for( i = 0; i < bomb_member_size; ++i )
	{
		send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, len, buff, bomb_turret[i]->id );
	}
	//remove players
	memset( bomb_turret, 0, sizeof(bomb_turret) );
	bomb_member_size = 0;
	//notify map
	len = sizeof(protocol_t);
	PKG_UINT32( msg, WT_BLUE_BOMB, len );
	PKG_UINT32( msg, fb_weapon_info[WT_BLUE_BOMB].damage, len );
	init_proto_head( msg, PROTO_FB_NOTIFY_ATTACK, len );
	send_to_map3( FB_MAP_ID, msg, len );
}
int fb_ani_over_event(void* param1, void* param2)
{
	is_in_boss_atk_ani = 0;
	timer_boss_atk_ani = NULL;
	return 0;
}

int fb_turret_event( void* turretID, void* param2 )
{
	int ID = *(int*)turretID;
	DEBUG_LOG( "Turret Eevent ID:[%d]", ID );
	free(turretID);
	if( ID > -1 && ID < WT_MAX )
	{
		if( ID == WT_BLUE_BOMB )
		{
			if( bomb_member_size == FB_BOMB_TURRET_SIZE )
			{
				blue_bomb_attack();
			}
			else
			{
				fb_weapon_info[ID].timer = NULL;
			}
		}
		else if( ID >= WT_MAGIC_ICE && ID <= WT_MAGIC_SPIRIT && magic_turret[ID] != NULL )
		{
			//fire
			attack_boss( fb_weapon_info[ID].damage );
			//remove player
			magic_turret[ID] = NULL;
			//clear timer
			fb_weapon_info[ID].timer = NULL;
			//notify map
			int len = sizeof(protocol_t);
			PKG_UINT32( msg, ID, len );
			PKG_UINT32( msg, fb_weapon_info[ID].damage, len );
			init_proto_head( msg, PROTO_FB_NOTIFY_ATTACK, len );
			send_to_map3( FB_MAP_ID, msg, len );
		}
	}
	return 0;
}

int fb_get_boss_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	if( p->tiles && FB_MAP_ID != p->tiles->id )
	{
		return send_to_self_error( p, p->waitcmd, ERR_fb_invalid_cmd, 1 );
	}
	int j = sizeof(protocol_t);
	PKG_UINT32( msg, boss_hp, j );
	PKG_UINT32( msg, is_in_boss_atk_ani, j );
	if( is_in_boss_atk_ani )
	{
		PKG_UINT32( msg, get_now_tv()->tv_sec - ani_begin_time , j );
	}
	else
	{
		PKG_UINT32( msg, (int)0 , j );
	}

	int i;
	for( i = WT_MAGIC_ICE; i <= WT_MAGIC_SPIRIT; ++i )
	{
		if( magic_turret[i] )
		{
			PKG_UINT32( msg, magic_turret[i]->id, j );
		}
		else
		{
			PKG_UINT32( msg, (int)0, j );
		}

	}
	for( i = 0; i <FB_BOMB_TURRET_SIZE; ++i )
	{
		if( bomb_turret[i] )
		{
			PKG_UINT32( msg, bomb_turret[i]->id, j );
		}
		else
		{
			PKG_UINT32( msg, (int)0, j );
		}
	}
	init_proto_head( msg, p->waitcmd, j );
	return send_to_self( p, msg, j, 1 );
}

int fb_enter_turret_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	if( p->tiles && FB_MAP_ID != p->tiles->id )
	{
		return send_to_self_error( p, p->waitcmd, ERR_fb_invalid_cmd, 1 );
	}
	if( 0 == is_boss_alive() )
	{
		return send_to_self_error( p, p->waitcmd, ERR_fb_boss_not_alive, 1 );
	}
	if( 1 == is_in_boss_atk_ani )
	{
		return send_to_self_error( p, p->waitcmd, ERR_fb_invalid_cmd, 1 );
	}
	int clientTurret;
	CHECK_BODY_LEN( len, sizeof(clientTurret) );
	int j = 0;
	UNPKG_UINT32( body, clientTurret, j );
	DEBUG_LOG( "Player Enter Turret BOSS HP:[%d], Clietn TurretID:[%d]", boss_hp, clientTurret );
	int isEnter = 0;
	int turretID;
	if( 0 == clientTurret )
	{
		turretID = p->team_id - 1;
		if( turretID < 4 && turretID >= 0  && magic_turret[turretID] == NULL )
		{
			magic_turret[turretID] = p;
			//begin a Timer to Attack Boss
			int* ptrTurret = malloc(sizeof(int));
			*ptrTurret = turretID;
			fb_weapon_info[turretID].timer = add_event( &timer_list, fb_turret_event , ptrTurret, NULL,
				get_now_tv()->tv_sec + fb_weapon_info[turretID].cd, 0 );
			isEnter = 1;
		}
	}
	else if( clientTurret <= 4 && clientTurret > 0 )
	{
		if( bomb_member_size < FB_BOMB_TURRET_SIZE )
		{
			if( bomb_turret[clientTurret-1] == NULL )
			{
				int *bombTurretID = (int*)p->session;
				*bombTurretID = clientTurret;

				int j = 0;
				uint8_t buff[13];
				pkg_item_kind( p, buff, ITEM_ID_BLUE_BOMB, &j );
				PKG_H_UINT32(buff, ITEM_ID_BLUE_BOMB, j);
				PKG_H_UINT32(buff, ITEM_ID_BLUE_BOMB+1, j);
				PKG_UINT8(buff, 2, j);
				return send_request_to_db(SVR_PROTO_CHK_ITEM, p, j, buff, p->id);
			}
			else
			{
				isEnter = 0;
			}
		}
		else
		{
			isEnter = 0;
		}
	}
	if( 1 == isEnter )
	{
		int len = sizeof(protocol_t);
		PKG_UINT32( msg, p->id, len );
		PKG_UINT32( msg, turretID, len );
		init_proto_head( msg, p->waitcmd, len );
		send_to_map( p, msg, len, 1 );
		return 0;
	}
	else
	{
		return send_to_self_error( p, p->waitcmd, ERR_fb_enter_turret_failed, 1 );
	}
}

int fb_enter_bomb_trret(sprite_t* p, int bomb_count)
{
	if( bomb_count <= 0 )
	{
		return send_to_self_error( p, p->waitcmd, ERR_fb_enter_turret_failed, 1 );
	}
	else
	{
		int *clientTurretID = (int*)p->session;
		if( *clientTurretID <= 0 || *clientTurretID > 4 )
		{
			ERROR_RETURN( ("Final BOSS 2009 Enter Bomb Trret Error TurretID:[%d]", *clientTurretID), -1 );
		}

		if( bomb_turret[*clientTurretID - 1] == NULL )
		{
			bomb_turret[*clientTurretID - 1] = p;
			bomb_member_size++;
		}

		int turretID = WT_BLUE_BOMB + *clientTurretID - 1;
		int len = sizeof(protocol_t);
		PKG_UINT32( msg, p->id, len );
		PKG_UINT32( msg, turretID, len );
		init_proto_head( msg, p->waitcmd, len );
		send_to_map( p, msg, len, 1 );
		//we have enough palyers and the weapon isn't in cold down
		if( bomb_member_size == FB_BOMB_TURRET_SIZE && fb_weapon_info[WT_BLUE_BOMB].timer == NULL )
		{
			blue_bomb_attack();
		}
		else
		{
			//we should wait
		}
		return 0;
	}
}

int fb_leave_turret_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	int isComplete = 0;
	DEBUG_LOG( "Player Levae Turret cmd:[%d], expect:[%d]", p->waitcmd, PROTO_FB_LEAVE_TURRET_CMD );
	if( p->waitcmd == PROTO_FB_LEAVE_TURRET_CMD )
	{
		isComplete = 1;
	}
	if( p->tiles && FB_MAP_ID != p->tiles->id && 1 == isComplete )
	{
		return send_to_self_error( p, p->waitcmd, ERR_fb_invalid_cmd, 1 );
	}
	//int magic turret
	int turretID = p->team_id - 1;
	if( turretID <= WT_MAGIC_SPIRIT && turretID >= WT_MAGIC_ICE && magic_turret[turretID] == p )
	{
		DEBUG_LOG( "Player Levae Turret ID:[%u]", turretID );
		magic_turret[turretID] = NULL;
		int len = sizeof(protocol_t);
		PKG_UINT32( msg, p->id, len );
		PKG_UINT32( msg, turretID, len );
		init_proto_head( msg, PROTO_FB_LEAVE_TURRET_CMD, len );
		send_to_map( p, msg, len, isComplete );
		return 0;
	}
	else
	{
		int i;
		int removeOK = 0;
		for( i = 0; i < FB_BOMB_TURRET_SIZE; ++i )
		{
			if( bomb_turret[i] == p )
			{
				bomb_turret[i] = NULL;
				bomb_member_size--;
				removeOK = 1;
				turretID = WT_BLUE_BOMB + i;
				break;
			}
		}
		if( 0 == removeOK )
		{
			if( 1 == isComplete )
			{
				return send_to_self_error( p, p->waitcmd, ERR_fb_invalid_cmd, 1 );
			}
			else
			{
				return 0;
			}
			//ERROR_RETURN(("Remove Player From Bomb Turret Failed"),-1);
		}
		DEBUG_LOG( "Player Levae Turret ID:[%u]", turretID );
		int len = sizeof(protocol_t);
		PKG_UINT32( msg, p->id, len );
		PKG_UINT32( msg, turretID, len );
		init_proto_head( msg, PROTO_FB_LEAVE_TURRET_CMD, len );
		send_to_map( p, msg, len, isComplete );
		return 0;
	}
	if( 1 == isComplete )
	{
		return send_to_self_error( p, p->waitcmd, ERR_fb_invalid_cmd, 1 );
	}
	else
	{
		return 0;
	}
}


