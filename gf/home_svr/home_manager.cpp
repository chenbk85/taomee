/*
 * =====================================================================================
 *
 *       Filename:  home_manager.cpp
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  07/05/2011 10:28:52 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#include "home_manager.hpp"
#include "home.hpp"

Home *
HomeManager::FindHome(uint32_t UserId, uint32_t RoleTime)
{
	HomeIndexMap::iterator it = AllHomes_.find(RoleTime % 10);
	if (it != AllHomes_.end()) {
		HomeList * tlist = (it->second);
		for (HomeList::iterator it = tlist->begin(); it != tlist->end(); ++it) { 
			if ((*it)->OwnerUserId == UserId && (*it)->OwnerRoleTime == RoleTime) {
				return *it;
			}
		}
	}
	return 0;
}

void HomeManager::RemoveHome(uint32_t UserId, uint32_t RoleTime)
{
	HomeIndexMap::iterator it = AllHomes_.find(RoleTime % 10);
	if (it != AllHomes_.end()) {
		HomeList * tlist = (it->second);
		for (HomeList::iterator it = tlist->begin(); it != tlist->end(); ++it) { 
			if ((*it)->OwnerUserId == UserId && (*it)->OwnerRoleTime == RoleTime) {
				it = tlist->erase(it);
				return;
			}
		}
	}
}

void HomeManager::AddNewHome(Home * home)
{
	HomeIndexMap::iterator it = AllHomes_.find(home->OwnerRoleTime % 10);
	if (it == AllHomes_.end()) {
		HomeList * tlist = new HomeList();
		AllHomes_.insert(HomeIndexMap::value_type(home->OwnerRoleTime % 10, tlist));
		tlist->push_back(home);
	} else {
		HomeList * tlist = it->second;
		tlist->push_back(home);
	}
}

void HomeManager::Update()
{
	HomeIndexMap::iterator it = AllHomes_.begin();
	while (it != AllHomes_.end()) {
		HomeList * tlist = it->second;
		for (HomeList::iterator pIter = tlist->begin(); pIter != tlist->end(); ++pIter) { 
				Home * home = *pIter;
				home->Update();
		}
		++it;
	}
}
