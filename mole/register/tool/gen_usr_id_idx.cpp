extern "C" {
#include <sys/stat.h>
}

#include <cstdio>

#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 3) {
		cerr << "Usage: " << argv[0] << " binfile binindexfile" << endl;
		return -1;
	}

	struct stat st;
	if (stat(argv[1], &st) == -1) {
		perror("stat");
		return -1;
	}

	ofstream fout(argv[2], ofstream::binary);
	if (!fout) {
		cerr << "Fail to Create File " << argv[2] << endl;
		return -1;
	}

	uint32_t tmp = 0;
	fout.write(reinterpret_cast<char*>(&tmp), 4);
	tmp = st.st_size / 4;
	fout.write(reinterpret_cast<char*>(&tmp), 4);
}

