#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

struct protohead_t
{
	uint32_t len;
	uint32_t seq;
    char buf[];
}__attribute__((packed));

#define PRINT(ptr, n)
//#define PRINT(ptr, n) for (int i = 0; i < n; ++i) { printf("%2X", (uint8_t)ptr[i]); } printf ("\n")

#define SEND(fd, ptr, n) { \
            if (n != send(fd, ptr, n, 0)) \
            { \
                printf("sendto failed\n"); \
                exit(0); \
            } \
        }
        
#define RECV(fd, ptr, n) \
        for (int pos = 0; pos != n;) { \
            int r = recv(fd, ptr+pos, n-pos, 0); \
            if (r <= 0) { \
                printf("recv error\n"); \
                exit(0); \
            } \
            pos += r; \
        }
                
int main (int argc, char *argv[])
{
    if (argc != 5) {
        printf("args: ip port o_cnt cnt\n");
        return 0;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    inet_aton(argv[1], &(server_addr.sin_addr));
    server_addr.sin_port = htons(atoi(argv[2]));

    int o_cnt = atoi(argv[3]);
    int cnt = atoi(argv[4]);

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (-1 == connect(fd, (sockaddr *)&server_addr, sizeof(sockaddr_in))) {
        printf("connect failed\n");
        return 0;
    }
    
    char bufs[2000000];
    char bufr[2000000];

    memset(bufs, 0xAA, sizeof(bufs));
    memset(bufr, 0xFF, sizeof(bufr));
    protohead_t *ph;

    struct timeval begin, end;
    gettimeofday(&begin, NULL);

    for (int i = 0; i < o_cnt; ++i) {
        for (int j = 0; j < cnt; ++j) {
            ph = (protohead_t*)(bufs + 0);
            if (100 + j >= sizeof(bufs))
                ph->len = sizeof(bufs);
            else
                ph->len = 100 + j;
            SEND(fd, bufs, ph->len);
        }

        //gettimeofday(&end, NULL);
        //printf("time:%llu\n", ((end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec)) / 1000);

        for (int j = 0; j < cnt; ++j) {
            uint32_t len;
            if (100 + j >= sizeof(bufs))
                len = sizeof(bufs);
            else
                len = 100 + j;
            //printf("cnt:%d, len:%u\n", j, len);
            RECV(fd, bufr, len);
            PRINT(bufr, len);
        }
    }

    gettimeofday(&end, NULL);
    printf("time:%llu\n", ((end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec)) / 1000);

	return 0;
}
