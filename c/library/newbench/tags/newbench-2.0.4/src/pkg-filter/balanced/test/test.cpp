/*
 * =====================================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2010年08月15日 20时32分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Baron (YBJ), wedgwood@qq.com
 *        Company:
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <assert.h>
#include "i_barrier_ring_queue.h"
#include "c_barrier_ring_queue.h"

int main(int argc, char* argv[])
{
    i_barrier_ring_queue *p_instance;
    create_barrier_ring_queue_instance(&p_instance);
    int result = p_instance->init(80 * 1000, 0);
    if (result != 0)
    {
        printf("\n%d\n", p_instance->get_last_errno());
    }
    char tmp[1024] = {'\0'};

    for (int i = 0; i < 100000;++ i)
    {
        p_instance->push("abc", 4, 1, 1);
    }

    int l = p_instance->pop(tmp, 5);

    printf("\n%d\n", l);
    printf("\n%s\n", tmp);

    return 0;
}
