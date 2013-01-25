#ifndef DMIDECODE_H
#define DMIDECODE_H

#define MAX_STR_LEN 128
#ifdef __BEOS__
#define DEFAULT_MEM_DEV "/dev/misc/mem"
#else
#ifdef __sun
#define DEFAULT_MEM_DEV "/dev/xsvc"
#else
#define DEFAULT_MEM_DEV "/dev/mem"
#endif
#endif
#ifndef __BEOS__
#define USE_MMAP
#endif
#ifdef __ia64__
#define ALIGNMENT_WORKAROUND
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short i16;
typedef unsigned int u32;

#ifdef ALIGNMENT_WORKAROUND
#	ifdef BIGENDIAN
#	define WORD(x) (u16)((x)[1] + ((x)[0] << 8))
#	define DWORD(x) (u32)((x)[3] + ((x)[2] << 8) + ((x)[1] << 16) + ((x)[0] << 24))
#	define QWORD(x) (U64(DWORD(x + 4), DWORD(x)))
#	else /* BIGENDIAN */
#	define WORD(x) (u16)((x)[0] + ((x)[1] << 8))
#	define DWORD(x) (u32)((x)[0] + ((x)[1] << 8) + ((x)[2] << 16) + ((x)[3] << 24))
#	define QWORD(x) (U64(DWORD(x), DWORD(x + 4)))
#	endif /* BIGENDIAN */
#else /* ALIGNMENT_WORKAROUND */
#define WORD(x) (u16)(*(const u16 *)(x))
#define DWORD(x) (u32)(*(const u32 *)(x))
#define QWORD(x) (*(const u64 *)(x))
#endif /* ALIGNMENT_WORKAROUND */

typedef struct
{
    unsigned long size;	      ///memory size, ==0 No Module Installed, ==~0x0 Unknow
    char type[MAX_STR_LEN];   ///memory type
    char locator[MAX_STR_LEN];///插槽信息
}mem_info_t;

const char *dmi_string(const struct dmi_header *dm, u8 s);
void convert_to_dmi_header(dmi_header*, u8*);
unsigned long dmi_memory_device_size(u16);
unsigned long dmi_memory_device_extended_size(u32);
const char* dmi_memory_device_type(u8);

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
char        *server_tag, 
unsigned int server_tag_len
);

int update_dmi_info(
mem_info_t   *mem_info_list, 
unsigned int *mem_info_count,
char         *server_pattern,
unsigned      server_pattern_len,
char         *server_tag,
unsigned      server_tag_len
);

#endif// DMIDECODE_H
