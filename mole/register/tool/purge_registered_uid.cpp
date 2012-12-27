/**
 * =====================================================================================
 *
 * @file  purge_registered_uid.cpp
 *
 * @brief 清除已注册米米号，并且更新米米号索引文件。
 *
 * compiler  GCC4.1.2
 * platform  Linux
 *
 * copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 * 		
 * ------------------------------------------------------------
 * 	note: set tabspace=4 
 *
 * =====================================================================================
 */

extern "C" {
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
}

#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 3) {
		cerr << "Usage: " << argv[0] << " uidfile idxfile" << endl;
		return -1;
	}

	// Open and Read the Uid Index File
	int idxfd = open(argv[2], O_RDWR);
	if (idxfd == -1) {
		perror("open");
		return -1;
	}

	uint32_t idx, cnt;
	read(idxfd, &idx, 4);
	read(idxfd, &cnt, 4);

	// Open, Map and Purge Uid File
	struct stat st;
	if (stat(argv[1], &st) == -1) {
		perror("stat");
		return -1;
	}

	int fd = open(argv[1], O_RDWR);
	if (fd == -1) {
		perror("open");
		return -1;
	}

	uint32_t* ids = reinterpret_cast<uint32_t*>(mmap(0, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
	if (ids == MAP_FAILED) {
		perror("map");
		return -1;
	}

	uint32_t last_idx = (st.st_size / 4) - 1;
	for (uint32_t i = 0; i != idx; ++i) {
		ids[i] = ids[last_idx - i];
	}

	if (ftruncate(fd, st.st_size - idx * 4) == -1) {
		perror("ftruncate");
		return -1;
	}

	if (lseek(idxfd, 0, SEEK_SET) == -1) {
		perror("lseek");
		return -1;
	}

	uint32_t tmp = 0;
	write(idxfd, &tmp, 4);
	tmp = (st.st_size- idx * 4) / 4;
	write(idxfd, &tmp, 4);
}
