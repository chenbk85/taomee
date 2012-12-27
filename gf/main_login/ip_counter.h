#ifndef IP_COUNTER_H
#define IP_COUNTER_H

int ip_counter_init();
void ip_counter_clear(uint32_t ip);
void ip_counter_add(uint32_t ip);
int ip_counter_info(uint32_t ip, ip_info_t& myinfo);
#endif
