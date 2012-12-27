/*
 * =====================================================================================
 *
 *       Filename:  home_manager.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/05/2011 10:28:47 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#ifndef HOME_MANAGER_HPP
#define HOME_MANAGER_HPP
extern "C" {
#include <stdint.h>
}
#include <map>
#include <list>
#include "singleton.hpp"

class Home;

typedef std::list<Home*> HomeList;

typedef std::map<uint32_t, HomeList*> HomeIndexMap;

class HomeManager {
 public:
  HomeManager(){}

  ~HomeManager(){}

  void AddNewHome(Home * home);

  void RemoveHome(uint32_t UserId, uint32_t RoleTime);
  
  Home * FindHome(uint32_t UserId, uint32_t RoleTime);

  void Update();

 private:

  HomeIndexMap AllHomes_;
};

#endif




