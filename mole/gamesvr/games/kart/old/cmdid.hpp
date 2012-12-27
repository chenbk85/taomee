#ifndef KARTGAME_CMDID_HPP_
#define KARTGAME_CMDID_HPP_

enum CmdIdType {
	KG_map_info       = 31051,
	KG_player_ready,
	KG_start,
	KG_player_action,
	KG_item_got,
};

enum ItemType {
	ITEM_invalid        = 0,
	ITEM_min            = 0,
	ITEM_tapered_bar,
	ITEM_rounded_hole,
	ITEM_pending_banana,
	ITEM_dropped_banana,
	ITEM_ice_magic,
	ITEM_rocket_engine,
	ITEM_tmp,

	ITEM_max            = 18,
	ITEM_rate           = 40,

	ITEM_track_border   = 0xFF
};

#endif // KARTGAME_CMDID_HPP_
