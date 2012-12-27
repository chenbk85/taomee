/*
 * =====================================================================================
 *
 *			 Filename:	input_shm.cpp
 *
 *		Description：向共享区存入装扮价格表。 ../etc/items.xml
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
using namespace std;


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "Cattire_conf.h"
vector<ATTIRE_INFO> v_attire;

void
parse_attire_info( xmlNodePtr cur, ATTIRE_INFO * a_info  ){
	xmlChar *key;
	key= xmlGetProp(cur,	(const xmlChar *)	"ID");
	a_info->id=atoi((const char *) key);
	key= xmlGetProp(cur,	(const xmlChar *)	"Price");
	a_info->price=atoi((const char *) key);
}

void
parse_kind_info(xmlDocPtr doc, xmlNodePtr cur )
{
	ATTIRE_INFO a_info;
	cur = cur->xmlChildrenNode;
	while (cur != NULL)
	{
		if ((!xmlStrcmp (cur->name, (const xmlChar *) "Item")))
		{
			printf ( "	deal item...\n");
			parse_attire_info ( cur, &a_info);
			v_attire.push_back(a_info );
		}
		cur = cur->next;
	}
	return;
}

static void
parseDoc (char *docname)
{

	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile (docname);

	if (doc == NULL)
		{
			fprintf (stderr, "Document not parsed successfully. \n");
			return;
		}

	cur = xmlDocGetRootElement (doc);

	if (cur == NULL)
	{
		fprintf (stderr, "empty document\n");
		xmlFreeDoc (doc);
		return;
	}

	if (xmlStrcmp (cur->name, (const xmlChar *) "Items"))
	{
		fprintf (stderr, "document of the wrong type, root node !=Items ");
		xmlFreeDoc (doc);
		return;
	}

	cur = cur->xmlChildrenNode;
	while (cur != NULL)
	{
		if ((!xmlStrcmp (cur->name, (const xmlChar *) "Kind")))
		{
			printf ( "deal Kind...\n");
			parse_kind_info (doc, cur);
		}
		cur = cur->next;
	}

	xmlFreeDoc (doc);
	return;
}

int
main (int argc, char **argv)
{

	char *docname;
	void * shm_pos;
	int shmid;
	ATTIRE_INFO *a;
	if (argc <= 1){
			printf ("Usage: %s docname\n", argv[0]);
			return (0);
	}
	v_attire.clear();
	docname = argv[1];
	
	parseDoc (docname);
	std::sort(v_attire.begin(),v_attire.end());
	if((shmid = shmget(13003 , SHM_SIZE, IPC_CREAT | 0660 )) == -1){
		return(-1);
	}	

	shm_pos=shmat(shmid , 0, 0);
	if (v_attire.size()<ATTIRE_INFO_MAXNUM ){
		printf("----[%u]===\n",v_attire.size());
		*((uint32_t*)shm_pos)=v_attire.size();
		memcpy((char *)shm_pos+4,v_attire.data(),v_attire.size()*sizeof(ATTIRE_INFO)  );
	}
	a=(ATTIRE_INFO * )((char *)shm_pos+4);
	printf("count:%u \n",*((uint32_t*)shm_pos) );
	for (int i=0; (uint32_t)i< *((uint32_t*)shm_pos);i++){
		printf("item[%d]:%u %u \n",i,(a+i)->id, (a+i)->price );
	}

	return 0;
}
