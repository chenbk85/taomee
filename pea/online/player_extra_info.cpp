#include "player_extra_info.hpp"
#include "proto.hpp"
#include "cli_proto.hpp"


int notify_extra_info(player_t * p, std::vector<uint32_t> info_id)
{
    if (info_id.empty())
    {
        return 0;
    }
    cli_proto_obj_attr_chg_out out;
    out.user_id = p->id;
    obj_attr_chg chg;
    for (uint32_t i = 0; i < info_id.size(); i++)
    {
        chg.obj_attr_type = info_id[i];
        chg.obj_attr_value = p->extra_info->id2value(info_id[i]);
        out.obj_attr_array.push_back(chg);
    }

    return send_to_player(p, &out, cli_proto_obj_attr_chg_cmd, 0);
}


int notify_extra_info(player_t * p, uint32_t info_id)
{
    cli_proto_obj_attr_chg_out out;
    out.user_id = p->id;
    obj_attr_chg chg;
    chg.obj_attr_type = info_id;
    chg.obj_attr_value = p->extra_info->id2value(info_id);
    out.obj_attr_array.push_back(chg);

    return send_to_player(p, &out, cli_proto_obj_attr_chg_cmd, 0);
}


int notify_extra_info(player_t * p, std::vector<db_extra_info_t> extra_info)
{
    if (extra_info.empty())
    {
        return 0;
    }
    cli_proto_obj_attr_chg_out out;
    out.user_id = p->id;
    obj_attr_chg chg;
    for (uint32_t i = 0; i < extra_info.size(); i++)
    {
        chg.obj_attr_type = extra_info[i].info_id;
        chg.obj_attr_value = p->extra_info->id2value(extra_info[i].info_id);
        out.obj_attr_array.push_back(chg);
    }

    return send_to_player(p, &out, cli_proto_obj_attr_chg_cmd, 0);
}

