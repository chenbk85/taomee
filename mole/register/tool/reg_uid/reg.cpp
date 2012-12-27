#include <ctime>

#include <iostream>
#include <string>

using namespace std;

#include <openssl/md5.h>

#include <libtaomee++/conf_parser/ini_parser.hpp>

using namespace taomee;

#include "Cclientproto.h"

int main(int argc, char* argv[])
{
	IniParser ini("./conf.ini");

	ini.parse();

	char ipaddr[16] = { 0 };
	string stmp = ini.read("dbproxy", "ip");
	memcpy(ipaddr, stmp.c_str(), stmp.size());

	unsigned short port = ini.read_int("dbproxy", "port");

	USER_REGISTER_IN user_db;
	memset(&user_db, 0x00, sizeof(USER_REGISTER_IN));

	MD5((const unsigned char*)"password", 8, (unsigned char*)&user_db.passwd);

	Cclientproto cp(ipaddr, port);  // DBProxy IP & Port
	cout << "Connected to " << ipaddr << endl << "Port: " << port << endl;

	uint32_t req_user_id; // id to reg
	cout << "Enter An User ID to Reg (Password Default to Be 'password'. 'q' to quit):" << endl;
	while (cin >> req_user_id) {
		if ((req_user_id < 20000000) || (req_user_id > 30000000)) {
			cerr << "Invalid User ID: " << req_user_id << endl;
			return -1;
		}
		// email address should be unique for each registeration
		cout << "Enter A Unique Email Address (160 Bytes At Most):" << endl;
		cin >> user_db.email;
		user_db.sex = 1;
		user_db.birthday = time(0);
		user_db.petcolor = 12550957;
		memcpy (user_db.nick, "xxx", 3);

		user_db.reg_addr_type = 0;
		user_db.ip = 0;
		
		uint32_t user_id_regged;         // actual regged id, should be the same as req_user_id
		int ret = cp.send_register_cmd(req_user_id, &user_db, &user_id_regged);
		cout << "RET: " << ret << '\t' << "Regged User ID: " << req_user_id << endl;
	}

    return 0;
}

