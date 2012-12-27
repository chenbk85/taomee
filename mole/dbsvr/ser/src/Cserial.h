/*
 * =====================================================================================
 * 
 *       Filename:  Cserial.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CSERIAL_INCL
#define  CSERIAL_INCL
#include "Ctable.h"
#define USERID_NULL 	0
#define PRICE_TYPE_MAX  20
#include "proto.h"
#include <map>
using namespace std;

typedef struct price_type_list {
	uint32_t count;  	
	uint32_t list[PRICE_TYPE_MAX];  	
}__attribute__((packed)) PRICE_TYPE_LIST ; 

typedef struct price_type {
	uint32_t type;  	
	uint32_t price;  	
}__attribute__((packed)) PRICE_TYPE ; 


typedef map<uint64_t, uint32_t > SERAIL2TYPE;
/*
typedef struct serial_conf {
 	char 	 serial_str[13];
 	uint32_t type;
} SERIAL_CONF;	
static SERIAL_CONF g_serial_conf[]={
	{ "2FRT2824Y5QW",10001 },
	{ "2EAA6TFB3W5C",10001 }
};
*/

/*
号码1：2FRT2824Y5QW
号码2：2EAA6TFB3W5C
*/
class Cserial :Ctable{
	protected:
		PRICE_TYPE_LIST price_type_list;
		SERAIL2TYPE  serail_def; 
		uint32_t pre_tableid;
		char  serail_str_c[SERIAL_STR_LEN+1 ];
		uint64_t  serial_id;

		char * get_table_name(uint64_t serialid);
		inline void set_serail_str( char * serialid_str  );
		inline int update_useflag(uint64_t serialid , uint32_t useflag);
		inline int get_useflag(uint64_t serialid , uint32_t *p_useflag);
	public:
		Cserial(mysql_interface * db ); 
		int set_used( uint64_t serialid , uint32_t useflag, uint32_t * p_price);
		int set_used_by_serialid_str(char*  serialid_str, uint32_t * p_type);
		int set_unused( uint64_t serialid  );
		int get_noused_id( uint32_t type,  uint64_t * serialid);
		int get_info(uint64_t serialid , uint32_t *p_useflag,  uint32_t * p_type );
		int get_info_by_serialid_str(char * serialid_str, uint32_t *p_useflag,  uint32_t * p_type );
};



#endif   /* ----- #ifndef CSERIAL_INCL  ----- */

