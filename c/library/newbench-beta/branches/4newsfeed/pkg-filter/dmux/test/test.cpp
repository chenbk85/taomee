/**
 * @file test.cpp * @brief
 * @author baron baron@taomee.com
 * @version 1.0
 * @date 2011-04-15
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include "i_barrier_ring_queue.h"
#include "c_barrier_ring_queue.h"
#include "afutex.h"
#include <sys/mman.h>

typedef struct {
    int key;
} data_t;

int *mutex;
int *cnter;

int main(int argc, char* argv[])
{
    FILE *fh = fopen("./info", "a+");
    mutex = reinterpret_cast<int *>(mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_SHARED, -1, 0));

    if (mutex == MAP_FAILED)
    {
        printf("\nfail to mmap!\n");
        return 0;
    }

    cnter = reinterpret_cast<int *>(mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_SHARED, -1, 0));

    if (cnter == MAP_FAILED)
    {
        printf("\nfail to mmap!\n");
        return 0;
    }

    afutex_mutex_init(mutex);
    i_barrier_ring_queue *p_instance;
    create_barrier_ring_queue_instance(&p_instance);
    int result = p_instance->init(80 * 10000, 5);
    data_t data;

    if (result != 0)
    {
        printf("\n%d: %s\n", p_instance->get_last_errno(), p_instance->get_last_errstr());
        return -1;
    }

    char tmp[1024] = {'\0'};

    for (int i = 0; i < 100000;++ i)
    {
        data.key = rand() % 10;
printf("\n%d", data.key);
        p_instance->push(reinterpret_cast<char *>(&data), sizeof(data), data.key, 1);
    }

    //while (2)
    //{
        //result = p_instance->pop(tmp, 6);

        //printf("\nresult: %d", result);
        //fflush(NULL);

        //if (result > 0)
        //{
            //printf("\ndata[key:%d]\n", reinterpret_cast<data_t *>(tmp)->key);
        //}
        //else
        //{
            //break;
        //}
    //}

    //return 0;



    for (int i = 0; i < 10; ++ i)
    {
        if (fork() == 0)
        {
            sleep(1);
            pid_t pid = getpid();
            int key = -1;

            while (1)
            {
                afutex_lock(mutex);

                result = p_instance->pop(tmp, 6);

                if (result > 0)
                {
                    ++ (*cnter);
                }

                afutex_unlock(mutex, 10);

                //printf("\npid: %d result: %d", pid, result);
                //fflush(NULL);

                if (result > 0)
                {
                    if (key == -1)
                    {
                        key = reinterpret_cast<data_t *>(tmp)->key;
                        printf("\n%d: data[key:%d]", pid, key);
                        fflush(stdout);
                    }
                    else
                    {
                        if (key != reinterpret_cast<data_t *>(tmp)->key)
                        {
                            printf("\n---------------------error!");
                            fflush(stdout);
                        }
                    }
                }
                else
                {
                    break;
                }
            }

            if (result < 0)
            {
                printf("\n%d: error[%d:%s]", pid, p_instance->get_last_errno(), p_instance->get_last_errstr());
                return -1;
            }

            printf("\n%d: return", pid);
            fflush(stdout);
            return 0;
        }
        else
        {
            //printf("\nfork one!\n");
            //fflush(stdout);
        }
    }

    sleep(20);
    printf("\ntotal:%d", *cnter);
    munmap(mutex, sizeof(int));
    munmap(cnter, sizeof(int));
    fclose(fh);
    return 0;
}
