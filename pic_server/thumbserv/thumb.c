
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <glib.h>
#include <libtaomee/log.h>
#include <iter_serv/net_if.h>
#include "proto.h"
#include "util.h"
#include "thumb.h"


#define MAX_PHOTOS_SIZE 		(128*1024)
#define MIN_PHOTOS_BLOCK_SIZE 	(128*1024)
#define MIN_BLOCK_SIZE 			1024
#define PHOTOS_HEAD_SIZE 		1024
#define ALBUM_ID_SIZE 			(500 * 1000 * 1000)
#define ALBUM_DIR_SIZE 			1000
#define FIELNO_SIZE 			16
#define FREE_FALG 				"FREE"
#define FREE_FALG_LEN 			4
#define MAX_ALBUM_THUM_CNT		(MAX_PIC_NUM_PER_ABLUM * MAX_THUMB_CNT_PER_PIC)

#define OFFSET_BIT				15
#define OFFSET_MASK				0x7FFF
#define MAX_PHOTO_OFFSET		(32 * 1024)
#define MAX_PHOTO_BLOCK_SIZE	(MIN_BLOCK_SIZE * MAX_PHOTO_OFFSET)

typedef struct {
    uint32_t		size;
    uint32_t		photoscount;    
} photos_head_t;

typedef struct {
    uint32_t       photosid;
    uint32_t       stime;
} photos_info_t;

typedef struct {
    uint32_t       nextoffset;
    char            ch_flag[4];
} free_block_t;

typedef struct{
    uint32_t ver;
	uint32_t first_free_no;
	uint32_t block_free_count;
	uint32_t block_count;
} file_info_t;

//extern FILE * g_logFp;
int *pmaps_offset[ALBUM_DIR_SIZE];
file_info_t *pmaps_file_info[ALBUM_DIR_SIZE];
#define MAX_MMAP_SIZE	2 * 1024 * 1024
char gbuff[MAX_MMAP_SIZE] = {0};
int init_mmaps()
{
    int lp = 0;
    int fd;
    char *pmmap = NULL;
    char ch_file_path[MAX_FIEL_PATH];
    for(lp = 0; lp < ALBUM_DIR_SIZE; lp++) {
        pmaps_file_info[lp] = NULL;
        pmaps_offset[lp] = NULL;
    
        snprintf(ch_file_path, sizeof(ch_file_path), "%s/%03d/%s", PHOTOS_ROOT_PATH, lp, PHOTOS_FIEL_PRE);
        if((fd = open(ch_file_path, O_RDWR)) < 0) {
             ERROR_LOG("open file %s error", ch_file_path);
        }
        else {
			long fsize = lseek(fd, 0, SEEK_END);
			if (fsize == 0) {
				write(fd, gbuff, MAX_MMAP_SIZE);
			}
			int mmap_size = (ALBUM_ID_SIZE/ALBUM_DIR_SIZE) * sizeof(int) + FIELNO_SIZE * sizeof(file_info_t);
            pmmap = (char *)mmap(0, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if(pmmap == (char *)-1) {
                 ERROR_LOG("open mmap file %s error", ch_file_path);
                 close (fd);
                 return -1;
            }
			memset(pmmap, 0, sizeof(mmap_size));
            pmaps_file_info[lp] = (file_info_t*)pmmap;
            pmaps_offset[lp] = (int*)(pmmap + FIELNO_SIZE * sizeof(file_info_t));
            close (fd);
        }
    }
    return 0;        
}

int save_photo(uint32_t albumid, uint32_t photos_len, char *pphotos_buf)
{
    int ret = 0;
    long long lloffset;
    unsigned int dirid = albumid % ALBUM_DIR_SIZE;
    if(photos_len == 0 || pphotos_buf == NULL 
		|| pmaps_offset[dirid] == NULL || pmaps_file_info[dirid] == NULL)
		return 0;
	if (photos_len > MAX_PHOTOS_SIZE) {
		ERROR_RETURN(("too big thumb\t[%u %u]", albumid, photos_len), 0);
	}
	DEBUG_LOG("SAVE PHOTO\t[%u %u]", albumid, photos_len);
    int i = 0;
    uint32_t ioffset = pmaps_offset[dirid][albumid/ALBUM_DIR_SIZE] >> 8;
    uint32_t ifileno = pmaps_offset[dirid][albumid/ALBUM_DIR_SIZE] & 0xFF;
    uint32_t photos_block_len = MIN_PHOTOS_BLOCK_SIZE * (1 << ifileno);
    uint32_t need_size = ((photos_len + sizeof(photos_info_t)) / MIN_BLOCK_SIZE + 1) * MIN_BLOCK_SIZE;
    char ch_file_path[MAX_FIEL_PATH];
    char ch_photos_head[PHOTOS_HEAD_SIZE];
    char ch_write_buf[MAX_PHOTOS_SIZE];
    photos_head_t *phead = (photos_head_t*)ch_photos_head;
    uint32_t* pfree_list = NULL;
    uint32_t  save_offset = 0;
    free_block_t next_free;
    photos_info_t photo_info;
    photo_info.photosid = photos_len << OFFSET_BIT;
    photo_info.stime = time(NULL);
    snprintf(ch_file_path, sizeof(ch_file_path),"%s/%03d/%s%d",PHOTOS_ROOT_PATH, dirid, PHOTOS_FIEL_PRE, ifileno);

	DEBUG_LOG("NEED SIZE\t[%u %u]", albumid, need_size);
    int fd = open64(ch_file_path, O_RDWR|O_CREAT, 0666);
    if(fd < 0) ERROR_RETURN(("open file %s error", ch_file_path), -1);
    if(ioffset == 0 && ifileno == 0) {  // the first photo in album
        memset(ch_write_buf , 0 , MAX_PHOTOS_SIZE);
        phead = (photos_head_t*)ch_write_buf;
        phead->size = PHOTOS_HEAD_SIZE + need_size;
        save_offset = 1;  // 
        photo_info.photosid |= save_offset;
        phead->photoscount = 1;
		
		DEBUG_LOG("ALBUM SIZE\t[%u %u %u]", albumid, phead->size, phead->photoscount);
        memcpy(ch_write_buf + PHOTOS_HEAD_SIZE, &photo_info, sizeof(photos_info_t));
        memcpy(ch_write_buf + PHOTOS_HEAD_SIZE + sizeof(photos_info_t), pphotos_buf, photos_len);
        
		//uint32_t   write_len = PHOTOS_HEAD_SIZE + photos_len + sizeof(photos_info_t);
		uint32_t   write_len = PHOTOS_HEAD_SIZE + need_size;
        ioffset = pmaps_file_info[dirid][ifileno].first_free_no;
        if(ioffset == 0){  // can not find freed block, append in the file
            if(pmaps_file_info[dirid][ifileno].block_count == 0) // the first block need to be reserved
                pmaps_file_info[dirid][ifileno].block_count = 1;
            ioffset = pmaps_file_info[dirid][ifileno].block_count;
            lloffset = ioffset;
            lloffset *= photos_block_len;
			
			DEBUG_LOG("WRITE-THUMB-0\t[b_key=%u %s %llu %u %u]", albumid, ch_file_path, lloffset, save_offset, write_len);
            if((ret = pwrite64(fd , ch_write_buf ,  write_len, lloffset)) != write_len) {
                photo_info.photosid = 0;
            }
            else {
                pmaps_file_info[dirid][ifileno].block_count++;
            }
            
        }
        else{
            lloffset = ioffset;
            lloffset *= photos_block_len;
            if((ret = pread64(fd , &next_free , sizeof(free_block_t) , lloffset)) != sizeof(free_block_t)) {
				ERROR_LOG("read data %u bytes", ret);
                photo_info.photosid = 0;
            }
            else{
				// if  the freed block write error, then append the thumb in album
                if(memcmp(next_free.ch_flag, FREE_FALG, FREE_FALG_LEN) != 0) {
                    if(pmaps_file_info[dirid][ifileno].block_count == 0)
                    	pmaps_file_info[dirid][ifileno].block_count = 1;
                    ioffset = pmaps_file_info[dirid][ifileno].block_count;
                    lloffset = ioffset;
                    lloffset *= photos_block_len;
					DEBUG_LOG("WRITE-THUMB-1\t[b_key=%u %s %llu %u %u]", albumid, ch_file_path, lloffset, save_offset, write_len);
                    if((ret = pwrite64(fd , ch_write_buf , write_len , lloffset)) != write_len) {
                        photo_info.photosid = 0;
						ERROR_LOG("write file %s error", ch_file_path);
                    }
                    else{
                        pmaps_file_info[dirid][ifileno].block_count++;
                    }
                }
                else{ // the freed block can be used
                	DEBUG_LOG("WRITE-THUMB-2\t[b_key=%u %s %llu %u %u]", albumid, ch_file_path, lloffset, save_offset, write_len);
                    if((ret = pwrite64(fd , ch_write_buf , write_len, lloffset)) != write_len) {
                        photo_info.photosid = 0;
                        if(pwrite64(fd , &next_free , sizeof(free_block_t) , lloffset) != sizeof(free_block_t))
                        {
                        }
                    }
                    else{
                        pmaps_file_info[dirid][ifileno].first_free_no = next_free.nextoffset;
						DEBUG_LOG("FFN-A1\t[albumid=%u dir=%u fileno=%u f_br=%u u_nbr=%u]", albumid, dirid, ifileno + 1, next_free.nextoffset, ioffset);
                    }
					DEBUG_LOG("WRITE-THUMB-3\t[b_key=%u %u %u %u]", albumid, lloffset, save_offset, write_len);
                }
            }
        }
        if(photo_info.photosid != 0)
            pmaps_offset[dirid][albumid/ALBUM_DIR_SIZE] = ioffset << 8;
    }
    else { // not the first thumb in the album
        lloffset = ioffset;
        lloffset *= photos_block_len;
        if((ret = pread64(fd , ch_photos_head, PHOTOS_HEAD_SIZE , lloffset)) != PHOTOS_HEAD_SIZE) {
            close(fd);
            ERROR_RETURN(("read photos-head %s offset %lu error", ch_file_path, lloffset), 0);
        }
        pfree_list = (uint32_t *)(ch_photos_head + sizeof(photos_head_t));
        for(i = 0; i < 254; i++) {
            if(need_size == (pfree_list[i] >> OFFSET_BIT)){
                save_offset =  pfree_list[i] & OFFSET_MASK;
                pfree_list[i] = 0; // mean the block will be used
                break;
            }
        }
        if(save_offset == 0) { // cannot find the appropriate free block, then appended in the album
			if (phead->size + need_size >= MAX_PHOTO_BLOCK_SIZE) {
				close(fd);
                ERROR_RETURN(("thumb block too large\t[b_key=%u b_size=%u n_size=%u]", 
					albumid, phead->size, need_size), 0);
			}
			
            if(phead->size + need_size < photos_block_len) {
                save_offset = phead->size/MIN_BLOCK_SIZE;
                phead->size += need_size;
            }
            else { // the block file can not hold the album
                uint32_t im_photos_block_len = photos_block_len * 2;
                if(phead->size + need_size > photos_block_len * 2){
                    close(fd);
                    ERROR_RETURN(("thumb too large in album\t[b_key=%u b_size=%u n_size=%u 2b=%u]",
						albumid, phead->size, need_size, photos_block_len * 2), 0);
                }
                char *pbuf = (char *)malloc(phead->size + need_size);
                if(pbuf == NULL){
                    close(fd);
                    ERROR_RETURN(("malloc error"), 0);
                }
                //if((ret = pread64(fd , pbuf , photos_block_len , lloffset)) != photos_block_len) {
                errno = 0;
				if((ret = pread64(fd , pbuf , phead->size , lloffset)) != phead->size) {
					ERROR_LOG("read whole file %s offset %lu error[file size=%u block_len=%u errno=%u read_size=%u]", 
						ch_file_path, lloffset, phead->size, photos_block_len, errno, ret);
                    close(fd);
                    free(pbuf);
					return 0;
                }
                phead = (photos_head_t*)pbuf;
				// append thumb in the album
                save_offset = phead->size / MIN_BLOCK_SIZE;
                photo_info.photosid |= save_offset;
                memcpy(pbuf + phead->size, &photo_info, sizeof(photos_info_t));
                memcpy(pbuf + phead->size + sizeof(photos_info_t), pphotos_buf, photos_len);
                
                phead->size += need_size;
                phead->photoscount++;
                snprintf(ch_file_path,sizeof(ch_file_path),"%s/%03d/%s%d", PHOTOS_ROOT_PATH, albumid % ALBUM_DIR_SIZE, PHOTOS_FIEL_PRE, ifileno + 1);
				errno = 0;
				int mfd = open64(ch_file_path, O_RDWR|O_CREAT, 0666);
                if(mfd < 0) {
                    ERROR_LOG("open file %s error\t[err=%s]", ch_file_path, strerror(errno));
                    close(fd);
                    free(pbuf);
					return 0;
                }
                free_block_t m_next_free ;
                uint32_t imOffset = pmaps_file_info[dirid][ifileno + 1].first_free_no;
                long long mlloffset;
                if(imOffset == 0){ // no free block, appended in the block file
                    imOffset = pmaps_file_info[dirid][ifileno + 1].block_count;
                    mlloffset = imOffset;
                    mlloffset *= im_photos_block_len;
					DEBUG_LOG("WRITE-THUMBS-NF\t[b_key=%u %s %llu %u]", albumid, ch_file_path, mlloffset, phead->size);
                    if((ret = pwrite64(mfd , pbuf , phead->size , mlloffset)) != phead->size) {
                        photo_info.photosid = 0;
						ERROR_LOG("write file %s error", ch_file_path);
                    }
                    else {
                        pmaps_file_info[dirid][ifileno + 1].block_count++;
                    }
                }
                else { // have free block
                    mlloffset = imOffset;
                    mlloffset *= im_photos_block_len;
                    if((ret = pread64(mfd , &m_next_free , sizeof(free_block_t) , mlloffset)) != sizeof(free_block_t)) {
                        photo_info.photosid = 0;
						ERROR_LOG("read free-block %s offset %llu error:%s", ch_file_path, mlloffset, strerror(errno));
                    }
					// if the free block write error, then append in the block file
                    else if(memcmp(m_next_free.ch_flag, FREE_FALG, FREE_FALG_LEN) != 0) {
                        imOffset = pmaps_file_info[dirid][ifileno + 1].block_count;
                        mlloffset = imOffset;
                        mlloffset *= im_photos_block_len;
						
						DEBUG_LOG("WRITE-THUMBS-IF\t[b_key=%u %s %llu %u]", albumid, ch_file_path, mlloffset, phead->size);
                        if((ret = pwrite64(mfd , pbuf , phead->size , mlloffset)) != phead->size) {
                            photo_info.photosid = 0;
							ERROR_LOG("write file %s error", ch_file_path);
                        }
                        else{
                            pmaps_file_info[dirid][ifileno + 1].block_count++;
                        }
                    }
                    else { // the free block can be used   
						DEBUG_LOG("WRITE-THUMBS-RF\t[b_key=%u %s %llu %u]", albumid, ch_file_path, mlloffset, phead->size);
                        if((ret = pwrite64(mfd , pbuf , phead->size , mlloffset)) != phead->size)
                        {
                            photo_info.photosid = 0;
                            if((ret = pwrite64(mfd , &m_next_free , sizeof(free_block_t) , mlloffset)) != sizeof(free_block_t))
                            {
                            }
                        }
                        else{
                            pmaps_file_info[dirid][ifileno + 1].first_free_no = m_next_free.nextoffset;
							DEBUG_LOG("FFN-A2\t[albumid=%u dir=%u fileno=%u f_br=%u u_nbr=%u]", albumid, dirid, ifileno + 1, m_next_free.nextoffset, imOffset);
                        }
                    }
                }
                if(photo_info.photosid != 0){
                    pmaps_offset[dirid][albumid/ALBUM_DIR_SIZE] = imOffset << 8;
                    pmaps_offset[dirid][albumid/ALBUM_DIR_SIZE] |= ifileno + 1;
                    free_block_t stfree;
                    memcpy(stfree.ch_flag, FREE_FALG, FREE_FALG_LEN);
                    stfree.nextoffset = pmaps_file_info[dirid][ifileno].first_free_no;
					// set the block which have been moved free
                    if((ret = pwrite64(fd , &stfree , sizeof(free_block_t) , lloffset)) != sizeof(free_block_t))
                    {   
                    	ERROR_LOG("lost a free block\t[ifileno=%u lloffset=%u]", ifileno, lloffset);
                    }
                    else{
						DEBUG_LOG("FFN-A3\t[albumid=%u dir=%u fileno=%u f_br=%u s_nbr=%u]", albumid, dirid, ifileno, ioffset, stfree.nextoffset);
                        pmaps_file_info[dirid][ifileno].first_free_no = ioffset;
                    }
                }
                close(fd);
                close(mfd);
                free(pbuf);
                return photo_info.photosid;
            }
        }
        photo_info.photosid |= save_offset;
        memcpy(ch_write_buf, &photo_info, sizeof(photos_info_t));
        memcpy(ch_write_buf + sizeof(photos_info_t), pphotos_buf, photos_len);
        //if((ret = pwrite64(fd , ch_write_buf , photos_len + sizeof(photos_info_t) , lloffset + save_offset * MIN_BLOCK_SIZE))
			//!= photos_len + sizeof(photos_info_t)) {
		DEBUG_LOG("WRITE-THUMB-4\t[b_key=%u %s %llu %u %u]", albumid, ch_file_path, lloffset + save_offset * MIN_BLOCK_SIZE, save_offset, need_size);
		if((ret = pwrite64(fd , ch_write_buf , need_size, lloffset + save_offset * MIN_BLOCK_SIZE))
			!= need_size) {
            photo_info.photosid = 0;
        }
        else {
            phead->photoscount++;
            if((ret = pwrite64(fd , ch_photos_head , PHOTOS_HEAD_SIZE , lloffset)) != PHOTOS_HEAD_SIZE)
            {
                photo_info.photosid = 0;
            }
        }
		
    }
    close(fd);
    return photo_info.photosid;
}


int del_photo(uint32_t albumid, uint32_t photoid)
{
    int ret = 0;
    long long lloffset;
    uint32_t dirid = albumid % ALBUM_DIR_SIZE;
	if (photoid == 0)
		ERROR_RETURN(("invalid photo id\t[%u]", photoid), -1);
    if(pmaps_offset[dirid] == NULL || pmaps_file_info[dirid] == NULL)
		ERROR_RETURN(("album not exist1\t[%u]", albumid), -1);
    int i = 0;
    uint32_t ioffset = pmaps_offset[dirid][albumid/ALBUM_DIR_SIZE] >> 8;
    uint32_t ifileno = pmaps_offset[dirid][albumid/ALBUM_DIR_SIZE] & 0xFF;
    uint32_t photos_block_len = MIN_PHOTOS_BLOCK_SIZE * (1 << ifileno);
    if(ioffset == 0 && ifileno == 0) ERROR_RETURN(("album not exist2\t[%u]", albumid), -1);
	
    char ch_file_path[MAX_FIEL_PATH];
    char ch_photos_head[PHOTOS_HEAD_SIZE];
    photos_head_t *phead = (photos_head_t*)ch_photos_head;
    uint32_t *pfreelist = NULL;
    uint32_t  del_size = (((photoid >> OFFSET_BIT) + sizeof(photos_info_t))/MIN_BLOCK_SIZE + 1) * MIN_BLOCK_SIZE;
    uint32_t  del_offset = photoid & OFFSET_MASK;
    
    photos_info_t photo_info;
    snprintf(ch_file_path, sizeof(ch_file_path), "%s/%03d/%s%d", PHOTOS_ROOT_PATH, dirid, PHOTOS_FIEL_PRE, ifileno);
    
    int fd = open64(ch_file_path, O_RDWR|O_CREAT,0666);
    if(fd < 0) ERROR_RETURN(("open file %s error", ch_file_path), -1);
    lloffset = ioffset;
    lloffset *= photos_block_len;
    if((ret = pread64(fd , &photo_info , sizeof(photos_info_t) , lloffset + del_offset * MIN_BLOCK_SIZE )) != sizeof(photos_info_t)) {
        close(fd);
        ERROR_RETURN(("read photo-info %s offset %lu error", ch_file_path, lloffset + del_offset * MIN_BLOCK_SIZE), -1);
    }
    if(photo_info.stime == 0 || photo_info.photosid != photoid) {
        close(fd);
        ERROR_RETURN(("cannot find the file %u", photoid), -1);
    }
    photo_info.photosid = (del_size << OFFSET_BIT ) | del_offset;  // ???
    photo_info.stime = 0;
    
    if((ret = pread64(fd , ch_photos_head , PHOTOS_HEAD_SIZE , lloffset)) != PHOTOS_HEAD_SIZE) {
        close(fd);
        ERROR_RETURN(("read photos-head %s offset %lu error", ch_file_path, lloffset), -1);
    }
    if((ret = pwrite64(fd , &photo_info , sizeof(photos_info_t) , lloffset + del_offset * MIN_BLOCK_SIZE )) != sizeof(photos_info_t)) {
        close(fd);
        ERROR_RETURN(("write file %s offset %lu error", ch_file_path, lloffset), -1);
    }
    phead->photoscount--;
    if(phead->photoscount <= 0) {
        free_block_t stfree;
        memcpy(stfree.ch_flag, FREE_FALG, FREE_FALG_LEN);
        stfree.nextoffset = pmaps_file_info[dirid][ifileno].first_free_no;
        if((ret = pwrite64(fd , &stfree , sizeof(free_block_t) , lloffset)) != sizeof(free_block_t)) {
        	ERROR_LOG("write file %s offset %lu error", ch_file_path, lloffset);
        }
        else {
            pmaps_file_info[dirid][ifileno].first_free_no = ioffset;
            pmaps_offset[dirid][albumid/ALBUM_DIR_SIZE] = 0;
			DEBUG_LOG("FFN-D1\t[albumid=%u dir=%u fileno=%u f_br=%u s_nbr=%u]", albumid, dirid, ifileno, ioffset, stfree.nextoffset);
        }
        close(fd);
        return 0;
    }
    
    pfreelist = (unsigned int *)(ch_photos_head + sizeof(photos_head_t));
    for(i = 0; i < 254; i++) {
        if(photo_info.photosid == pfreelist[i] ){
            close(fd);
            return 0;
        }
    }
    
    if(del_offset * MIN_BLOCK_SIZE + del_size == phead->size) { // the freed block is the last one in album
        phead->size -= del_size;
        for(i = 0; i < 254; i++){
            if(phead->size <= MIN_BLOCK_SIZE) break;  // only the block head info buffer
            del_size = pfreelist[i] >> OFFSET_BIT;
            del_offset = pfreelist[i] & OFFSET_MASK;
            if(del_offset * MIN_BLOCK_SIZE + del_size == phead->size ) {
                phead->size -= del_size;
                pfreelist[i] = 0;
                i = 0;
            }
        }
        if(phead->size <= MIN_BLOCK_SIZE) {
            free_block_t stfree;
            memcpy(stfree.ch_flag, FREE_FALG, FREE_FALG_LEN);
            stfree.nextoffset = pmaps_file_info[dirid][ifileno].first_free_no;
            if((ret = pwrite64(fd , &stfree , sizeof(free_block_t) , lloffset)) != sizeof(free_block_t))
            {   
            }
            else{
                pmaps_file_info[dirid][ifileno].first_free_no = ioffset;
                pmaps_offset[dirid][albumid/ALBUM_DIR_SIZE] = 0;
				DEBUG_LOG("FFN-D2\t[albumid=%u dir=%u fileno=%u f_br=%u s_nbr=%u]", albumid, dirid, ifileno, ioffset, stfree.nextoffset);
            }
        }
        else{
			// update the free list and head size
            if((ret = pwrite64(fd , ch_photos_head , PHOTOS_HEAD_SIZE , lloffset)) != PHOTOS_HEAD_SIZE)
            {
                
            }
        }
    }
    else{  // not the last block in album
        for(i = 0; i < 254; i++){
            if(pfreelist[i] == 0 ){
                pfreelist[i] = photo_info.photosid;
                break;
            }
        }
        if((ret = pwrite64(fd , ch_photos_head , PHOTOS_HEAD_SIZE , lloffset)) != PHOTOS_HEAD_SIZE)
        {
            
        }
    }
    close(fd);
    return 0;
}

int del_album(uint32_t albumid)
{
    int ret = 0;
    long long lloffset;
    uint32_t dirid = albumid % ALBUM_DIR_SIZE;
    if(pmaps_offset[dirid] == NULL || pmaps_file_info[dirid] == NULL)
		ERROR_RETURN(("album not exist3\t[%u]", albumid), -1);
    uint32_t ioffset = pmaps_offset[dirid][albumid/ALBUM_DIR_SIZE] >> 8;
    uint32_t ifileno = pmaps_offset[dirid][albumid/ALBUM_DIR_SIZE] & 0xFF;
    uint32_t photos_block_len = MIN_PHOTOS_BLOCK_SIZE * (1 << ifileno);
    if(ioffset == 0 && ifileno == 0) ERROR_RETURN(("album not exist4\t[%u]", albumid), -1);
	
    char ch_file_path[MAX_FIEL_PATH];
    snprintf(ch_file_path, sizeof(ch_file_path), "%s/%03d/%s%d", PHOTOS_ROOT_PATH, dirid, PHOTOS_FIEL_PRE, ifileno);
    
    int fd = open64(ch_file_path, O_RDWR|O_CREAT,0666);
    if(fd < 0) ERROR_RETURN(("open file %s error", ch_file_path), -1);
    lloffset = ioffset;
    lloffset *= photos_block_len;
    free_block_t stfree;
    memcpy(stfree.ch_flag, FREE_FALG, FREE_FALG_LEN);
    stfree.nextoffset = pmaps_file_info[dirid][ifileno].first_free_no;
    if((ret = pwrite64(fd , &stfree , sizeof(free_block_t) , lloffset)) != sizeof(free_block_t)) {
		close(fd);
    	ERROR_RETURN(("write file %s offset %lu error", ch_file_path, lloffset), -1);
    }
    else {
        pmaps_file_info[dirid][ifileno].first_free_no = ioffset;
        pmaps_offset[dirid][albumid/ALBUM_DIR_SIZE] = 0;
		DEBUG_LOG("FFN-D3\t[albumid=%u dir=%u fileno=%u f_br=%u s_nbr=%u]", albumid, dirid, ifileno, ioffset, stfree.nextoffset);
    }
    close(fd);
    return 0;
}

int read_photo(unsigned int albumid,unsigned int photosid,char *pphotos_buf)
{
    int ret = 0;
    long long lloffset;
    uint32_t dirid = albumid % ALBUM_DIR_SIZE;
	if (photosid == 0) return 0;
    if(pmaps_offset[dirid] == NULL || pmaps_file_info[dirid] == NULL) return 0;
    uint32_t ioffset = pmaps_offset[dirid][albumid/ALBUM_DIR_SIZE] >> 8;
    uint32_t ifileno = pmaps_offset[dirid][albumid/ALBUM_DIR_SIZE] & 0xFF;
    uint32_t photos_block_len = MIN_PHOTOS_BLOCK_SIZE * (1 << ifileno);

	//DEBUG_LOG("FILE POS\t[offset=%u fileno=%u]", ioffset, ifileno);
    if(ioffset == 0 && ifileno == 0) ERROR_RETURN(("no file indx\t[key=%u]", albumid), 0);
    char ch_file_path[MAX_FIEL_PATH];
    char ch_photos_buf[MAX_PHOTOS_SIZE];
    uint32_t read_size = photosid >> OFFSET_BIT ;
    uint32_t read_offset = photosid & OFFSET_MASK;
    if(read_size > MAX_PHOTOS_SIZE) return 0;
    photos_info_t *photo_info = (photos_info_t*)ch_photos_buf;
    snprintf(ch_file_path, sizeof(ch_file_path), "%s/%03u/%s%d", PHOTOS_ROOT_PATH, dirid, PHOTOS_FIEL_PRE, ifileno);
    
    int fd = open64(ch_file_path, O_RDWR|O_CREAT, 0666);
    if(fd < 0) ERROR_RETURN(("cannot open file\t[%s %s]", ch_file_path, strerror(errno)), 0);
    lloffset = ioffset;
    lloffset *= photos_block_len;
	DEBUG_LOG("READ FILE\t[%s %llu %u %u %u]", ch_file_path, lloffset + read_offset * MIN_BLOCK_SIZE, sizeof(photos_info_t) + read_size, read_offset, read_size);
    if((ret = pread64(fd , ch_photos_buf , sizeof(photos_info_t) + read_size , lloffset + read_offset * MIN_BLOCK_SIZE)) != sizeof(photo_info) + read_size) {
        read_size = 0;
		ERROR_LOG("read photo error\t[%s %s]", ch_file_path, strerror(errno));
    }
    else {
        if(photo_info->photosid == photosid && photo_info->stime > 0) {
            memcpy(pphotos_buf, ch_photos_buf + sizeof(photos_info_t), read_size);
        }
        else {
            read_size = 0;
			ERROR_LOG("photo info error\t[%u %u %u %u]",albumid, photosid, photo_info->photosid, photo_info->stime);
        }
    }
    close(fd);
    return read_size;
}
#if 0
static char album_photos_buff[MAX_PHOTOS_BLOCK_SIZE];
static char send_thumbs_buff[MAX_PHOTOS_BLOCK_SIZE];

void write_pic(uint32_t pic_id, char* data, int len)
{
	char filename[64];
	snprintf(filename, 64, "/opt/taomee/file_root/pic/%u", pic_id);
	int fd = open(filename, O_RDWR | O_CREAT, 0666);
	if (fd < 0) {
		ERROR_LOG("OPEN FILE %s error, %s", filename, strerror(errno));
	}
	write(fd, data, len);
}

int pkg_thumbs_from_block(char* p_album_block, char* p_send_buff, int* pkg_len)
{
	photos_head_t* phead = (photos_head_t*)p_album_block;
	if (phead->photoscount > MAX_ALBUM_THUM_CNT) {
		ERROR_RETURN(("too much photos\t[%u]", phead->photoscount), -1);
	}
	int j = *pkg_len;
	PKG_H_UINT32(p_send_buff, phead->photoscount, j);
	DEBUG_LOG("THUMB CNT=%u ALBUM SIZE=%u", phead->photoscount, phead->size);

	uint32_t pic_len = 0;
	photos_info_t* ppi = (photos_info_t*)(p_album_block + PHOTOS_HEAD_SIZE);
	int lp;
	for (lp = 0; lp < phead->photoscount; lp++) {
		pic_len = ppi->photosid >> 16;
		if (pic_len > MAX_PHOTOS_SIZE) {
			ERROR_RETURN(("too big thumb size\t[%u]", pic_len), -1);
		}
		DEBUG_LOG("THMUBID=%u: len=%u", ppi->photosid, pic_len);
		PKG_H_UINT32(p_send_buff, ppi->photosid, j);
		PKG_H_UINT32(p_send_buff, pic_len, j);
		PKG_STR(p_send_buff, (((char*)ppi) + 8), j, pic_len);
		write_pic(ppi->photosid, (((char*)ppi) + 8), pic_len);
		uint32_t need_size = ((pic_len + sizeof(photos_info_t)) / MIN_BLOCK_SIZE + 1) * MIN_BLOCK_SIZE;
		ppi = (photos_info_t*)(((char*)ppi) + need_size);
	}
	*pkg_len = j;
	return 0;
}

int read_album_thumbs_block(unsigned int albumid, char *p_album_block)
{
    int ret = 0;
    long long lloffset;
    uint32_t dirid = albumid % ALBUM_DIR_SIZE;
    if(pmaps_offset[dirid] == NULL || pmaps_file_info[dirid] == NULL) return 0;
    uint32_t ioffset = pmaps_offset[dirid][albumid/ALBUM_DIR_SIZE] >> 8;
    uint32_t ifileno = pmaps_offset[dirid][albumid/ALBUM_DIR_SIZE] & 0xFF;
    uint32_t photos_block_len = MIN_PHOTOS_BLOCK_SIZE * (1 << ifileno);
    if(ioffset == 0 && ifileno == 0) 
		ERROR_RETURN(("cannot find album %d", albumid), -1);
	
    char ch_file_path[MAX_FIEL_PATH];
	char ch_photos_head[PHOTOS_HEAD_SIZE];
	photos_head_t *phead = (photos_head_t*)ch_photos_head;
    snprintf(ch_file_path, sizeof(ch_file_path), "%s/%03d/%s%d", PHOTOS_ROOT_PATH, dirid, PHOTOS_FIEL_PRE, ifileno);
    
    int fd = open64(ch_file_path, O_RDWR|O_CREAT, 0666);
    if(fd < 0) 
		ERROR_RETURN(("open file %s error: %s", ch_file_path, strerror(errno)), -1);
    lloffset = ioffset;
    lloffset *= photos_block_len;
	if((ret = pread64(fd , ch_photos_head , sizeof(photos_head_t), lloffset)) != sizeof(photos_head_t)) {
        ERROR_RETURN(("read %s offset %u error, %s", ch_file_path, lloffset, strerror(errno)), -1);
    }
	DEBUG_LOG("READ ALBUM\t[%u %s %llu %u]", albumid, ch_file_path, lloffset, phead->size);
    if((ret = pread64(fd , p_album_block , phead->size, lloffset)) != phead->size) {
         ERROR_RETURN(("read %s offset %u error, %s", ch_file_path, lloffset, strerror(errno)), -1);
    }
    close(fd);
    return 0;
}

int ReadPhotosForQhttp(unsigned int albumid,unsigned int photosid,char *pphotos_buf)
{
    int ret = 0;
    long long lloffset;
    uint32_t dirid = albumid % ALBUM_DIR_SIZE;
    //if(pMapsOffset[iDirId]==NULL||pMapsFileInfo[iDirId]==NULL)return 0;
    uint32_t ioffset = pmaps_offset[dirid][albumid/ALBUM_DIR_SIZE] >> 8;
    uint32_t ifileno = pmaps_offset[dirid][albumid/ALBUM_DIR_SIZE] & 0xFF;
    uint32_t photos_block_len = MIN_PHOTOS_BLOCK_SIZE * (1 << ifileno);
    //if(iOffset==0&&iFileNo==0)return 0;
    char ch_file_path[MAX_FIEL_PATH];
    
    uint32_t read_size = photosid >> 16 ;
    uint32_t read_offset = photosid & 0xFFFF;
    //if(iReadSize > MAXPHOTOSSIZE)return 0;
    photos_info_t *photo_info = (photos_info_t *)pphotos_buf;
    //snprintf(chFilePath,sizeof(chFilePath),"/%d/%s%d",iDirId,PHOTOS_FIEL_PRE,iFileNo);
    int fd;
    if(pmaps_offset[dirid] == NULL || pmaps_file_info[dirid] == NULL) return 0;
    if(ioffset == 0 && ifileno == 0) return 0;
    if(read_size > MAX_PHOTOS_SIZE) return 0;
    snprintf(ch_file_path, sizeof(ch_file_path), "/%d/%s%d", dirid, PHOTOS_FIEL_PRE, ifileno);
    fd = open64(ch_file_path, O_RDWR|O_CREAT, 0666);
    if(fd < 0) return 0;
    lloffset = ioffset;
    lloffset *= photos_block_len;
    if((ret = pread64(fd , pphotos_buf , sizeof(photos_info_t) + read_size , lloffset + read_offset * MIN_BLOCK_SIZE)) != sizeof(photos_info_t) + read_size) {
        read_size = 0;
    }
    else{
    //fprintf(g_logFp,"ReadPhotos pPhotoInfo->photosid %d  iPhotosId %d!\n",pPhotoInfo->photosid,iPhotosId);fflush(g_logFp);
        if(photo_info->photosid == photosid && photo_info->stime > 0)
        {
        }
        else
        {
            read_size = 0;
        }
    }
    close(fd);
    return read_size;
}
#endif

int upload_thumb_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen)
{
	up_thumb_t*   plal = (up_thumb_t*)g_slice_alloc0(sizeof(up_thumb_t));
	if (!plal) {
		ERROR_RETURN(("%s no memory", __FUNCTION__), -1);
	}
	CHECK_BODY_LEN_GE(pkglen, 12);
	int j = 0;
	UNPKG_H_UINT32(pkg, plal->u_key, j);
	UNPKG_H_UINT32(pkg, plal->key_albumid, j);
	UNPKG_H_UINT32(pkg, plal->thumb_cnt, j);

	char buff[256];
	int k = sizeof(protocol_t);
	PKG_H_UINT32(buff, plal->u_key, k);
	PKG_H_UINT32(buff, plal->thumb_cnt, k);
	DEBUG_LOG("UPLOAD THUMB\t[uid=%u u_key=%u b_key=%u %u]", 
		userid, plal->u_key, plal->key_albumid, plal->thumb_cnt);
	int lp;
	for (lp = 0; lp < plal->thumb_cnt; lp++) {
		thumb_t* pt = &plal->stthumb[lp];
		CHECK_BODY_LEN_GE(pkglen, j + 4);
		UNPKG_H_UINT32(pkg, pt->thumb_len, j);
		CHECK_BODY_LEN_GE(pkglen, j + pt->thumb_len);
		UNPKG_STR(pkg, pt->thumb_data, j, pt->thumb_len);
		pt->thumb_id = save_photo(plal->key_albumid, pt->thumb_len, pt->thumb_data);
		if (!pt->thumb_id) {
			ERROR_LOG("upload-thumb err\t[b_key=%u th_len=%u]", 
				plal->key_albumid, pt->thumb_len);
		}
		DEBUG_LOG("THUMB%u \t[thumbid=%u len=%u]",lp, pt->thumb_id, pt->thumb_len);
		PKG_H_UINT32(buff, plal->stthumb[lp].thumb_id, k);
	}
	init_proto_head(buff, userid, cmd, k);
	net_send(sockfd, buff, k);
	g_slice_free1(sizeof(up_thumb_t), plal);
	return 0;
}

int get_thumb_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen)
{
	uint32_t  key_albumid, pic_id, pic_len;
	CHECK_BODY_LEN(pkglen, 8);
	int j = 0;
	UNPKG_H_UINT32(pkg, key_albumid, j);
	UNPKG_H_UINT32(pkg, pic_id, j);
	//DEBUG_LOG("GET ALBUM\t[%u %u %u]",userid, key_albumid, pic_id);

	char  pic_buff[MAX_THUMB_SIZE + 22];
	int k = sizeof(protocol_t);
	if ((pic_len = read_photo(key_albumid, pic_id, pic_buff + 22)) == 0){
		ERROR_LOG("cannot read thumb\t[uid=%u %u %u]",userid, key_albumid, pic_id);
	}

	*(uint32_t*)(pic_buff + k) = pic_len;
	k += 4;
	//DEBUG_LOG("THUMB \t[thumbid=%u len=%u]", pic_id, pic_len);
	k += pic_len;
	init_proto_head(pic_buff, userid, cmd, k);
	return net_send(sockfd, pic_buff, k);
}
#if 0
int get_album_thumbs_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen)
{
	int  key_albumid;
	int j = 0;
	UNPKG_H_UINT32(pkg, key_albumid, j);
	DEBUG_LOG("GET ALBUM\t[%u %u]", userid, key_albumid);
	int k = sizeof(protocol_t);
	if (read_album_thumbs_block(key_albumid, album_photos_buff) == -1) {
		ERROR_LOG("read album[%u] error", key_albumid);
		init_proto_head(send_thumbs_buff, userid, cmd, k);
		PKG_H_UINT32(send_thumbs_buff, 0, k);
		return net_send(sockfd, send_thumbs_buff, k);
	}
	pkg_thumbs_from_block(album_photos_buff, send_thumbs_buff, &k);
	init_proto_head(send_thumbs_buff, userid, cmd, k);
	DEBUG_LOG("RET CGI\t[%u %u %u]", userid, cmd, k);
	return net_send(sockfd, send_thumbs_buff, k);
}
#endif
int del_thumbs_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen)
{
	CHECK_BODY_LEN_GE(pkglen, 12);
	int usr_fd = 0;
	uint32_t key_album;
	del_multi_thumbs_id_t ldmt;
	int j = 0;
	UNPKG_H_UINT32(pkg, usr_fd, j);
	UNPKG_H_UINT32(pkg, key_album, j);
	UNPKG_H_UINT32(pkg, ldmt.file_cnt, j);

	DEBUG_LOG("DEL MULTI THUMBS\t[id=%u key=%u file_cnt=%u]", userid, key_album, ldmt.file_cnt);
	int loop;
	for(loop = 0; loop < ldmt.file_cnt; loop++) {
		thumb_ids_t* lti = &ldmt.sti[loop];
		CHECK_BODY_LEN_GE(pkglen, j + 4);
		UNPKG_H_UINT32(pkg, lti->thumb_cnt, j);
		int lp;
		for (lp = 0; lp < lti->thumb_cnt; lp++) {
			UNPKG_H_UINT32(pkg, lti->thumb_id[lp], j);
			if (del_photo(key_album, lti->thumb_id[lp]) == -1) {
				ERROR_LOG("del photo error\t[%u %u]", key_album, lti->thumb_id[lp]);
			} else {
				DEBUG_LOG("DEL PHOTO OK[%u %u]", key_album, lti->thumb_id[lp]);
			}
		}
	}
	char buff[256];
	int k = sizeof(protocol_t);
	PKG_H_UINT32(buff, usr_fd, k);
	PKG_H_UINT32(buff, key_album, k);
	PKG_H_UINT32(buff, ldmt.file_cnt, k);
	init_proto_head(buff, userid, cmd, k);
	DEBUG_LOG("DEL MULTI THUMBS\t[%u fd=%u key=%u file_cnt=%u k=%u]",sockfd, usr_fd, key_album, ldmt.file_cnt, k);
	return net_send(sockfd, buff, k);
}

int del_album_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen)
{
	CHECK_BODY_LEN_GE(pkglen, 12);
	int usr_fd = 0;
	uint32_t key_album, cmd_from;
	int j = 0;
	UNPKG_H_UINT32(pkg, usr_fd, j);
	UNPKG_H_UINT32(pkg, key_album, j);
	UNPKG_H_UINT32(pkg, cmd_from, j);

	DEBUG_LOG("DEL ALBUM\t[id=%u b_key=%u cmd_from=%u]", userid, key_album, cmd_from);
	if (del_album(key_album) == -1) {
		ERROR_LOG("del album error\t[uid=%u %u]",userid, key_album);
	} else {
		DEBUG_LOG("DEL ALBUM OK[uid=%u %u]",userid, key_album);
	}
	if (cmd_from) {  // 1,  from  adminserv; 0, from user script
		char buff[256];
		int k = sizeof(protocol_t);
		PKG_H_UINT32(buff, usr_fd, k);
		PKG_H_UINT32(buff, key_album, k);
		init_proto_head(buff, userid, cmd, k);
		return net_send(sockfd, buff, k);
	}
	return 0;
}

int change_thumb_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen)
{
	up_thumb_t*   plal = (up_thumb_t*)g_slice_alloc0(sizeof(up_thumb_t));
	if (!plal) {
		ERROR_RETURN(("%s no memory", __FUNCTION__), -1);
	}
	CHECK_BODY_LEN_GE(pkglen, 12);
	int j = 0, del_cnt = 0;
	uint32_t thumbids[MAX_THUMB_CNT_PER_PIC];
	UNPKG_H_UINT32(pkg, plal->u_key, j);
	UNPKG_H_UINT32(pkg, plal->key_albumid, j);
	UNPKG_H_UINT32(pkg, del_cnt, j);
	CHECK_BODY_LEN_GE(pkglen, 12 + del_cnt * 4 + 4);
	int loop;
	for (loop = 0; loop < del_cnt; loop++) {
		UNPKG_H_UINT32(pkg, thumbids[loop], j);
		if (del_photo(plal->key_albumid, thumbids[loop]) == -1) {
			ERROR_LOG("chg-del photo error\t[%u %u]", plal->key_albumid, thumbids[loop]);
		} else {
			DEBUG_LOG("CHG-DEL PHOTO OK[%u %u]", plal->key_albumid, thumbids[loop]);
		}
	}
	UNPKG_H_UINT32(pkg, plal->thumb_cnt, j);

	char buff[256];
	int k = sizeof(protocol_t);
	PKG_H_UINT32(buff, plal->u_key, k);
	PKG_H_UINT32(buff, plal->thumb_cnt, k);
	DEBUG_LOG("CHANGE THUMB\t[%u %u %u %u]", userid, plal->u_key, plal->key_albumid, plal->thumb_cnt);

	int lp;
	for (lp = 0; lp < plal->thumb_cnt; lp++) {
		thumb_t* pt = &plal->stthumb[lp];
		CHECK_BODY_LEN_GE(pkglen, j + 4);
		UNPKG_H_UINT32(pkg, pt->thumb_len, j);
		CHECK_BODY_LEN_GE(pkglen, j + pt->thumb_len);
		UNPKG_STR(pkg, pt->thumb_data, j, pt->thumb_len);
		pt->thumb_id = save_photo(plal->key_albumid, pt->thumb_len, pt->thumb_data);
		if (!pt->thumb_id) {
			ERROR_LOG("chg-thumb err\t[b_key=%u th_len=%u]", plal->key_albumid, pt->thumb_len);
		}
		DEBUG_LOG("CHG-ADD-THUMB%u \t[thumbid=%u len=%u]",lp, pt->thumb_id, pt->thumb_len);
		PKG_H_UINT32(buff, pt->thumb_id, k);
	}
	init_proto_head(buff, userid, cmd, k);
	net_send(sockfd, buff, k);
	g_slice_free1(sizeof(up_thumb_t), plal);
	return 0;
}

#if 0
static uint8_t thumbs_buff[MAX_THUMB_SIZE * 4];
int transfer_thumbs_del_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen)
{
	CHECK_BODY_LEN_GE(pkglen, 12);
	int usr_fd = 0;
	uint32_t key_album;
	thumb_ids_t ldt;
	int j = 0;
	UNPKG_H_UINT32(pkg, usr_fd, j);
	UNPKG_H_UINT32(pkg, key_album, j);
	UNPKG_H_UINT32(pkg, ldt.thumb_cnt, j);
	if (ldt.thumb_cnt > MAX_THUMB_CNT_PER_PIC) {
		ERROR_LOG("invalid thumb cnt\t[%u %u]", key_album, ldt.thumb_cnt);
		return 0;
	}
	CHECK_BODY_LEN_GE(pkglen, ldt.thumb_cnt * 4 + 12);
	
	thumb_t* plt = (thumb_t*)g_slice_alloc0(sizeof(thumb_t) * ldt.thumb_cnt);
	if (!plt) {
		ERROR_LOG("no memory\t[%u %u]", key_album, ldt.thumb_cnt);
		return 0;
	}
	DEBUG_LOG("TRANSFER THUMBS\t[%u %u %u]", userid, key_album, ldt.thumb_cnt);
	int lp;
	for (lp = 0; lp < ldt.thumb_cnt; lp++) {
		UNPKG_H_UINT32(pkg, ldt.thumb_id[lp], j);
		if ((plt[lp].thumb_len = read_photo(key_album, ldt.thumb_id[lp], plt[lp].thumb_data)) == 0){
			ERROR_LOG("trans cannot read thumb\t[%u %u]", key_album, ldt.thumb_id[lp]);
			continue;
		}
		if (del_photo(key_album, ldt.thumb_id[lp]) == -1) {
			ERROR_LOG("trans_del photo error\t[%u %u]", key_album, ldt.thumb_id[lp]);
		} else {
			DEBUG_LOG("TRANS_DEL PHOTO OK[%u %u]", key_album, ldt.thumb_id[lp]);
		}
	}

	int k = sizeof(protocol_t);
	PKG_H_UINT32(thumbs_buff, usr_fd, k);
	PKG_H_UINT32(thumbs_buff, key_album, k);
	int label_k = k;
	PKG_H_UINT32(thumbs_buff, ldt.thumb_cnt, k);
	int counter = 0;
	for (lp = 0; lp < ldt.thumb_cnt; lp++) {
		if (plt[lp].thumb_len) {
			PKG_H_UINT32(thumbs_buff, plt[lp].thumb_len, k);
			PKG_STR(thumbs_buff, plt[lp].thumb_data, k, plt[lp].thumb_len);
			counter++;
		}
	}
	PKG_H_UINT32(thumbs_buff, counter, label_k);
	init_proto_head(thumbs_buff, userid, cmd, k);
	net_send(sockfd, thumbs_buff, k);
	g_slice_free1(sizeof(thumb_t) * ldt.thumb_cnt, plt);
	return 0;
}
#endif
static char get_multi_thumbs_buff[MAX_THUMB_SIZE * MAX_THUMB_CNT_PER_PIC * MAX_TRANS_NUM];
int trans_multi_get_thumbs_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen)
{
	CHECK_BODY_LEN_GE(pkglen, 12);
	int usr_fd = 0;
	uint32_t key_album;
	trans_multi_thumbs_id_t ltmt;
	int j = 0;
	UNPKG_H_UINT32(pkg, usr_fd, j);
	UNPKG_H_UINT32(pkg, key_album, j);
	UNPKG_H_UINT32(pkg, ltmt.file_cnt, j);

	DEBUG_LOG("GET MULTI THUMBS\t[id=%u key=%u file_cnt=%u]", userid, key_album, ltmt.file_cnt);
	int buf_itr = sizeof(protocol_t);
	PKG_H_UINT32(get_multi_thumbs_buff, usr_fd, buf_itr);
	PKG_H_UINT32(get_multi_thumbs_buff, key_album, buf_itr);
	PKG_H_UINT32(get_multi_thumbs_buff, ltmt.file_cnt, buf_itr);
	int lp;
	for (lp = 0; lp < ltmt.file_cnt; lp++) {
		thumb_ids_t* pti = &(ltmt.sti[lp]);
		CHECK_BODY_LEN_GE(pkglen, j + 4);
		UNPKG_H_UINT32(pkg, pti->thumb_cnt, j);
		PKG_H_UINT32(get_multi_thumbs_buff, pti->thumb_cnt, buf_itr);
		int lp0, coutr = 0;
		for (lp0 = 0; lp0 < pti->thumb_cnt; lp0++) {
			CHECK_BODY_LEN_GE(pkglen, j + 4);
			UNPKG_H_UINT32(pkg, pti->thumb_id[lp0], j);
			PKG_H_UINT32(get_multi_thumbs_buff, pti->thumb_id[lp0], buf_itr);
			int pic_len = 0, label_pic_len_idx = buf_itr; buf_itr += 4;
			if ((pic_len = read_photo(key_album, pti->thumb_id[lp0], get_multi_thumbs_buff + buf_itr)) == 0){
				ERROR_LOG("cannot read thumb\t[%u %u]", key_album, pti->thumb_id[lp0]);
			} else {
				DEBUG_LOG("READ PHOTO OK[%u %u]", key_album, pti->thumb_id[lp0]);
				buf_itr += pic_len;
				coutr++;
			}
			PKG_H_UINT32(get_multi_thumbs_buff, pic_len, label_pic_len_idx);
		}
	}

	init_proto_head(get_multi_thumbs_buff, userid, cmd, buf_itr);
	return net_send(sockfd, get_multi_thumbs_buff, buf_itr);
}


//static char add_multi_thumbs_buff[MAX_THUMB_SIZE * MAX_THUMB_CNT_PER_PIC * MAX_TRANS_NUM];
int trans_multi_add_thumbs_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen)
{
	CHECK_BODY_LEN_GE(pkglen, 12);
	int usr_fd = 0;
	uint32_t key_album;
	trans_multi_thumbs_id_t ltmt;
	int j = 0;
	UNPKG_H_UINT32(pkg, usr_fd, j);
	UNPKG_H_UINT32(pkg, key_album, j);
	UNPKG_H_UINT32(pkg, ltmt.file_cnt, j);

	DEBUG_LOG("ADD MULTI THUMBS\t[id=%u key=%u file_cnt=%u]", userid, key_album, ltmt.file_cnt);
	uint8_t  add_multi_buff[1024];
	int buf_itr = sizeof(protocol_t);
	PKG_H_UINT32(add_multi_buff, usr_fd, buf_itr);
	PKG_H_UINT32(add_multi_buff, key_album, buf_itr);
	PKG_H_UINT32(add_multi_buff, ltmt.file_cnt, buf_itr);
	int lp, album_full = 0;
	for (lp = 0; lp < ltmt.file_cnt; lp++) {
		thumb_ids_t* pti = &(ltmt.sti[lp]);
		CHECK_BODY_LEN_GE(pkglen, j + 4);
		UNPKG_H_UINT32(pkg, pti->thumb_cnt, j);
		
		DEBUG_LOG("TRAN ADD\t[%u %u fcnt=%u tcnt=%u]", usr_fd, key_album, ltmt.file_cnt, pti->thumb_cnt);
		PKG_H_UINT32(add_multi_buff, pti->thumb_cnt, buf_itr);
		int lp0;
		for (lp0 = 0; lp0 < pti->thumb_cnt; lp0++) {
			CHECK_BODY_LEN_GE(pkglen, j + 8);
			UNPKG_H_UINT32(pkg, pti->thumb_id[lp0], j);
			int thumb_len = 0;
			UNPKG_H_UINT32(pkg, thumb_len, j);
			DEBUG_LOG("TRAN ADD\t[thumbid=%u tlen=%u]", pti->thumb_id[lp0], thumb_len);
			CHECK_BODY_LEN_GE(pkglen, j + thumb_len);
			int thumbid = 0;
			if (!album_full) {
				if ((thumbid = save_photo(key_album, thumb_len, pkg + j)) == 0){
					ERROR_LOG("cannot save thumb\t[%u %u]", key_album, pti->thumb_id[lp0]);
					album_full = 1;
				} else {
					DEBUG_LOG("SAVE PHOTO OK[key=%u oid=%u nid=%u]", key_album, pti->thumb_id[lp0], thumbid);
				}
			}
			j += thumb_len;
			PKG_H_UINT32(add_multi_buff, thumbid, buf_itr);
		}
	}

	init_proto_head(add_multi_buff, userid, cmd, buf_itr);
	return net_send(sockfd, add_multi_buff, buf_itr);
}

int trans_multi_del_thumbs_cmd(uint32_t userid, uint16_t cmd, int sockfd, void* pkg, int pkglen)
{
	CHECK_BODY_LEN_GE(pkglen, 12);
	int usr_fd = 0;
	uint32_t key_album;
	trans_multi_thumbs_id_t ltmt;
	int j = 0;
	UNPKG_H_UINT32(pkg, usr_fd, j);
	UNPKG_H_UINT32(pkg, key_album, j);
	UNPKG_H_UINT32(pkg, ltmt.file_cnt, j);

	DEBUG_LOG("TRANS MULTI DEL THUMBS\t[id=%u key=%u file_cnt=%u]", userid, key_album, ltmt.file_cnt);
	int lp;
	for (lp = 0; lp < ltmt.file_cnt; lp++) {
		thumb_ids_t* pti = &(ltmt.sti[lp]);
		CHECK_BODY_LEN_GE(pkglen, j + 4);
		UNPKG_H_UINT32(pkg, pti->thumb_cnt, j);
		int lp0;
		for (lp0 = 0; lp0 < pti->thumb_cnt; lp0++) {
			CHECK_BODY_LEN_GE(pkglen, j + 4);
			UNPKG_H_UINT32(pkg, pti->thumb_id[lp0], j);
			if ((del_photo(key_album, pti->thumb_id[lp0])) != 0){
				ERROR_LOG("trans multi cannot del thumb\t[%u %u]", key_album, pti->thumb_id[lp0]);
			} else {
				DEBUG_LOG("TRANS MULITI DEL PHOTO OK[%u %u]", key_album, pti->thumb_id[lp0]);
			}
		}
	}
	return 0;
}

