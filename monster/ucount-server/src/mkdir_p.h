/** 
 * ===============================================================
 * @file mkdir_p.h
 * @brief 抽取了mkdir源代码中递归建立多级目录部分，形成makedir函数
 * 实现了mkdir -p的效果
 * @author smyang（杨思敏）, smyang@taomee.com
 * @version 1.0.0
 * @date 2010-09-08
 * copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ===============================================================
 */

#ifndef H_MKDIR_P_H_2010_09_08
#define H_MKDIR_P_H_2010_09_08

#include <stdint.h>

/** 
 * @brief mkdir_p 一次性建立多级目录
 *
 * @param prefix 如果prefix指向的路径无法访问，则mkdir_p(prefix/dir)
 * @param dir 要建立的目录的绝对路径为prefix/dir
 * 
 * @return 0成功，-1失败
 */
int mkdir_p(const char * prefix, const char * dir);

/** 
 * @brief mkdir_p 等价于mkdir_p(prefix, "time")
 *
 * @param prefix 
 * @param time 要建立的目录的绝对路径为prefix/time
 * 
 * @return 0成功，-1失败
 */
int mkdir_p(const char * prefix, uint32_t time);

/** 
 * @brief mkdir_p 一次性建立多级目录
 * 
 * @param dir 要建立的目录
 * 
 * @return 0成功，-1失败
 */
int mkdir_p(const char * dir);

#endif
