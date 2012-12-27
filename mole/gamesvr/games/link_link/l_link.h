/*
 * =====================================================================================
 *
 *       Filename:  link.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/07/2012 11:51:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#ifndef LINK_LINK_INCL
#define LINK_LINK_INCL


#define ROWS     10
#define COLS     14
#define DIRECTIONS 4


#define  NORTH   0 //
#define  SOUTH   1 //
#define  WEST    2 //
#define  EAST    3 //
#include "../../ant/inet/pdumanip.hpp"
#include <vector>

struct point{
	uint8_t x;
	uint8_t y;
	point():x(0),y(0){}
	point(uint8_t x0, uint8_t y0):x(x0), y(y0){}
	inline bool operator == ( const point& other )
	{
		return (this->x == other.x && this->y == other.y);
	}
};

typedef struct point point_t;

 typedef struct route{
	uint8_t corner;
	std::vector<point_t> already_path; 
} route_t;


class l_link{

	public:
		int find_path(point_t start, point_t end);
		l_link(l_link &other);
		l_link();
		~l_link(){};
	public:
		uint8_t map[ROWS+2][COLS+2];

		uint32_t pair_count;
		uint32_t find_pairs;

	private:
		route_t path;
		uint8_t directions[DIRECTIONS];

	private:
		int check_corners(point_t cur);
		int find_optimal_direction(point_t point1, point_t point2);
		int travel(point_t start, point_t end);
		int generate_map();
};

#endif
