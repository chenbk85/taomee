#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "stage.hpp"
#include "map_data.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>


extern "C"
{
#include <glib.h>
#include <assert.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>
}

stage_data_mgr* stage_data_mgr::get_instance()
{
	static stage_data_mgr obj;
	return &obj;
}

bool stage_data_mgr::init_all_xmls(const char* dir_name)
{
	DIR* dir = NULL;
	struct dirent* dirp = NULL;

	dir = opendir(dir_name);
	if(dir == NULL)
	{
		return false;
	}

	while( (dirp = readdir(dir)) != NULL)
	{
		if( strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)continue;
        if (dirp->d_name[0] == '.') continue;
		char path_name[1024] = {0};
		strcpy(path_name, dir_name);
		strcat(path_name, dirp->d_name);
		if( !is_file(path_name))continue;
		if( strstr(path_name, ".xml") == NULL)continue;
		if( strstr(path_name, "xml.") != NULL)continue;
		TRACE_TLOG("load stage xml file name = %s", path_name);
		init_xml(path_name);
	}
	return true;

}

bool stage_data_mgr::init_xml(const char* xml)
{
	xmlDocPtr   doc = NULL;
	xmlNodePtr  root = NULL;
	xmlNodePtr  stage_node = NULL;
	xmlNodePtr  pos_node = NULL;


	char err_msg[4096] = {0};

	doc = xmlParseFile(xml);
	if(doc == NULL)
	{
		sprintf(err_msg, "xml file %s is not exist", xml);
		throw XmlParseError(err_msg);
		return false;
	}

	root = xmlDocGetRootElement(doc);
	if(root == NULL)
	{
		xmlFreeDoc(doc);
		sprintf(err_msg, "xml file %s content is empty", xml);
		throw XmlParseError(err_msg);
		return false;
	}

	stage_node = root->xmlChildrenNode;
	while(stage_node)
	{
		if(xmlStrcmp(stage_node->name, reinterpret_cast<const xmlChar*>("stage")) == 0)
		{
			stage_data* p_data = new stage_data();
			get_xml_prop_def(p_data->map_id,  stage_node, "map_id", 0);
			if(p_data->map_id == 0)
			{
				xmlFreeDoc(doc);
				sprintf(err_msg, "invalid map_id = %u", p_data->map_id);
				throw XmlParseError(err_msg);
				return false;
			}

			if(is_stage_data_exist(p_data->map_id))
			{
				xmlFreeDoc(doc);
				sprintf(err_msg, "map_id = %u is exist in file %s", p_data->map_id, xml);
				throw XmlParseError(err_msg);
				return false;
			}
	
			pos_node = stage_node->xmlChildrenNode;
			while(pos_node)
			{
				if(xmlStrcmp(pos_node->name, reinterpret_cast<const xmlChar*>("blue")) == 0)
				{
					pos pt;
					get_xml_prop_def(pt.x,   pos_node, "x", 0);
					get_xml_prop_def(pt.y,   pos_node, "y", 0);
					get_xml_prop_def(pt.dir, pos_node, "dir", 0);
					p_data->blue_pos.push_back(pt);
				}
				if(xmlStrcmp(pos_node->name, reinterpret_cast<const xmlChar*>("red")) == 0)
				{
					pos pt;
					get_xml_prop_def(pt.x,  pos_node, "x", 0);
					get_xml_prop_def(pt.y,  pos_node, "y", 0);
					get_xml_prop_def(pt.dir, pos_node, "dir", 0);
					p_data->red_pos.push_back(pt);
				}	
				pos_node = pos_node->next;	
			}
			add_stage_data(p_data);
		}	
		stage_node = stage_node->next;	
	}

	xmlCleanupParser();
	xmlFreeDoc(doc);
	return true;	
}


bool stage_data_mgr::final()
{
	map<uint32_t, stage_data*>::iterator pItr = datas.begin();
	for(; pItr != datas.end(); ++pItr)
	{
		stage_data* p_data = pItr->second;
		delete p_data;
		p_data = NULL;
	}
	return true;	
}

bool stage_data_mgr::is_stage_data_exist(uint32_t map_id)
{
	map<uint32_t, stage_data*>::iterator pItr = datas.find(map_id);
	return pItr != datas.end();
}

stage_data* stage_data_mgr::get_stage_data_by_id(uint32_t map_id)
{
	map<uint32_t, stage_data*>::iterator pItr = datas.find(map_id);
	if(pItr == datas.end())return NULL;
	return pItr->second;
}

bool stage_data_mgr::add_stage_data(stage_data* data)
{
	if(is_stage_data_exist(data->map_id))return false;
	datas[data->map_id] = data;
	return true;
}
