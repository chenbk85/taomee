#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

#ifdef USE_MMAP
#include <sys/mman.h>
#ifndef MAP_FAILED
#define MAP_FAILED ((void *) -1)
#endif /* !MAP_FAILED */
#endif /* USE MMAP */

#include "dmidecode.h"
extern "C"
{
#include <libtaomee/log.h>
}

#define out_of_spec "<OUT OF SPEC>"
static const char *bad_index = "<BAD INDEX>";

struct dmi_header
{
    u8 type;
    u8 length;
    u16 handle;
    u8 *data;
};

static const char *type[] = 
{
    "Other", /* 0x01 */
    "Unknown",
    "DRAM",
    "EDRAM",
    "VRAM",
    "SRAM",
    "RAM",
    "ROM",
    "Flash",
    "EEPROM",
    "FEPROM",
    "EPROM",
    "CDRAM",
    "3DRAM",
    "SDRAM",
    "SGRAM",
    "RDRAM",
    "DDR",
    "DDR2",
    "DDR2 FB-DIMM",
    "Reserved",
    "Reserved",
    "Reserved",
    "DDR3",
    "FBD2", /* 0x19 */
};

static int myread(int fd, u8 *buf, size_t count)
{
    ssize_t r = 1;
    size_t r2 = 0;

    while(r2 != count && r != 0) 
    {
        r = read(fd, buf + r2, count - r2);
        if(r == -1)
        {
            if(errno != EINTR)
            {
                close(fd);
                return -1;
            }
        }
        else 
        {
            r2 += r;
        }
    }

    if(r2 != count)
    {
        close(fd);
        ERROR_LOG("Unexpected end of file.");
        return -1;
    }
    return 0;
}

bool checksum(const u8 *buf, size_t len)
{
    u8 sum = 0;
    size_t a;
    for(a = 0; a < len; a++)
        sum += buf[a];
    return (sum == 0);
}

void *mem_chunk(size_t base, size_t len, const char *devmem)
{
    void *p;
    int   fd;
#ifdef USE_MMAP
    size_t mmoffset;
    void  *mmp;
#endif

    if((fd = open(devmem, O_RDONLY)) == -1)
    {
        ERROR_LOG("Open file %s failed,sys error:%s", devmem, strerror(errno));
        return NULL;
    }

    if((p = malloc(len)) == NULL) 
    {
        ERROR_LOG("Malloc failed,sys error:%s", strerror(errno));
        return NULL;
    }

#ifdef USE_MMAP
#ifdef _SC_PAGESIZE
    mmoffset = base % sysconf(_SC_PAGESIZE);
#else
    mmoffset = base % getpagesize();
#endif
    mmp = mmap(0, mmoffset + len, PROT_READ, MAP_SHARED, fd, base - mmoffset);
    if(mmp == MAP_FAILED)
    {
        goto try_read;
    }

    memcpy(p, (u8 *)mmp + mmoffset, len);

    if(munmap(mmp, mmoffset + len) == -1)
    {
        ERROR_LOG("Mmap file failed,sys error:%s", strerror(errno));
    }
    goto out;

#endif 

try_read:
    if(lseek(fd, base, SEEK_SET) == -1) 
    {
        ERROR_LOG("lseek file failed,sys error:%s", strerror(errno));
        free(p);
        return NULL;
    }

    if(myread(fd, (u8 *)p, len) == -1)
    {
        ERROR_LOG("read file failed,sys error:%s", strerror(errno));
        free(p);
        return NULL;
    }

out:
    if(close(fd) == -1) 
    {
        ERROR_LOG("read file failed,sys error:%s", strerror(errno));
    }

    return p;
}

const char * dmi_string(const struct dmi_header *dm, u8 s)
{
    char *bp = (char *)dm->data;
    size_t i, len;
    if(s == 0)
    {
        return "Not Specified";
    }

    bp += dm->length;
    while(s > 1 && *bp)
    {
        bp += strlen(bp);
        bp++;
        s--;
    }

    if(!*bp)
    {
        return bad_index;
    }

    len = strlen(bp);
    for(i = 0; i < len; i++)
        if(bp[i] < 32 || bp[i] == 127)
            bp[i] = '.';

    return bp;
}

void convert_to_dmi_header(struct dmi_header *h, u8 *data)
{
    h->type = data[0];
    h->length = data[1];
    h->handle = WORD(data + 2);
    h->data = data;
}

unsigned long dmi_memory_device_size(u16 code)
{
    if(code == 0xFFFF) {
        return ~0x0;
    } else if(code & 0x8000) {
        return (unsigned long)code >>10;
    } else {
        return (unsigned long)code;
    }
}

unsigned long dmi_memory_device_extended_size(u32 code)
{
    code &= 0x7FFFFFFFUL;
    return (unsigned long)code;
}

const char * dmi_memory_device_type(u8 code)
{
    if(code >= 0x01 && code <= 0x19)
        return type[code - 0x01];
    return out_of_spec;
}

void dmi_table_lookup(
u32           base, 
u16           len, 
u16           num, 
const char   *devmem, 
mem_info_t   *mem_info_list, 
unsigned      max_mem_info_list, 
unsigned     *mem_info_idx, 
char         *server_pattern_buf, 
unsigned int server_pattern_buf_len, 
char        *server_tag_buf, 
unsigned int server_tag_buf_len
)
{
    u8 *buf = NULL;
    u8 *data = NULL;
    int i = 0;

    if((buf = (u8 *)mem_chunk(base, len, devmem)) == NULL) 
    {
        return;
    }

    data = buf;
    ///4 is the length of an SMBIOS structure header
    while(i < num && data + 4 <= buf + len) 
    {
        u8 *next;
        struct dmi_header h;

        convert_to_dmi_header(&h, data);

        if(h.length < 4)
        {
            break;
        }

        next = data + h.length;
        while(next - buf + 1 < len && (next[0] != 0 || next[1] != 0)) 
        {
            next++;
        }
        next += 2;		

        if(next - buf <= len)
        {
            if(h.type == 1)// 系统
            {
                const u8 *system_data = h.data;
                strncpy(server_pattern_buf, dmi_string(&h, system_data[0x05]), server_pattern_buf_len);
                strncpy(server_tag_buf, dmi_string(&h, system_data[0x07]), server_tag_buf_len);
            }
            if(h.type == 17)// 内存
            {
                const u8 *mem_data = h.data;
                const char *str = NULL;
                if(h.length < 0x15) goto next;
                if(*mem_info_idx >= max_mem_info_list) break;
                if(h.length >= 0x20 && WORD(mem_data + 0x0C) == 0x7FFF)
                {
                    mem_info_list[*mem_info_idx].size = dmi_memory_device_extended_size(DWORD(mem_data + 0x1C));
                }
                else 
                {
                    mem_info_list[*mem_info_idx].size = dmi_memory_device_size(WORD(mem_data + 0x0C));
                }
                str = dmi_string(&h, mem_data[0x10]);
                strncpy(mem_info_list[*mem_info_idx].locator, str, sizeof(mem_info_list[*mem_info_idx].locator));
                str = dmi_memory_device_type(mem_data[0x12]);
                strncpy(mem_info_list[*mem_info_idx].type, str, sizeof(mem_info_list[*mem_info_idx].type));
                (*mem_info_idx)++;
            }
            ///其他需要获取的信息可以加在这里
        }
next:
        data = next;
        i++;
    }

    free(buf);
}

int update_dmi_info(
mem_info_t   *mem_info_list, 
unsigned int *mem_info_count,
char         *server_pattern,
unsigned     server_pattern_len,
char         *server_tag,
unsigned     server_tag_len
)
{
    size_t fi = 0;
    u8    *buf = NULL;

    //将文件映射到内存中
    if((buf = (u8 *)mem_chunk(0xF0000, 0x10000, DEFAULT_MEM_DEV)) == NULL) 
    {
        ERROR_LOG("Mmap file %s failed.", DEFAULT_MEM_DEV);
        return -1;
    }

    unsigned max_mem_info_count = *mem_info_count;
    unsigned mem_info_idx = 0;
    for(fi = 0; fi <= 0xFFF0; fi += 16)
    {
        if(memcmp(buf + fi, "_SM_", 4) == 0 && fi <= 0xFFE0)
        {
            u8 *start = buf + fi;
            //遍历16个字节为一个单位的块,每个块前后都有校验码
            if(!checksum(start, start[0x05]) || memcmp(start + 0x10, "_DMI_", 5) != 0 || !checksum(start + 0x10, 0x0F))
            {
                ERROR_LOG("Invalid checksum of this chunk.");
                continue;
            }
            dmi_table_lookup(DWORD(start + 0x18), WORD(start + 0x16),
                    WORD(start + 0x1C), DEFAULT_MEM_DEV, 
                    mem_info_list, max_mem_info_count, &mem_info_idx,
                    server_pattern, server_pattern_len,
                    server_tag, server_tag_len);
            fi += 16;
        }
        else if(memcmp(buf + fi, "_DMI_", 5) == 0)
        {
            u8 *start = buf + fi;
            if(!checksum(start, 0x0F)) 
            {
                continue;
            }
            dmi_table_lookup(DWORD(start + 0x08), WORD(start + 0x06), 
                    WORD(start + 0x0C), DEFAULT_MEM_DEV,
                    mem_info_list, max_mem_info_count, &mem_info_idx, 
                    server_pattern, server_pattern_len,
                    server_tag, server_tag_len);
        }
    }

    free(buf);
    *mem_info_count = mem_info_idx;
    return 0;
}
