#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <iostream>
#include <iomanip>
using namespace std;

struct sockaddr_in g_dest_addr;
int g_thread_cnt = 0;
int g_pkg_cnt = 0;
int g_pkg_inter = 0;
int g_proc_wait = 0;
int g_filter_flag = -1;
int g_user_id = 0;
volatile int g_stop = 0;

volatile int g_proced_pkg_cnt = 0;

void sigterm_handler(int signo)
{
    g_stop = 1;
}

void install_sigact()
{
    struct sigaction sa;
    sigset_t sset;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigterm_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);
    signal(SIGPIPE,SIG_IGN);
    signal(SIGCHLD,SIG_IGN);
}

typedef struct {
    uint32_t pkg_len;
    uint32_t seq_num;
    uint16_t cmd_id;
    uint32_t status_code;
    uint32_t user_id;
}__attribute__((packed)) common_protocol_header_t;

typedef struct {
    common_protocol_header_t h;
    int need_filter;
    int sleep_sec;
    char val[32];
}__attribute__((packed)) mb_test_filter_req_t;

typedef struct {
    common_protocol_header_t h;
    char val[32];
}__attribute__((packed)) mb_test_filter_ack_t;

int connect_svr()
{
    int sf = socket(AF_INET, SOCK_STREAM, 0);
    if (sf < 0) {
        cout << "Error open socket." << endl;
        return -1;
    }

    if (connect(sf, (struct sockaddr *) &g_dest_addr, sizeof(g_dest_addr)) != 0) {
        cout << "Connect to svr fail" << endl;
        close(sf);
        return -1;
    }

    return sf;
}

void* thread_fun_long_conn(void* arg)
{
    int fd = connect_svr();
    if (fd < 0) {
        cout << "Connect fail" << endl;
        return NULL;
    }

    /// 组包
    mb_test_filter_req_t req;
    memset(&req, 0, sizeof(req));
    req.h.pkg_len = sizeof(mb_test_filter_req_t);
    req.h.cmd_id = 0x1002;
    req.sleep_sec = g_proc_wait;

    srand(time(NULL));
    for (int i = 0; i < g_pkg_cnt; i++) {
        if (g_stop) {
            break;
        }

        if (g_user_id != 0) {
            req.h.user_id = g_user_id;
        }
        else {
            req.h.user_id = (int)(((double)rand() / (double)(RAND_MAX)) * 200);
        }

        if (g_filter_flag != -1) {
            req.need_filter = g_filter_flag;
        }
        else {
            req.need_filter = (double)rand()/(double)(RAND_MAX) > 0.5 ? 1 : 0;
        }

        sprintf(req.val, "%d", i);
        int ret = send(fd, (void*)&req, sizeof(req), 0);
        if (ret != sizeof(req)) {
            cout << "Send fail: " << ret << endl;
            break;
        }

        mb_test_filter_ack_t ack;
        ret = recv(fd, (void*)&ack, sizeof(ack), 0);
        if (ret != sizeof(ack)) {
            cout << "Recv fail: " << ret << endl;
            break;
        }

        //cout << "Send sleep: " << req.sleep_sec << ", usr: " << setw(3) << setfill('0') << req.h.user_id << ", filter: " << req.need_filter << ", val: " << setw(6) << setfill('0') << req.val << ", " << "recv: " << setw(6) << setfill('0') << ack.val << endl;

        if (g_pkg_inter != 0) {
            sleep(g_pkg_inter);
        }

        g_proced_pkg_cnt ++;
    }

    close(fd);
    cout << "Thread exited" << endl;
    return NULL;
}

int main(int argc, char* argv[])
{
    /// 读取参数
    if (argc < 5 || argc > 9) {
        cout << "Usage: " << argv[0] << " ip port thread_cnt pkg_cnt pkg_inter proc_wait filter_flag user_id" << endl;
        return 0;
    }

    if (argc >= 6) {
        g_pkg_inter = atoi(argv[5]);
    }
    if (argc >= 7) {
        g_proc_wait = atoi(argv[6]);
    }
    if (argc >= 8) {
        g_filter_flag = atoi(argv[7]);
    }
    if (argc == 9) {
        g_user_id = atoi(argv[8]);
    }

    bzero(&g_dest_addr, sizeof(g_dest_addr));
    g_dest_addr.sin_family = AF_INET;
    g_dest_addr.sin_port = htons(atoi(argv[2]));
    if (inet_aton(argv[1], (struct in_addr *) &g_dest_addr.sin_addr.s_addr) == 0) {
        cout << "Error addr: " << argv[1] << endl;
        return 0;
    }

    /// 测试和服务端的连接
    int fd = connect_svr();
    if (fd < 0) {
        cout << "Connect to " << argv[1] << ":" << argv[2] << " fail." << endl;
        return 0;
    }
    close(fd);

    g_thread_cnt = atoi(argv[3]);
    g_pkg_cnt = atoi(argv[4]);

    install_sigact();

    /// 创建线程
    if (g_thread_cnt <= 0) {
        cout << "No thread to create." << endl;
        return 0;
    }

    pthread_t* tids = (pthread_t*)malloc(g_thread_cnt * sizeof(pthread_t));
    if (tids == NULL) {
        cout << "Malloc pthread fail." << endl;
        return 0;
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(int t = 0; t < g_thread_cnt; t++) {
        cout << "Main: creating thread " <<  t << endl;
        int rc = pthread_create(&tids[t], &attr, thread_fun_long_conn, (void *)t);
        if (rc) {
            cout << "ERROR; return code from pthread_create() is " << rc << endl;
            return -1;
        }
    }

    int start_time = time(NULL);
    int old_proced_pkg_cnt = g_proced_pkg_cnt;
    int old_time = time(NULL);
    while (! g_stop) {
        int new_time = time(NULL);
        if (new_time != old_time) {
            cout << "spd: " << (g_proced_pkg_cnt - old_proced_pkg_cnt) / (new_time - old_time) << ", avg: " <<
                g_proced_pkg_cnt / (new_time - start_time) << endl;

            old_proced_pkg_cnt = g_proced_pkg_cnt;
            old_time = new_time;
        }

        sleep(2);
    }

    pthread_attr_destroy(&attr);
    for(int t = 0; t < g_thread_cnt; t++) {
        void* status = NULL;
        int rc = pthread_join(tids[t], &status);
        if (rc) {
           cout << "ERROR; return code from pthread_join() is : "<< rc << endl;
           return -1;
        }
        cout << "Main: completed join with thread " << t << " having a status of " << (long)status << endl;
    }

    cout << "Main: program completed. Exiting." << endl;
    pthread_exit(NULL);
    return 0;
}
