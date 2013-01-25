#include <iostream>
#include <vector>

using namespace std;

#include <async_serv/net_if.h>

// 此处乃声明，实际实现在data.cpp中
extern vector<int> ivec;
extern int test(int a);

extern "C" int init_service(int isparent)
{
    cout << "enter init_service" << endl;

	if (!isparent) {
		// 往ivec中插入10个元素，内存由堆里面分配
		for (int i = 0; i != 10; ++i) {
            test(i);
		}
	}
	
	return 0;
}

extern "C" int fini_service(int isparent)
{
    cout << "enter fini_service" << endl;

    return 0;
}

/*
 * 这个接口用在重读text.so之后，子进程对data.so进行一些必要的重新初始化。
 * 这里并不需要特别进行重新初始化，只是利用它来输出ivec中的元素。
 */
extern "C" int reload_global_data()
{
    cout << "enter reload_global" << endl;

	// 重读text.so后，ivec中的内容不变
	cout << "ivec size: " << ivec.size() << endl;
	for (vector<int>::iterator it = ivec.begin(); it != ivec.end(); ++it) {
		cout << *it << endl;
	}
	
	return 0;
}

extern "C" int get_pkg_len(int fd, const void* pkg, int pkglen, int isparent)
{
    cout << "enter get_pkg_len" << endl;

	return 0;
}

extern "C" int proc_pkg_from_client(void* data, int len, fdsession_t* fdsess)
{
	return 0;
}

extern "C" void proc_pkg_from_serv(int fd, void* data, int len)
{
}

extern "C" void on_client_conn_closed(int fd)
{
}

extern "C" void on_fd_closed(int fd)
{
}
