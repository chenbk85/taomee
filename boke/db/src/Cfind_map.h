/*
 * =========================================================================
 *
 *        Filename: Cfind_map.h
 *
 *        Version:  1.0
 *        Created:  2011-05-13 15:29:51
 *        Description:  
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */
#ifndef  CFIND_MAP_H
#define  CFIND_MAP_H

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"


class Cfind_map:public CtableRoute100x10
{
private:
public:
	Cfind_map (mysql_interface * db);
	int	insert(userid_t userid,  uint32_t islandid, uint32_t mapid);
	int	get_list(userid_t userid, std::vector<find_map_t> &find_map_list );
	int del(userid_t userid, uint32_t islandid, uint32_t mapid);
};

#endif  /*CFIND_MAP_H*/
