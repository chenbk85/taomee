
extern "C" {
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
}

#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 4) {
		cerr << "Usage: " << argv[0] << " uidfile idxfile" << endl;
		return -1;
	}
	char buffer[10];

	uint32_t num;

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

	int fdout1 = open(argv[2], O_RDWR | O_CREAT | O_TRUNC);
	int fdout2 = open(argv[3], O_RDWR | O_CREAT | O_TRUNC);

	uint32_t last_idx = (st.st_size / 4) - 1;
	for (uint32_t i = 0; i <= last_idx; ++i) {
		read(fd, &num, 4);
		if (num < 10000000) {
			sprintf(buffer, "%u", num);
			write(fdout1, buffer, strlen(buffer));
			write(fdout1, "\n", 1);
		} else {
			//write(fdout2, &num, 4);
			sprintf(buffer, "%u", num);
			write(fdout2, buffer, strlen(buffer));
			write(fdout2, "\n", 1);
		}
	}

}
