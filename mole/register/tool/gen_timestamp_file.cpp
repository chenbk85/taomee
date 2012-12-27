/**
 * =====================================================================================
 *
 * @file  gen_timestamp_file.cpp
 *
 * @brief 创建时间戳记录文件
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
}

#include <cstdio>
#include <ctime>

#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 3) {
		cerr << "Usage: " << argv[0] << " size outfile" << endl;
		return -1;
	}

	uint32_t size;
	int cnt = sscanf(argv[1], "%u", &size);
	if (cnt == 0) {
		cerr << "Invalid Size: " << argv[1] << endl;
		return -1;
	}

	int fd = open(argv[2], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd == -1) {
		perror("open");
		return -1;
	}

	if (lseek(fd, size * sizeof(time_t) - 1, SEEK_SET) == -1) {
		perror("lseek");
		return -1;
	}
	write(fd, "", 1);
}
