#include <cmath>

extern "C" {
#include <gameserv/timer.h>
}

#include "surf_riding_rule.hpp"

//--------------------------
//	static fields
//
std::vector<SurfRidingRule::TileGrpType> SurfRidingRule::tilegrps;

//--------------------------
//	public methods
//
/**
 * handle_player_action -
 *
 * return: 0 if everything ok, -1 to close `p`, GER_end_of_game to end the whole game
 */
int SurfRidingRule::
handle_player_action(sprite_t* p, const uint8_t body[], int len)
{
	static const int bonus[] = { -150, -100, 50, 100 };
	static const size_t bonus_cnt = (sizeof bonus) / (sizeof bonus[0]);

	if ( started_check(p, len, 20) == -1 ) {
		return -1;
	}

	int i = 0;
	uint16_t action;
	uint32_t x, y, acttime;
	ant::unpack(body, x, i);
	ant::unpack(body, y, i);
	ant::unpack(body, action, i);
	ant::unpack(body, acttime, i);

	// judge if gamecheater is used
	if ( is_cheater(p, action, x, y, acttime) ) {
		ERROR_RETURN(("%s GameCheater: uid=%u grpid=%lu", gamegrp_->game->name, p->id, gamegrp_->id), -1);
	}
	// accumulate game score
	if ( action < bonus_cnt ) {
		p->score += bonus[action];
		DEBUG_LOG("Uid=%u Action=%u Bonus=%d TotalScore=%d", p->id, action, bonus[action], p->score);
	}
	// update last active time
	if ( (action < 2000) || (action == sc_arrive_flag) ) {
		p->last_act_time = acttime;
	}
	// player finished game
	if ( action == sc_arrive_flag ) {
		DEBUG_LOG("%u Arrived at %s Termination: time=%lu", p->id, gamegrp_->game->name, p->last_act_time);
		if ( handle_finished(p) == -1 ) {
			return -1;
		}
		REMOVE_TIMERS(p);
	}

	if ( pendings_.size() ) {
		handle_pending_finished();
	} else if ( nfinishers_ == gamegrp_->game->players ) {
		return GER_end_of_game;
	}

	return 0;
}

/**
 * sendmap - generate a surf riding map and send it to all the clients
 *
 */
void SurfRidingRule::sendmap() const
{
	static uint8_t pkg[4096];
	static const uint16_t maplen  = 800;
	static const uint16_t upbound = maplen - 15;

	int len = sizeof(protocol_t);
	ant::pack(pkg, maplen, len); // pack len of map
	len += 2; // reserve space for packing bar_cnt
	//
	uint16_t bar_cnt = 0;
    int x = 0, i1, i2;
    // randomly generates barries and their positions
    while ( (x += ant::ranged_random(15, 18)) < upbound ) {
		i1 = rand() % tilegrps.size();      // get a tile group randomly
		i2 = rand() % tilegrps[i1].size();  // get a tile randomly
		for (TileType::const_iterator it = tilegrps[i1][i2].begin(); it != tilegrps[i1][i2].end(); ++it) {
			ant::pack(pkg, static_cast<uint16_t>(it->coord.x() + x), len);
			ant::pack(pkg, it->coord.y(), len);
			ant::pack(pkg, it->type, len);
			//DEBUG_LOG("x=%d y=%d type=%d", it->coord.x() + x, it->coord.y(), it->type);
		}
		bar_cnt += tilegrps[i1][i2].size();
		//DEBUG_LOG("i1=%d i2=%d size=%d", i1, i2, tilegrps[i1][i2].size());
    }
	// pack num of barriers
	int i = sizeof(protocol_t) + sizeof(maplen);
	ant::pack(pkg, bar_cnt, i);
	//
	init_proto_head(pkg, proto_game_map_info, len);
	send_to_players(gamegrp_, pkg, len);
	//DEBUG_LOG("bar_cnt=%d len=%d", bar_cnt, len);
}

//--------------------------
//	private methods
//
/**
 * handle_finished -
 *
 * return:
 */
int SurfRidingRule::handle_finished(sprite_t* p)
{
	static const float rates[] = { 1.0f, 0.6f, 0.29f };

	if ( !pendings_.count(p->id) ) {
		if ( arrivers_.insert(ArriverMap::value_type(p->id, p->last_act_time)).second ) {
			++nfinishers_;
		} else {
			ERROR_RETURN( ("%u Had Already Arrived At Terminal", p->id), -1 );
		}
	}

	int rank = get_rank(p);
	if ( rank == -1 ) {
		pendings_.insert(p->id);
	} else {
		pendings_.erase(p->id);

		const game_t* game = gamegrp_->game;
		game_score_t  gs;
		memset(&gs, 0, sizeof gs);
		gs.rank   = static_cast<uint16_t>(rank + 1);
		gs.exp	  = static_cast<int>(round(game->exp * rates[rank]));
		gs.strong = static_cast<int>(round(game->strong * rates[rank]));
		gs.coins  = static_cast<int>(round(game->yxb * rates[rank]));
		gs.time   = (p->last_act_time - start_tm_) * 100 + (rand() % 100);
		gs.score  = p->score;
		switch (rank) {
		case 0:
			if (game->players != 1) {
				gs.score += 10000;
			}
			break;
		case 1:
			if (game->players == 3) {
				gs.score += 1000;
			}
			break;
		default:
			break;
		}
		gs.itmid   = 12136;
		gs.itm_max = 1;

		/*if (!rank) {
			if ( (gs.score > 8000) || !(rand() % 3) ) {
				gs.itmid   = 12136;
				gs.itm_max = 1;
			}
			//else {
				//gs.itmid = get_fire_medal(p, rank);
			//}
		}*/
		uint32_t gid = 21;
		pack_score_session(p, &gs, gid, gs.score);
		if ( submit_game_score(p, &gs) == -1 ) {
			ERROR_RETURN( ("Failed to Submit Game Score: uid=%u", p->id) , -1 );
		}
	}
	return 0;
}

/**
 * get_rank - get rank of `p`
 *
 * return: 0, 1, 2..., or -1 if rank cannot be determinated yet
 */
int SurfRidingRule::get_rank(const sprite_t* p) const
{
	// scan players still running
	for ( uint8_t i = 0; i != gamegrp_->count; ++i ) {
		const sprite_t* op = gamegrp_->players[i];
		if ( op != p ) {
			if ( !arrivers_.count(op->id) && (op->last_act_time < p->last_act_time)
					&& (op->last_act_time != 0) ) {
				// cannot determine rank while some other players are still runing and
				// their last act time is smaller than the one just arrived at terminal
				return -1;
			}
		}
	}

	int rank = 0;
	// scan players in ArriverMap
	for ( ArriverMap::const_iterator it = arrivers_.begin(); it != arrivers_.end(); ++it ) {
		if ( p->last_act_time > it->second ) {
			++rank;
		}
	}
	return rank;
}

//--------------------------
//	public statics
//
int SurfRidingRule::load_tile_grps()
{
	static const char* file = "./conf/surf_riding_map.xml";

	int err = -1;
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile(file);
	if (!doc) goto ret2;

	cur = xmlDocGetRootElement(doc);
	if (!cur) goto ret1;

	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"TileGroup")){
			err = load_tile_grp(cur);
			if (err) goto ret1;
		}
		cur = cur->next;
	}

ret1:
	xmlFreeDoc(doc);
ret2:
	BOOT_LOG(err, "Load %s", file);
}

//--------------------------
//	private statics
//
int SurfRidingRule::load_tile_grp(xmlNodePtr cur_node)
{
	int row, col, type;
	TileGrpType tilegrp;
	xmlNodePtr child_node;

	cur_node = cur_node->xmlChildrenNode;
	while (cur_node) {
		if (!xmlStrcmp(cur_node->name, (const xmlChar*)"Tile")){
			TileType tile;
			child_node = cur_node->xmlChildrenNode;
			while (child_node) {
				if (!xmlStrcmp(child_node->name, (const xmlChar*)"BarPos")){
					DECODE_XML_PROP_INT(row, child_node, "Row");
					DECODE_XML_PROP_INT(col, child_node, "Col");
					DECODE_XML_PROP_INT(type, child_node, "BarType");
					tile.push_back(BarrierType(type, row, col));
				}
				child_node = child_node->next;
			}
			tilegrp.push_back(tile);
		}
		cur_node = cur_node->next;
	}
	//
	tilegrps.push_back(tilegrp);
	return 0;
}
