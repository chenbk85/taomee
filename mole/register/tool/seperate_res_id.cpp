#include <stdint.h>

#include <cstdio>
#include <cstring>

#include <fstream>
#include <iostream>

using namespace std;

bool IsSmDigit(uint32_t id);
bool IsConDigit(uint32_t id);

int main(int argc, char* argv[])
{
	if (argc != 4) {
		cerr << "Usage: " << argv[0] << " infile regoutfile resoutfile" << endl;
		return -1;
	}

	ifstream fin(argv[1]);
	ofstream fout1(argv[2]);
	ofstream fout2(argv[3]);
	if (!fin || !fout1 || !fout2) {
		cerr << "File Error" << endl;
		return -1;
	}

	uint32_t id;
	while (fin >> id) {
		if (IsSmDigit(id) || IsConDigit(id)) {
			fout2 << id << '\n';
		} else {
			fout1 << id << '\n';
		}
	}

	fout1 << flush;
	fout2 << flush;
}

bool IsSmDigit(uint32_t id)
{
	const int keynum = 10;
	static const char* keys[keynum] = {
		"0000", "1111", "2222", "3333", "4444",
		"5555", "6666", "7777", "8888", "9999"
	};

	char digit[20];

	snprintf(digit, sizeof digit, "%u", id);
	for (int i = 0; i != keynum; ++i) {
		if (strstr(digit, keys[i]) != 0) {
			return true;
		}
	}
		
	return false;
}

bool IsConDigit(uint32_t id)
{
	const int keynum = 16;
	static const char* keys[keynum] = {
		"0123", "1234", "2345", "3456", "4567", "5678", "6789", "7890",
		"0987", "9876", "8765", "7654", "6543", "5432", "4321", "3210"
	};

	char digit[20];

	snprintf(digit, sizeof digit, "%u", id);
	for (int i = 0; i != keynum; ++i) {
		if (strstr(digit, keys[i]) != 0) {
			return true;
		}
	}

	return false;
}
