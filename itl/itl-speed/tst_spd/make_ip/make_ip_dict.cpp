#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <mysql/mysql.h>
#include <list>
#include <map>
#include <fstream>

struct code_t
{
	uint32_t	ipspan;
	uint32_t	province_code;
	uint32_t	city_code;
	uint32_t	isp_id;
	uint8_t	isp[64];
	uint8_t	type;			//该点是某个ip段的起点还是终点0表示起点，1表示终点，2表示即是起点也是终点(此时ipspan=1)
	uint8_t	key[80];		//"city_code""isp"两个串作为hash的key
	uint32_t offset;		//标识该记录在mmap中应处于什么位置,具体的偏移＝offset*sizeof(type)
}__attribute__((packed));

struct ip_info_t
{
	uint32_t start_ip;
	uint32_t end_ip;
	uint32_t ipspan;
	uint32_t province_code;
	uint32_t city_code;
	uint8_t	 isp[64];
}__attribute__((packed));

typedef struct common_isp {
	uint32_t	isp_id;
	uint8_t	isp[64];
	common_isp *next;
} common_isp_t;

common_isp_t *common_isp_link = NULL;

char host[128] = "10.1.10.245";
char user[128] = "singku";
char pass[128] = "tony@55745535";
char dbnm[128] = "db_ip_distribution_11_Q4";

int get_ready()
{
	char *cr;
	char buf[128];
	printf("\e[33mInput The Host of IP Database:\n\e[m");
	fgets(buf, sizeof(buf), stdin);
	cr = strchr((const char *)buf, (int)'\n');
	*cr = 0;
	if (buf[0] != 0)
		memcpy(host, buf, sizeof(host));
	else
		printf("use default host %s\n", host);

	printf("\e[33mInput The Database Name:\n\e[m");
	fgets(buf, sizeof(buf), stdin);
	cr = strchr((const char *)buf, (int)'\n');
	*cr = 0;
	if (buf[0] != 0)
		memcpy(dbnm, buf, sizeof(dbnm));
	else
		printf("use default dbnm %s\n", dbnm);

	printf("\e[33mInput The Authorized User Name:\n\e[m");
	fgets(buf, sizeof(buf), stdin);
	cr = strchr((const char *)buf, (int)'\n');
	*cr = 0;
	if (buf[0] != 0)
		memcpy(user, buf, sizeof(user));
	else
		printf("use default user %s\n", user);

	printf("\e[33mInput The PassWord of this User:\n\e[m");
	printf("\e[?25l\e[8m");
	fgets(buf, sizeof(buf), stdin);
	cr = strchr((const char *)buf, (int)'\n');
	*cr = 0;
	if (buf[0] != 0)
		memcpy(pass, buf, sizeof(pass));
	else
		printf("use default passwd\n");
	printf("\e[0m\e[?25h");

	return 0;
}

void add_isp(int id, char *name)
{
	common_isp_t *aisp,*pnext,*ppos;
	aisp = pnext = ppos = NULL;
	pnext = common_isp_link;
	while (pnext != NULL) {
		if ((uint32_t)id == pnext->isp_id
			|| memcmp((const char*)name, (const char *)(pnext->isp), strlen(name)) == 0) {
			printf("same id or isp name\n");
			exit(-1);
		}
		pnext = pnext->next;
	}
	aisp = (common_isp_t*)malloc(sizeof(common_isp_t));
	if (aisp == NULL) {
		printf("add isp: malloc failed!\n");
		exit(-1);
	}
	aisp->isp_id = id;
	memcpy(aisp->isp, name, strlen(name) + 1);
	aisp->next = NULL;
	if (common_isp_link == NULL)
		common_isp_link = aisp;
	else {
		pnext = common_isp_link;
		ppos = pnext;
		while (pnext != NULL) {
			if (strlen(name) > strlen((const char*)(pnext->isp)))
				break;
			ppos = pnext;
			pnext = pnext->next;
		}
		if (pnext == common_isp_link) {//head
			aisp->next = pnext;
			common_isp_link = aisp;
		}
		else {
			aisp->next = pnext;
			ppos->next = aisp;
		}
	}

}

int read_ispno()
{
	FILE *fp;
	char c,c1;
	char s[1024];
	int len;
	char *eq;
	int id;
	char name[64];
	fp = fopen("isp.conf", "rt");
	if (fp == NULL) {
		printf("Can't open config file \"isp.conf\"\n");
		exit(-1);
	}

	while (!feof(fp)) {
		c = fgetc(fp);
		if ((int)c == -1)
			//EOF==-1
			break;
		if (c != '$') {
			while (c != '\n' && !feof(fp))
				c = fgetc(fp);
			continue;
		}
		if ((c1 = fgetc(fp)) != ':') {
			while (c != '\n' && !feof(fp))
				c = fgetc(fp);
			continue;
		}

		if ('$' == c && ':' == c1) {

			//memset(s,0,TEXT_LINE_LEN);
			fgets(s, 1024, fp);
			len = 0;
			if((eq = strchr(s, '\n')) == NULL)
				continue;
			len = eq-s;
			if ( len <= 0 )
				continue;

			if ((eq = strchr(s, (int)'=')) == NULL)
				continue;
			len = eq - s;
			if (len <= 0)
				continue;
			id = -1;
			sscanf(s, "%d=%s", &id, name);
			if (id < 0 || id > 1000) {
				printf("illegal isp:%s\n",s);
				exit(-1);
			}
			add_isp(id, name);
		}
	}//while
	return 0;
}

void print_isp()
{
	common_isp_t *p = common_isp_link;
	while(p!=NULL) {
		printf("==>%d,%s\n",p->isp_id,p->isp);
		p = p->next;
	}
}

void free_isp() {
	common_isp_t *p = common_isp_link;
	common_isp_t *tmp;
	while(p!=NULL) {
		tmp = p;
		free(tmp);
		p = p->next;
	}
}

int get_isp_no(char *name)
{
	common_isp_t *pnext = common_isp_link;
	while(pnext != NULL) {
		if (strstr((const char*)name, (const char*)(pnext->isp)) != NULL)
			return pnext->isp_id;
		pnext = pnext->next;
	}
	return 1000;
}

int main()
{
	unsigned int port = 3306;

	get_ready();

	read_ispno();

	//print_isp();

	MYSQL *handle;
	handle = mysql_init(0);
	mysql_options(handle,MYSQL_SET_CHARSET_NAME,"utf8");
	if (!mysql_real_connect(handle, host, user, pass, dbnm, port, 0, CLIENT_FOUND_ROWS))
	{
		printf("connect mysql server %s failed \e[31m%s\e[m\n", host, mysql_error(handle));
		return 0;
	}

	const char *sql1 =
			"(select start_ip, end_ip, province_code, city_code, isp,ip_span from t_city_ip) union \
			(select start_ip, end_ip, province_code, province_code as city_code, province_name as isp, \
			(end_ip-start_ip+1) as ip_span from t_country_ip, t_code_country, t_province_code \
			where t_country_ip.code2 = t_code_country.c2code and \
			t_code_country.country_cn = t_province_code.province_name) union \
            (select start_ip, end_ip, 830000 as province_code, 830000 as city_code, country_cn as isp,\
             (end_ip - start_ip +1 ) as ip_span from t_country_ip, t_code_country where \
             t_country_ip.`code2` = t_code_country.`c2code` \
             and `t_code_country`.`c2code` != 'HK'\
             and `t_code_country`.`c2code` != 'TW'\
             and `t_code_country`.`c2code` != 'MC'\
             and `t_code_country`.`c2code` != 'CN')";
            
	if (mysql_real_query(handle, sql1, strlen(sql1)) != 0)
	{
		printf("\e[31m%s\e[m", mysql_error(handle));
		return 0;
	}

	MYSQL_RES *result = mysql_store_result(handle);
	if(result == 0)
	{
		printf("\e[31m%s\e[m", mysql_error(handle));
		return false;
	}

	std::list<ip_info_t> provincelist;
	std::map<uint32_t, code_t> ipmap;

	MYSQL_ROW mysql_row;
	while ((mysql_row = mysql_fetch_row(result)) != 0)
	{
		ip_info_t t;
		t.start_ip = atoll(mysql_row[0]);
		t.end_ip = atoll(mysql_row[1]);
		t.province_code = atoll(mysql_row[2]);
		t.city_code = atoll(mysql_row[3]);
		snprintf((char*)(t.isp), sizeof(t.isp), "%s", mysql_row[4]);
		t.ipspan = atoll(mysql_row[5]);
		provincelist.push_back(t);
	}
	mysql_free_result(result);

	printf("province list size = %u\n", provincelist.size());
	code_t c;
	uint32_t forward_offset;
	uint32_t backward_offset;
	std::map<uint32_t, code_t>::iterator its;
	std::map<uint32_t, code_t>::iterator ite;
	std::list<ip_info_t>::iterator itlist;
	for (itlist = provincelist.begin(); itlist != provincelist.end(); ++itlist)
	{
		if (itlist->end_ip < itlist->start_ip || itlist->ipspan == 0)
			continue;

		forward_offset = 0;
		backward_offset = 0;

		its = ipmap.find(itlist->start_ip);
		ite = ipmap.find(itlist->end_ip);

		if (its == ipmap.end()) {
			//新的起点，或者起点+终点
			memset(&c, 0, sizeof(c));
			ipmap[itlist->start_ip] = c;
			if (itlist->ipspan == 1)
				ipmap[itlist->start_ip].type = 2;
			else
				ipmap[itlist->start_ip].type = 0;
			its = ipmap.find(itlist->start_ip);//统一用its操作 所以重新查找一下
		}
		else {
			if (itlist->ipspan == 1)
				continue;//该点已被占领，而新点既想当起点，又想当终点，则不允许。

			if (its->second.type == 0) {
				;//该点已经是起点，则地址信息设定为ipspan较小的点。这里可以删除 但是保留说明
			}
			else if(its->second.type == 1) {
				printf("add new start:already:%u,new:%u\n",its->first,itlist->start_ip);
				//该点已经是终点或者起点＋终点，新起点只能选择向后寻找新的起点
				for (backward_offset = 1; (itlist->start_ip + backward_offset) != 0; backward_offset ++) {
					if ((itlist->start_ip + backward_offset) > itlist->end_ip)
						break;
					if ((its = ipmap.find(itlist->start_ip + backward_offset)) == ipmap.end()) {
						memset(&c, 0, sizeof(c));
						ipmap[itlist->start_ip + backward_offset] = c;
						ipmap[itlist->start_ip + backward_offset].type = 0;
						break;	//新起点
					}
					else if (its != ipmap.end() && its->second.type == 0)
						break;//同样的起点
					//否则继续往后找
				}//for
				if (	( (itlist->start_ip + backward_offset) > itlist->end_ip )
					|| ( (itlist->start_ip + backward_offset) == itlist->end_ip && its != ipmap.end() )
					)
					continue; //没有位置了
			}//else if
		}
		its = ipmap.find(itlist->start_ip + backward_offset);

		if (its == ipmap.end()) {
			printf ("start unknown error!\n");
			exit (-1);
		}
		if (its->second.ipspan == 0 || its->second.ipspan > (itlist->ipspan - backward_offset)) {
			//设置为ipspan较小的点，或者为0的话第一次设定
			its->second.province_code = itlist->province_code;
			its->second.city_code = itlist->city_code;
			its->second.ipspan = itlist->ipspan - backward_offset;
			its->second.offset = 0;
			memcpy(its->second.isp, itlist->isp, sizeof(its->second.isp));
			snprintf((char*)(its->second.key), sizeof(its->second.key),
					"%u%s", its->second.city_code, (char*)(its->second.isp));
		}

		if (itlist->start_ip + backward_offset == itlist->end_ip)
			its->second.type = 2;

		if (its->second.ipspan == 1)
			continue;//必然是新的起点和终点，则终点不用继续处理了.

		//========================================
		if (ite == ipmap.end()) {
			//如果新点是新的起点+终点，则在上面判断新的起点时已经完成，continue了，这里不需要继续判断
			//新的终点
			memset(&c, 0, sizeof(c));
			ipmap[itlist->end_ip] = c;
			ipmap[itlist->end_ip].type = 1;
			ite = ipmap.find(itlist->end_ip);//统一用ite操作 所以重新查找一下
		}
		else {
			if (ite->second.type == 1) {
				;//该点已经是终点，则地址信息设定为ipspan较小的点 同理
			}
			else {
				//该点已经是起点或者起点+终点,新终点只能选择向前寻找新的终点
				printf("add new end:already:%u,new:%u\n",ite->first,itlist->end_ip);
				for (forward_offset = 1; (itlist->end_ip - forward_offset) != 0; forward_offset ++) {
					if ((itlist->end_ip - forward_offset) < itlist->start_ip)
						break;
					if ((ite = ipmap.find(itlist->end_ip - forward_offset)) == ipmap.end()) {
						memset(&c, 0, sizeof(c));
						ipmap[itlist->end_ip - forward_offset] = c;
						ipmap[itlist->end_ip - forward_offset].type = 1;
						break;	//新终点
					}
					else if (ite != ipmap.end() && ite->second.type == 1)
						break;//同样的终点
					//否则继续往前找
				}//for
				if ((itlist->end_ip - forward_offset) <= itlist->start_ip )
					continue; //没有位置了，之所以是同时判断了等于,是因为如果走到start_ip，而start已经设置.
			}
		}
		ite = ipmap.find(itlist->end_ip - forward_offset);

		if (ite == ipmap.end()) {
			printf ("end unknown error!\n");
			exit (-1);
		}
		if (ite->second.ipspan == 0 || ite->second.ipspan > (itlist->ipspan - backward_offset- forward_offset)) {
			//设置为ipspan较小的点，或者为0的话第一次设定
			ite->second.province_code = itlist->province_code;
			ite->second.city_code = itlist->city_code;
			ite->second.ipspan = itlist->ipspan - backward_offset - forward_offset;
			ite->second.offset = 0;
			memcpy(ite->second.isp, itlist->isp, sizeof(ite->second.isp));
			snprintf((char*)(ite->second.key), sizeof(ite->second.key),
					"%u%s", ite->second.city_code, (char*)(ite->second.isp));
		}
		if (forward_offset > 0)
			its->second.ipspan -= forward_offset;
	}

	memset(&c, 0, sizeof(c));
	ipmap[0xffffffff] = c;
	uint32_t size = ipmap.size();
	FILE *fp = fopen("ipdict.dat","wb+");
	if(fp == NULL )
	{
		fprintf(stderr,"\e[31mcan't open ipdict.dat\e[m\n");
		return -1;
	}
	printf("province map size(after processing!) = %u\n", size);
	fwrite(&size, sizeof(size), 1, fp);
	std::map<uint32_t, code_t>::iterator it;
	FILE *dddd = fopen("1","wt");
	for (it = ipmap.begin(); it != ipmap.end(); ++it)
	{
		uint32_t key = (*it).first;
		code_t code = (*it).second;
		code.isp_id = get_isp_no((char*)(code.isp));

		fprintf(dddd, "ip:%u,span:%u,type:%u,provice:%u,city:%u,isp:%s\n",
				key, code.ipspan,code.type,code.province_code,code.city_code,code.isp);

		fwrite(&key, sizeof(key), 1, fp);
		fwrite(&code, sizeof(code), 1, fp);
	}
	fclose(dddd);
	fclose(fp);
	printf("producing ipdict.dat\t\e[32m[ok]\e[0m\n");

	free_isp();
	return 0;
}


