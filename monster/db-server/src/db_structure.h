/**
 * =====================================================================================
 *       @file  db_structure.h
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  09/08/2011 03:39:17 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef H_DB_STRUCTURE_H_20110908
#define H_DB_STRUCTURE_H_20110908

#pragma pack(push)
#pragma pack(1)

typedef struct
{
    uint32_t approved_message_num;
    uint32_t unapproved_message_num;
} message_num_t;

#pragma pack(pop)

#endif //H_DB_STRUCTURE_H_20110908

