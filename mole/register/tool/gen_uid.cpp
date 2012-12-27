/**
 * =====================================================================================
 *
 * @file  gen_uid.cpp
 *
 * @brief 生成新的可注册米米号和保留号。
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

#include <stdint.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

bool IsSmDigit(uint32_t id);
bool IsConDigit(uint32_t id);

int main(int argc, char* argv[])
{
	if (argc != 5) {
		cerr << "Usage: " << argv[0] << " beg_id end_id uidfile res_idfile" << endl;
		return -1;
	}

	istringstream iss(string(argv[1]) + " " + string(argv[2]));
	uint32_t beg, end;
	iss >> beg >> end;
	if (!iss) {
		cerr << "Invalid ID Range Inputted!" << endl;
		return -1;
	}
	if (end < beg) {
		cerr << "Invalid ID Range: " << beg << '\t' << end << endl;
	}

	ofstream fout1(argv[3]);  // uid file
	ofstream fout2(argv[4]);  // res uid file
	if (!fout1 || !fout2) {
		cerr << "Fail to Create Files" << endl;
		return -1;
	}

	for (uint32_t i = beg; i <= end; ++i) {
		if (IsSmDigit(i) || IsConDigit(i)) {
			fout2 << i << '\n';
		} else {
			fout1 << i << '\n';
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
