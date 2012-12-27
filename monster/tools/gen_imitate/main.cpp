/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年02月15日 12时12分37秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <errno.h>
using namespace std;

#define INIT_TIME 20

//输入文件的格式为：
//正确与错误(0:正确 1：错误) 剩余时间(0;表示答题结束)
//

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        cout<<"parameter error, format:"<<argv[0] << " input_file_name"<<endl;
        return -1;
    }

    FILE *fp =  fopen(argv[1], "r");
    if(fp == NULL)
    {
        cout << argv[1] << "open failed" <<endl;
        return -1;
    }

    char buf[100];

    xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST"imitate_answer");

    xmlDocSetRootElement(doc, root_node);//设置根节点

    int imitate_num = 1;
    int step = 0;
    char *delim = " ";
    xmlNodePtr node = xmlNewNode(NULL, BAD_CAST"imitate");
    xmlAddChild(root_node, node);
    xmlNewProp(node, BAD_CAST"id", BAD_CAST"1");

    int last_remain_time = INIT_TIME;
    int correct_num = 0;
    char attr_value[100] = {0};
    char *p = NULL;
   p = fgets(buf, 100, fp);
   // while(fgets(buf, 100, fp))
    while(p)
    {
//        cout<<"buf = "<<buf<<endl;
        memset(attr_value, 0, sizeof(attr_value));
        char *right_or_wrong = strtok(buf, delim);
        char *remain_time_str = strtok(NULL, delim);

        int is_right = atoi(right_or_wrong);
        if(is_right == 0)
        {
            correct_num++;
        }

        int remain_time = atoi(remain_time_str);
        if(remain_time > INIT_TIME)
        {
            remain_time = 0;
        }

        memset(buf, 0, sizeof(buf));
        if(last_remain_time == INIT_TIME && remain_time == 0)
        {//跳过,表示用户未答题
        	p = fgets(buf, 100, fp);
        	if(p == NULL)
        	{
            		break;
        	}
            	continue;
        }
        int next_time = last_remain_time - remain_time;
        last_remain_time = remain_time;

        xmlNodePtr step_node = xmlNewNode(NULL, BAD_CAST"step");
        xmlAddChild(node, step_node);

        step++;

        sprintf(attr_value, "%d", step);
        xmlNewProp(step_node, BAD_CAST"id", (xmlChar*)attr_value);

        sprintf(attr_value, "%d", next_time);
        xmlNewProp(step_node, BAD_CAST"next_time", (xmlChar*)attr_value);

        sprintf(attr_value, "%d", correct_num);
        xmlNewProp(step_node, BAD_CAST"correct_num", (xmlChar*)attr_value);

        sprintf(attr_value, "%d", remain_time);
        xmlNewProp(step_node, BAD_CAST"remain_time", (xmlChar*)attr_value);


        p = fgets(buf, 100, fp);
        if(p == NULL)
        {
            break;
        }

        if(remain_time == 0 || remain_time > INIT_TIME)
        {//结束
            imitate_num += 1;
            step = 0;
            last_remain_time = INIT_TIME;
            correct_num = 0;
            node = xmlNewNode(NULL, BAD_CAST"imitate");
            xmlAddChild(root_node, node);
            sprintf(attr_value, "%d", imitate_num);
            xmlNewProp(node, BAD_CAST"id", (xmlChar*)attr_value);
        }
    }




     int ret = xmlSaveFormatFile("./imitate.xml", doc, 1);

     fclose(fp);
    if(ret == -1)
    {
        cout<<"write to xml failed."<<endl;
    }

    xmlFreeDoc(doc);
    return 0;
}
