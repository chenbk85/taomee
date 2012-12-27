/*
 * =====================================================================================
 *
 *			 Filename:	input_shm.cpp
 *
 *		Description:	
 *
 *				Version:	1.0
 *				Created:	2008年01月14日 11时08分58秒 CST
 *			 Revision:	none
 *			 Compiler:	gcc
 *				 Author:	xcwen (xcwen), xcwenn@gmail.com
 *				Company:	TAOMEE
 * 		
 * 		------------------------------------------------------------
 * 		view configure:
 * 			VIM:	set tabstop=4 
 * 		
 * =====================================================================================
 */
#include <algorithm>
#include <vector>
#include "benchapi.h" 
using namespace std;


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "mysql_iface.h"
typedef  struct item_info {
	uint32_t type;
	uint32_t gencount;
} ITEM_INFO; 

typedef struct serial_info  {
	uint32_t batchid;
	uint32_t serial_table_id;
	uint32_t start_serial_id;
	char gendate[20];
	char stopdate[20];
	uint32_t used ;
	vector<ITEM_INFO> v_item;
} 	SERIAL_INFO ; 


void 
parse_item_info( xmlNodePtr cur,  ITEM_INFO & a_info  ){
	xmlChar *key;

	key= xmlGetProp(cur,	(const xmlChar *)	"type");
	 a_info.type=atoi((const char *) key );
	key= xmlGetProp(cur,	(const xmlChar *)	"gencount");
	 a_info.gencount=atoi((const char *) key );
}
int 
parseDoc (char *docname, SERIAL_INFO &serial_config)
{

	xmlDocPtr doc;
	xmlNodePtr cur;
	xmlChar *key;
	ITEM_INFO  a_info;

	if ((doc = xmlParseFile (docname))==NULL) {
		fprintf (stderr, "Document not parsed successfully. \n");
		return FAIL;
	}
	if ((cur = xmlDocGetRootElement (doc)) == NULL){
		fprintf (stderr, "empty document\n");
		xmlFreeDoc (doc);
		return FAIL;
	}

	if (xmlStrcmp (cur->name, (const xmlChar *) "items")){
		fprintf (stderr, "document of the wrong type, root node !=items ");
		xmlFreeDoc (doc);
		return FAIL;
	}

	key= xmlGetProp(cur,	(const xmlChar *)"batchid");
	serial_config.batchid=atoi((const char *) key );
	key= xmlGetProp(cur,	(const xmlChar *)"serailtableid");

	serial_config.serial_table_id=atoi((const char *) key );
	key= xmlGetProp(cur,(const xmlChar *)"gendate");
	strncpy(serial_config.gendate,((const char *) key ),19 );
	key= xmlGetProp(cur,(const xmlChar *)"stopdate");
	strncpy(serial_config.stopdate,((const char *) key ),19);

	key= xmlGetProp(cur,(const xmlChar *)"used");
	serial_config.used=atoi((const char *) key );

	key= xmlGetProp(cur,(const xmlChar *)"start_serial_id");
	serial_config.start_serial_id=atoi((const char *) key );

	cur = cur->xmlChildrenNode;
	while (cur != NULL)
	{
		if ((!xmlStrcmp (cur->name, (const xmlChar *) "item")))
		{
			parse_item_info ( cur, a_info);
			serial_config.v_item.push_back(a_info);
		}
		cur = cur->next;
	}
	xmlFreeDoc (doc);
	return SUCC;
}
int check_config(SERIAL_INFO &serial_config ){
	uint32_t totalgencount=0;
	//check
	if (serial_config.batchid>9222){
		printf ("err: 批次号：%d >9222\n", serial_config.batchid );
		return FAIL;
	}

	if (serial_config.serial_table_id>99){
		printf ("err:对应的序列号表ID：%u >99\n",serial_config.serial_table_id);
		return FAIL;
	}

	printf ("------------------------------------------------\n" );
	printf ("	批次号          ：%d \n",serial_config.batchid );
	printf ("	对应的序列号表ID：%u \n",serial_config.serial_table_id);
	printf ("	序号id累加起始值：%u \n",serial_config.start_serial_id);
	printf ("	产生日期        ：%s \n",serial_config.gendate);
	printf ("	停用日期        ：%s \n",serial_config.stopdate);
	printf ("	使用标志        ：%u \n",serial_config.used);

	vector<ITEM_INFO>::iterator it;
	for(it=serial_config.v_item.begin(); it!=serial_config.v_item.end(); ++it )
	{
		printf ("	    类型:%u	产生数:%u \n",it->type, it->gencount );
		totalgencount+=it->gencount;
	}
	printf ("	产生总数:%u \n",totalgencount  );
	printf ("------------------------------------------------\n" );
	printf ("	输入'y',执行:" );
	return SUCC;
	if (getchar()=='y'){
		return SUCC;
	}else{
		return FAIL;
	}
}

int insert_data(SERIAL_INFO &serial_config )
{
	char sqlstr[1000];
	int affect_row, ret; 
	uint32_t rand_tmp,id_tmp;
	char serial_table_name[40];
	char serialid[30];
	FILE *fp=fopen("/dev/urandom","r");

	mysql_interface  db("localhost","root","ta0mee");	
	sprintf(serial_table_name,"SERIAL_DB.t_serial_%02d",serial_config.serial_table_id );
	//create table 
	sprintf(sqlstr," CREATE TABLE %s (\
				  id INT UNSIGNED,\
				  serialid  BIGINT UNSIGNED,\
				  type INT UNSIGNED,\
				  useflag INT UNSIGNED,\
				  PRIMARY KEY  (serialid)\
					  ) ENGINE=MyISAM CHARSET=utf8", serial_table_name  );
	if ( (ret=db.exec_update_sql( sqlstr,&affect_row) )!=DB_SUCC){
		printf("表已存在DBERR [%d]\n",ret)	;
		//return FAIL;
	}

	printf("创建表完成[%s]\n",serial_table_name );

	//insert serialid
	uint32_t total_i =0;
	vector<ITEM_INFO>::iterator it;
	for(it=serial_config.v_item.begin(); it!=serial_config.v_item.end(); ++it )
	{
		uint32_t i=total_i ;
		total_i+=it->gencount;
		for (;i<total_i;i++){
			fread(&rand_tmp,4,1,fp);	
			fread(&id_tmp,4,1,fp);	
			//sprintf (serialid,"%04u%02u%06u%07u",serial_config.batchid,
			//		serial_config.serial_table_id,
			//		i,rand_tmp%10000000);
			sprintf (serialid,"%04u%02u%06u%07u",serial_config.batchid,
					serial_config.serial_table_id,
					id_tmp%1000000,rand_tmp%10000000);
			uint64_t  serialid_uint= atoll(serialid); 
			char serialid_str[30];
			strcpy(serialid_str ,get_serialid_str(serialid_uint ));
			if (i==0) 
				printf ("serialid=%s\n",serialid_str);
			if (check_serial_str(serialid_str)){
				sprintf(sqlstr,"insert into %s values(%u, '%s',%u,0 )  ",
					serial_table_name,serial_config.start_serial_id+i, serialid ,it->type );
				if ( (ret=db.exec_update_sql( sqlstr,&affect_row) )!=DB_SUCC){
					i--;
				}else{
					DEBUG_LOG("%u:%llu:%s:%llu" ,serial_config.start_serial_id+i ,
							serialid_uint, serialid_str, get_serialid(serialid_str)   );
				}
			} else{ 
				DEBUG_LOG("fail:%s",  serialid_str  );
				i--;
			}

			
		}	
	}
	printf("插入数据完成\n");
	return SUCC;
}
int
main (int argc, char **argv)
{
	SERIAL_INFO serial_config;
	char *docname;
	memset(&serial_config,0,sizeof(SERIAL_INFO) );
	if (argc <= 1){
			printf ("Usage: %s config.xml \n", argv[0]);
			return (0);
	}
	docname = argv[1];
	if (parseDoc (docname,serial_config )!=SUCC) {
		printf("XML解析失败\n");
		return 1;
	};

	if (check_config(serial_config)!=SUCC){
		printf("执行：不执行 \n");
		return 1;
	}

	if (insert_data(serial_config )!=SUCC){
		printf("执行:失败 查看日志\n");
		return 1;
	}

	printf("执行：完成\n");
	return SUCC;
	
}

