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
                
#define CHECK(buf1, buf2, n) \
        for (int pos = 0; pos != n; ++pos) { \
            if (buf1[pos] != buf2[pos]) { \
                printf("buf check error\n"); \
                break; \
            } \
        }
         
int main (int argc, char *argv[])
{
    if (argc != 4) {
        printf("args: ip port cnt\n");
        return 0;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    inet_aton(argv[1], &(server_addr.sin_addr));
    server_addr.sin_port = htons(atoi(argv[2]));

    int cnt = atoi(argv[3]);

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (-1 == connect(fd, (sockaddr *)&server_addr, sizeof(sockaddr_in))) {
        printf("connect failed\n");
        return 0;
    }
    
    char bufs[20000];
    char bufr[20000];

    protohead_t *ph;

    struct timeval begin, end;
    gettimeofday(&begin, NULL);

    for (int j = 0; j < cnt; ++j) {
        memset(bufs, 0xAA, sizeof(bufs));
        memset(bufr, 0xFF, sizeof(bufr));
        ph = (protohead_t*)(bufs + 0);
        ph->len = 10;
        SEND(fd, bufs, ph->len);
        RECV(fd, bufr, ph->len);
        CHECK(bufs, bufr, ph->len);
        PRINT(bufr, ph->len);
    }

    gettimeofday(&end, NULL);
    printf("time:%llu\n", ((end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec)) / 1000);

	return 0;
}
