#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

//config
#include <sys/mman.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "newbench.h"
#include "util.h"


static char g_default_ifs[256] = {0};

void inline build_ifs(char *tifs, const u_char *ifs0)
{
    const u_char *ifs = ifs0;
    memset(tifs, 0, 256);
    while(*ifs) tifs[*ifs++] = 1;
}

void inline make_default_ifs()
{
    g_default_ifs[9] = 1;
    g_default_ifs[10] = 1;
    g_default_ifs[13] = 1;
    g_default_ifs[32] = 1;
}

/*
 * NULL IFS: default blanks
 * first byte is NULL, IFS table
 * first byte is NOT NULL, IFS string
 */
int str_explode(const char *ifs, char *line0, char *field[], int n)
{
    char *line = line0;
    int i;

    make_default_ifs();
    if(ifs==NULL) {
        ifs = g_default_ifs;
    } else if(*ifs) {
        char *implicit_ifs = (char*)alloca(256);
        build_ifs(implicit_ifs, (const u_char*)ifs);
        ifs = implicit_ifs ;
    }

    i = 0;
    while(1) {
        while(ifs[(u_char)*line]) line++;
        if(!*line) break;
        field[i++] = line;
        if(i>=n) {
            line += strlen(line)-1;
            while(ifs[(u_char)*line]) line--;
            line[1] = '\0';
            break;
        }
        while(*line && !ifs[(u_char)*line]) line++;
        if(!*line) break;
        *line++ = '\0';
    }

    return i;
}

/**
 * @brief  加载bind文件
 * @param
 * @return 0:success, -1:fail
 */
int load_bind_file(const char *file_name, bind_config_t **pbind_list)
{
    int len;
    char *buf;

    len = mmap_bind_file(file_name, &buf);
    if (len <= 0)
    {
        NB_BOOT_LOG(-1, "mmap_bind_file failed!");
        return -1;
    }

    struct bind_config *bc = NULL;
    struct bind_config *tail = NULL;
    char *field[4];
    char *start = buf;
    len = strlen(buf);
    while (start < buf + len)
    {
        char *end = strchr(start, '\n');
        if (*end)
        {
            *end = '\0';
        }

        if (*start != '#' && str_explode(NULL, start, field, 4) == 4)
        {
            bc = (struct bind_config *) malloc(sizeof(struct bind_config));

            bc->bind_ip = strdup(field[0]);
            bc->bind_port = atoi(field[1]);
            if (!strcasecmp(field[2], "tcp"))
            {
                bc->type = SOCK_STREAM;
            }
            else if(!strcasecmp(field[2], "udp"))
            {
                bc->type = SOCK_DGRAM;
            }
            else
            {
                NB_BOOT_LOG(-1, "Invalid socket type:%s", field[2]);
                free(bc->bind_ip);
                free(bc);
                break;
            }
            //bc->timeout = tscsec * atoi (field[3]);
            bc->timeout = atoi(field[3]) * 1000;//转换为秒
            bc->next = NULL;

            //insert into list
            if (*pbind_list == NULL || tail == NULL)
            {
                *pbind_list = bc;
                tail = bc;
            }
            else
            {
                tail->next = bc;
                tail = bc;
            }
        }
        start = end + 1;
    }

    munmap(buf, len);
    int ret_code = check_bind_config(*pbind_list);
    if (ret_code != 0)
    {
        free_bind_file(*pbind_list);
    }

    //DEBUG_LOG("Load bind file: %s", file_name);
    return ret_code;
}


/**
 * @brief  将绑定文件信息加载到内存
 * @param
 * @return 0:success, -1:fai
 */
int mmap_bind_file(const char *file_name, char **buf)
{
    int fd;
    int len;
    int ret_code = -1;

    fd = open(file_name, O_RDONLY);
    if (fd < 0)
    {
        NB_BOOT_LOG(-1, "open %s error, cwd=%s", file_name, get_current_dir_name());
        return -1;
    }
    len = lseek(fd, 0L, SEEK_END);
    lseek(fd, 0L, SEEK_SET);

    *buf = (char *)mmap(0, len + 1, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (*buf == MAP_FAILED)
    {
        NB_BOOT_LOG(-1, "mmap failed, length=%d, %m", len + 1);
    }
    else
    {
        read(fd, *buf, len);
        (*buf)[len] = 0;
        ret_code = len + 1;
    }

    close(fd);
    return ret_code;
}


/**
 * @brief  检查绑定配置信息
 * @param
 * @return 0:success, -1:fail
 */
int check_bind_config (struct bind_config *bind_list)
{
    struct bind_config *bc;
    int cnt = 0;

    for (bc = bind_list; bc; bc = bc->next)
    {
        if (INADDR_NONE == inet_addr (bc->bind_ip))
        {
            NB_BOOT_LOG(-1, "Invalid ip address:%s", bc->bind_ip);
            return -1;
        }
        if (bc->bind_port == 0)
        {
            NB_BOOT_LOG(-1, "Invalid bind port:%u", bc->bind_port);
            printf("Invalid bind port:%u\n", bc->bind_port);
            return -1;
        }
        cnt++;
    }
    if (cnt == 0)
    {
        NB_BOOT_LOG(-1, "Bind port not found");
        return -1;
    }

    return 0;
}


/**
 * @brief  释放绑定列表空间
 * @param
 * @return
 */
void free_bind_file(struct bind_config *bind_list)
{
    struct bind_config *bc = NULL;
    struct bind_config *bc_next = bind_list;

    while (bc_next)
    {
        bc = bc_next;
        bc_next = bc_next->next;

        if (bc->bind_ip)
        {
            free(bc->bind_ip);
            bc->bind_ip = NULL;
        }
        free(bc);
        bc = NULL;
    }
}


/**
 * @brief  记录绑定信息(IP、端口号等)
 * @param
 * @return
 */
void bind_dump(struct bind_config *bind_list)
{
    struct bind_config *bc = NULL;
    for (bc = bind_list; bc; bc = bc->next)
    {
        DEBUG_LOG("Bind port %s:%u, type %s, idle timeout %d", bc->bind_ip, bc->bind_port,
            bc->type == SOCK_STREAM ? "TCP" : "UDP", bc->timeout);
    }
}

