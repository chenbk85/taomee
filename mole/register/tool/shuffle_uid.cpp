/**
 * =====================================================================================
 *
 * @file  shuffle_uid.cpp
 *
 * @brief 打乱二进制米米号文件中米米号的排列。
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
	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " id_file" << endl;
		return -1;
	}

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

	srand(time(0));
	random_shuffle(ids, &(ids[st.st_size / 4]));
}
