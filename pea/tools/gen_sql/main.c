#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define LINE(...)               \
    fprintf(fp, __VA_ARGS__);   \
    fprintf(fp, "\n");



bool create_db()
{
    const char * name = "pea_db";
    char filename[1024];
    sprintf(filename, "%s.sql", name);
	FILE* fp = fopen(filename, "wt");

	if(fp == NULL)
	{
		printf("can not open file: %s", filename);
		return false;
	}

	for(int i =0; i <=99; i++)
	{
		LINE("set names utf8; create database pea_%02d;", i);
	}
	fclose(fp);
	return true;
}

bool create_table_pea_mail()
{
	const char* name = "pea_mail";
	char filename[1024] = {0};
	sprintf(filename, "%s.sql", name);
	FILE* fp = fopen(filename, "wt");

	if( fp == NULL)
	{
		printf("can not open file: %s", filename);
		return false;
	}

	for(int i =0; i <=99; i++)
	{
		for(int j =0; j<=9; j++)
		{
			fprintf(fp, "drop table if exists pea_%02d.%s_%d;\n", i, name, j);
			fprintf(fp, "create table pea_%02d.%s_%d\n", i, name, j);
			fprintf(fp, "(\n");
			fprintf(fp, "mail_id int(10) unsigned not null auto_increment primary key, \n");
			fprintf(fp, "mail_time int(10) unsigned not null default 0, \n");
			fprintf(fp, "mail_state int(10) unsigned not null default 0, \n");
			fprintf(fp, "mail_templet int(10) unsigned not null default 0, \n");
			fprintf(fp, "mail_type int(10) unsigned not null default 0, \n");
			fprintf(fp, "server_id int(10) unsigned not null default 0, \n");
			fprintf(fp, "sender_id int(10) unsigned not null default 0, \n");
			fprintf(fp, "sender_role_tm int(10) unsigned not null default 0, \n");
			fprintf(fp, "sender_nick varchar(1024) not null default '', \n");
			fprintf(fp, "receive_id int(10) unsigned not null default 0, \n");
			fprintf(fp, "receive_role_tm int(10) unsigned not null default 0, \n");
			fprintf(fp, "receive_nick varchar(1024) not null default '', \n");
			fprintf(fp, "mail_title varchar(1024)  not null default '', \n");
			fprintf(fp, "mail_content varchar(1024) not null default '', \n");
			fprintf(fp, "numerical_enclosure varchar(1024) not null default '', \n");
			fprintf(fp, "item_enclosure varchar(1024) not null default '', \n");
			fprintf(fp, "equip_enclosure varchar(1024) not null default '' \n");
			fprintf(fp, ");\n");
		}	
	}
	fclose(fp);
	return true;
}

bool create_table_pea_item()
{
    const char * name = "pea_item";
    char filename[1024];
    sprintf(filename, "%s.sql", name);
	FILE* fp = fopen(filename, "wt");

	if( fp == NULL)
	{
		printf("can not open file: %s", filename);
		return false;
	}

	for(int i =0; i <=99; i++)
	{
		for(int j =0; j<=9; j++)
		{
			fprintf(fp, "drop table if exists pea_%02d.%s_%d;\n", i, name, j);
			fprintf(fp, "create table pea_%02d.%s_%d\n", i, name, j);
			fprintf(fp, "(\n");
			fprintf(fp, "id int(10) unsigned not null auto_increment primary key, \n");
			fprintf(fp, "user_id int(10) unsigned not null default 0, \n");
			fprintf(fp, "role_tm int(10) unsigned not null default 0, \n");
			fprintf(fp, "server_id int(10) unsigned not null default 0,\n");
			fprintf(fp, "grid_index int(10) unsigned not null default 0,\n");
			fprintf(fp, "item_id int(10) unsigned not null default 0,\n");
			fprintf(fp, "item_count int(10) unsigned not null default 0,\n");
			fprintf(fp, "get_time int(10) unsigned not null default 0,\n");
			fprintf(fp, "expire_time int(10) unsigned not null default 0,\n");
			fprintf(fp, "unique key(user_id, role_tm, server_id, grid_index)\n");
			fprintf(fp, ");\n" );
		}
	}

    fclose(fp);
    return true;
}

bool create_table_pea_user()
{
    const char * name = "pea_user";
    char filename[1024];
    sprintf(filename, "%s.sql", name);
	FILE* fp = fopen(filename, "wt");

	if(fp == NULL)
	{
		printf("can not open file: %s", filename);
		return false;
	}

	for(int i =0; i <=99; i++)
	{
		for(int j =0; j<=9; j++)
		{
			fprintf(fp, "drop table if exists pea_%02d.pea_user_%d;\n", i, j);
			fprintf(fp, "create table pea_%02d.pea_user_%d\n", i, j);
			fprintf(fp, "(\n");
			fprintf(fp, "user_id int(10) unsigned not null default 0, \n");
			fprintf(fp, "role_tm int(10) unsigned not null default 0, \n");
			fprintf(fp, "nick    varchar(16)  not null default '', \n");
			fprintf(fp, "color int(10) unsigned not null default 0, \n");
			fprintf(fp, "resource_id int(10) unsigned not null default 0, \n");
			fprintf(fp, "server_id int(10) unsigned not null default 0,\n");
			fprintf(fp, "last_login_tm int(10) unsigned not null default 0, \n");
			fprintf(fp, "last_off_line_tm int(10) unsigned not null default 0, \n");
			fprintf(fp, "exp int(10) unsigned not null default 0, \n");
			fprintf(fp, "level int(10) unsigned not null default 0, \n");
			fprintf(fp, "max_bag_grid_count int(10) unsigned not null default 0, \n");
			fprintf(fp, "map_id int(10) unsigned not null default 0, \n");
			fprintf(fp, "map_x int(10) unsigned not null default 0, \n");
			fprintf(fp, "map_y int(10) unsigned not null default 0, \n");
			fprintf(fp, "gold int(10) unsigned not null default 0, \n");
            fprintf(fp, "primary key (user_id, role_tm, server_id)\n");
			fprintf(fp, ") engine=InnoDB default charset=utf8;\n\n");
		}
	}
	
	fclose(fp);
	return true;
}

bool create_add_column_pea_user()
{
	char sql[4096] = {0};
	char db_name[1024] = {0};
	char table_name[4096] = {0};

	FILE* fp = fopen("add_pea_user.sql", "wt");

	if(fp == NULL)
	{
		printf("can not open file: add_pea_user.sql");
		return false;
	}

	for(int i =0; i <=99; i++)
	{
		for(int j =0; j<=9; j++)
		{
			fprintf(fp, "alter table pea_%02d.pea_user_%d add color int(10) not null default 0 after nick;\n", i, j);
			fprintf(fp, "alter table pea_%02d.pea_user_%d add resource_id int(10) not null default 0 after color;\n", i, j);
		}
	}

	fclose(fp);
	return true;
}

int main(int argc, char** argv)
{
    create_db();
    create_table_pea_user();
    create_table_pea_item();
    create_table_pea_mail();
	return 0;
}
