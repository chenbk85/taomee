/*
 * =====================================================================================
 *
 *       Filename:  link.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/07/2012 11:50:30 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include <libtaomee++/random/random.hpp>
#include <netinet/in.h>

extern "C"
{
#include "../../proto.h"
#include "../../dbproxy.h"
#include "../../timer.h"
#include "../../game.h"

#ifndef TW_VER
#include  <statistic_agent/msglog.h>
#endif

}

#include "../../ant/inet/pdumanip.hpp"
#include "l_link.h"
#include "word_map.h"
#include <algorithm>
#define PIC_STATE   49

uint32_t deep = 0;

l_link::l_link()
{

	for(uint32_t i = 0; i < DIRECTIONS; ++i){
		if(i == 0){
			directions[i] = EAST;
		}
		else if(i == 1){
			directions[i] = SOUTH;
		}
		else if(i == 2){
			directions[i] = WEST;
		}
		else{
			directions[i] = NORTH;
		}
	}//for
	memset(&path, 0, sizeof(path));
	pair_count = 0;
	find_pairs = 0;
	generate_map();
}

l_link::l_link(l_link &other)
{
	memcpy(this->map, other.map, sizeof(this->map));
	this->pair_count = other.pair_count;
	memcpy(this->directions, other.directions, sizeof(directions));
	memcpy(&this->path, &other.path, sizeof(path));
	
	this->find_pairs = other.find_pairs;
}

int l_link::find_path(point_t start, point_t end)
{
	DEBUG_LOG("start_x: %u, start_y: %u, end_x: %u, end_y: %u", start.x, start.y, end.x, end.y);
	if(start.x == 0 || start.x >= ROWS || start.y ==0 || start.y >= COLS ){
		return -1;
	}
	if(end.x == 0 || end.x >= ROWS || end.y == 0 || end.y >= COLS ){
		return -1;
	}
	if(start == end){
		return -1;
	}
	if(map[start.x][start.y] != map[end.x][end.y]){
		return -1;
	}
	point_t new_start(start.x, start.y);
	point_t new_end(end.x, end.y);
	memset(&path, 0, sizeof(path));
	deep = 0;
	int ret = travel(new_start, new_end);
	if(ret == 0){
		if(pair_count == find_pairs){
			ret = 2;
		}
	}
	else{
		ret = -1;
	}
	return ret;

}

int l_link::travel(point_t start, point_t end)
{
	++deep;
	//DEBUG_LOG("%u, start(%u, : %u ),  end(%u, %u)", deep, start.x, start.y, end.x, end.y);
	path.already_path.push_back(start);
	if(start == end){
		map[end.x][end.y] = 0;
		map[path.already_path[0].x][path.already_path[0].y] = 0;
		++find_pairs;
		//DEBUG_LOG("----find_pairs: %u, pair_count: %u ----", find_pairs, pair_count);
		return 0;
	}
	/*
	find_optimal_direction(start, end);
	*/
	uint32_t i = 0;
	for(; i < DIRECTIONS; ++i) {
		int step_x = 0, step_y = 0;
		switch(directions[i]){
			case EAST:
				{
					step_y = 1;
					break;
				}
			case WEST:
				{
					step_y = -1;
					break;
				}
			case SOUTH:
				{
					step_x = 1;
					break;
				}
			case NORTH:
				{
					step_x = -1;
					break;
				}
		}
		
		//new initial point;
		point_t new_start;
		new_start.x = step_x + start.x;
		new_start.y = step_y + start.y;

		//out of range
		if( new_start.x >= ROWS || new_start.y >= COLS){ 
			continue;
		}
		//some obstacles
		if(map[new_start.x][new_start.y] != 0 && map[new_start.x][new_start.y] != map[end.x][end.y]){ 
			continue;
		}
		//have already travel
		std::vector<point_t>::reverse_iterator iter = std::find( path.already_path.rbegin(), path.already_path.rend(), new_start);
		if(iter != path.already_path.rend()){
			continue;
		}

		//check corners,if corners > 2, bad over this path
		int able_corner = check_corners(new_start);
		if(able_corner == -1){
			continue;
		}
		//able to continue travel along this path
		int ret = travel(new_start, end);
		if(ret == 0){
			return ret;
		}
		if(able_corner == 1){
			--path.corner;
		}

	}
	if(i >= DIRECTIONS){
		path.already_path.pop_back();
		return -1;
	}
	return 0;

}

int l_link::check_corners(point_t cur)
{
	if(path.already_path.size() > 1){
		point_t prev = path.already_path[path.already_path.size()-2];
		if(prev.x != cur.x && prev.y != cur.y){
			if(path.corner < 2){
				++path.corner;
				return 1;
			}
			else{
				return -1;
			}
		}
		return 0;
		
	}

	return 0;
}

//find point2 where , relative to point1 direction
int l_link::find_optimal_direction(point_t point1, point_t point2)
{

	if(point1.y > point2.y){
		uint8_t direct = directions[0];
		directions[0] = directions[2];
		directions[2] = direct;
	}

	if(point1.x > point2.x){
		uint8_t direct = directions[1];
		directions[1] = directions[3];
		directions[3] = direct;
		
	}

	return 0;
}

//generate map
int l_link::generate_map()
{
	memset(map, 0, sizeof(map));
	uint32_t which = rand()%26;
	DEBUG_LOG("------- which: %u ---------", which);
	//memcpy(map, WordMap[which], sizeof(map));
	for(uint8_t i = 0; i < ROWS; ++i){
		for(uint8_t j = 0; j < COLS; ++j){
			map[i][j] = WordMap[which][i][j];
		}
	}

	for(uint32_t i = 0; i < ROWS; ++i){
		for(uint32_t j = 0; j < COLS; ++j){
			DEBUG_LOG("i: %u j: %u, state: %u", i, j ,map[i][j]);
		}
	}
	
	std::vector<point_t> vec;
	for(uint8_t i = 0; i < ROWS; ++i){
		for(uint8_t j = 0; j < COLS; ++j){
			if(map[i][j] == 1){
				point_t one(i,j);
				vec.push_back(one);
				++pair_count;
			}
		}//for
	}//for

	pair_count = pair_count / 2;
	std::random_shuffle(vec.begin(), vec.end());
	//std::random_shuffle(vec.begin(), vec.end());

	for(uint32_t k = 0; k < vec.size(); ){
		uint32_t rand_state =rand()%PIC_STATE + 1;
		map[vec[k].x][vec[k].y] = rand_state;
		map[vec[k+1].x][vec[k+1].y] = rand_state;
		k = k+2;
	}
	/*
	for(uint32_t i = 0; i < ROWS; ++i){
		for(uint32_t j = 0; j < COLS; ++j){
			DEBUG_LOG("i: %u j: %u, state: %u", i, j ,map[i][j]);
		}
	}
	*/
	return 0;
}
