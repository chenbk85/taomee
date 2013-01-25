#ifndef 	_SEND_DATA_H
#define		_SEND_DATA_H

extern pthread_spinlock_t lock_net_send;
extern "C" int send_data(uint8_t* buf, int len, int flag);

#endif
