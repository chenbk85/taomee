/*
 * =====================================================================================
 *
 *       Filename:  work_proc.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月20日 16时16分52秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */

#ifndef H_I_WORK_PROC_H_20110720
#define H_I_WORK_PROC_H_20110720

#include <benchapi.h>

struct i_work_proc
{
public:
    virtual int init(int argc, char ** argv) = 0;
	virtual int uninit() = 0;
	virtual int release() = 0;
    virtual int process(char * req, int req_len , char ** ack, int * ack_len) = 0;
};

int create_work_proc_instance(i_work_proc **pp_instance);

#endif
