/**
 * =====================================================================================
 *
 *	@file  user.hpp
 *
 *	@brief  用户基本信息处理相关的功能提供，应用框架的一部分
 *
 *	@version:  1.0
 *	Created:  2009年12月28日 10时05分57秒
 *	Revision:  none
 *	Compiler:  gcc -Wall -Wextra user.hpp
 *
 *	@author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *	Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#ifndef DBSER_USER_LIB_HPP
#define DBSER_USER_LIB_HPP
#include<cstring>
#include<map>
#include<utility>

namespace db_utility
{

struct DressingUnit
{
	uint32_t num;
	uint32_t colth[12];
}__attribute__((packed));

inline bool dress_clean(DressingUnit& dress)
{
	const uint32_t n=dress.num;
	std::sort(dress.colth, dress.colth+n);
	//一个组合最多有12个衣服
	if(12!=n)
	{
		::memset(dress.colth+n, 0, sizeof(dress.colth[0])*(12-n));
	}

	return true;
}

inline bool operator==(const DressingUnit& l, const DressingUnit& r)
{
	if(l.num!=r.num)
	{
		return false;
	}

	const uint32_t n=l.num;
	return 0==std::memcmp((void*)l.colth, (void*)r.colth, n*sizeof(r.colth[0]));
}

typedef uint32_t UserAttire;

struct UserAttireBuyUnit
{
	uint32_t count;
	uint32_t max;
	bool exist;
};

typedef std::map<UserAttire, UserAttireBuyUnit> UserAttireBuyMap;

struct UserAttireWithCount
{
	uint32_t count;
};

typedef std::map<UserAttire, UserAttireWithCount> UserAttireCountMap;

inline bool completeness_ckeck(UserAttireBuyMap& buy_map, const UserAttireCountMap& have_map)
{
	UserAttireCountMap::const_iterator itr=have_map.begin();
	for(; itr!=have_map.end(); ++itr)
	{
		UserAttireBuyMap::iterator buy_itr=buy_map.find(itr->first);
		if(buy_itr!=buy_map.end()) //如果找到
		{
			UserAttireBuyUnit& u=buy_itr->second;
			const UserAttireWithCount& c=itr->second;
			if(u.count+c.count>u.max)
			{
				return false;
			}
			u.exist=true;
		}
	}
	return true;
}

template<typename List>
inline void to_buy_map(size_t n, List* p, UserAttireBuyMap& buy_map)
{
	for(size_t i=0; i<n; ++i, ++p)
	{
		UserAttireBuyUnit u;
		u.count=p->count;
		u.max=p->max;
		u.exist=false;
		buy_map[p->attire_id]=u;
	}
}

template<typename List>
inline void to_count_map(size_t n, List* p, UserAttireCountMap& have_map)
{
	for(size_t i=0; i<n; ++i, ++p)
	{
		UserAttireWithCount u;
		u.count=p->count;
		have_map[p->attireid]=u;
	}
}

template<typename List>
inline void to_count_list(const UserAttireCountMap& have_map, size_t& n, List* p)
{
	UserAttireCountMap::const_iterator itr=have_map.begin();
	for(size_t i=0; i<have_map.size(); ++i, ++itr)
	{
		p->count=itr->second.count;
		p->attire_id=itr->first;
	}
	n=have_map.size();
}

template<typename List>
inline void to_attire_list(const UserAttireBuyMap& map, size_t limit, List* p)
{
	UserAttireBuyMap::const_iterator itr=map.begin();
	size_t lim=std::min(limit, map.size());
	for(size_t i=0; i<lim; ++i, ++itr)
	{
		p[i]=itr->first;
	}
}
}

#endif //DBSER_USER_LIB_HPP


