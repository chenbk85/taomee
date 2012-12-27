#include "seesaw.hpp"

Seesaw::PlayerCoordMap	Seesaw::player_pos_map_;
uint8_t					Seesaw::pkg_[1024];
sprite_t*				Seesaw::cur_user_;

//----------------------------------------------------------------------
// Private Methods
//
int Seesaw::handle_player_move(const uint8_t body[], int len)
{
	CHECK_BODY_LEN(len, 5);

	//
	int     i = 0;
	PosType pos;

	taomee::unpack(body, pos.x, i);
	taomee::unpack(body, pos.y, i);
	taomee::unpack(body, pos.direction, i);

	player_pos_map_[cur_user_->id] = pos;

	cur_user_->posX      = pos.x;
	cur_user_->posY      = pos.y;
	cur_user_->action    = ACTION_SIT;
	cur_user_->direction = pos.direction;
	//
	i = sizeof(protocol_t);
	taomee::pack(pkg_, cur_user_->id, i);
	taomee::pack(pkg_, body, len, i);
	init_proto_head(pkg_, cur_user_->waitcmd, i);

	send_to_map(cur_user_, pkg_, i, 1);
	return 0;
}

//
void Seesaw::rsp_seesaw_status()
{
	int     i   = sizeof(protocol_t);
	uint8_t cnt = player_pos_map_.size();

	taomee::pack(pkg_, cnt, i);
	if (cnt) {
		for ( PlayerCoordMap::const_iterator it = player_pos_map_.begin();
				it != player_pos_map_.end(); ++it ) {
			taomee::pack(pkg_, it->first, i);
			taomee::pack(pkg_, it->second.x, i);
			taomee::pack(pkg_, it->second.y, i);
			taomee::pack(pkg_, it->second.direction, i);
		}
	}
	init_proto_head(pkg_, cur_user_->waitcmd, i);

	send_to_self(cur_user_, pkg_, i, 1);
}
