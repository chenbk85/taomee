#include "server_task.h"
#include "tinyxml.h"


bool load_server_task(const char* xml_name,  std::map<int, task_data>& task_datas)
{
	if(xml_name == NULL)return false;
	TiXmlDocument doc(xml_name);
	bool bRet = doc.LoadFile();
	if(!bRet)
	{
		const char* pErr = doc.ErrorDesc();
		return false;
	}

	TiXmlNode* root =NULL;
	TiXmlNode* TaskElement = NULL;
	TiXmlNode* InItemsElement = NULL;
	TiXmlNode* OutItemsElement = NULL;
	
	root =doc.FirstChild("Tasks");
	if(root == NULL)return false;

	TaskElement = root->FirstChild("Task");
	if(TaskElement == NULL)return false;

	while(TaskElement)
	{
		task_data data;
		TaskElement->ToElement()->Attribute("id", (int*)&(data.task_id));
		strcpy(data.name, TaskElement->ToElement()->Attribute("name"));
		TaskElement->ToElement()->Attribute("type", (int*)&(data.type));
		strcpy(data.name, TaskElement->ToElement()->Attribute("need_lv"));
		TaskElement->ToElement()->Attribute("need_role", (int*)&(data.need_role));
		TaskElement->ToElement()->Attribute("vip_limit", (int*)&(data.vip_limit));
		TaskElement->ToElement()->Attribute("master_task_id", (int*)&(data.master_task_id));
		TaskElement->ToElement()->Attribute("difficulty", (int*)&(data.diffculty));	
		//////////////////////////////////////
		InItemsElement = TaskElement->FirstChild("InItem");
		while(InItemsElement)
		{
			in_items in_items_data;
			InItemsElement->ToElement()->Attribute("id", (int*)&(in_items_data.id));
			TiXmlNode* ItemsElement = InItemsElement->FirstChild("Item");
			while(ItemsElement)
			{
				in_item_data in_data;
				ItemsElement->ToElement()->Attribute("give_id", (int*)&(in_data.give_id));
				ItemsElement->ToElement()->Attribute("give_type", (int*)&(in_data.give_type));
				ItemsElement->ToElement()->Attribute("cnt", (int*)&(in_data.count));
				ItemsElement->ToElement()->Attribute("monster_id", (int*)&(in_data.monster_id));
				ItemsElement->ToElement()->Attribute("stage", (int*)&(in_data.stage));
				ItemsElement->ToElement()->Attribute("drop_odds", (int*)&(in_data.drop_odds));
				strcpy(in_data.name, ItemsElement->ToElement()->Attribute("name"));
				in_items_data.datas.push_back(in_data);
				ItemsElement = ItemsElement->NextSibling("Item");
			}			
			data.in_maps[in_items_data.id] = in_items_data;
			InItemsElement = InItemsElement->NextSibling("InItem");
		}
		//////////////////////////////////////
		OutItemsElement = TaskElement->FirstChild("OutItem");
		while(OutItemsElement)
		{
			out_items out_items_data;
			OutItemsElement->ToElement()->Attribute("id", (int*)&(out_items_data.id));
			TiXmlNode* ItemsElement = OutItemsElement->FirstChild("Item");
			while(ItemsElement)
			{
				out_item_data out_data;
				ItemsElement->ToElement()->Attribute("give_id", (int*)&(out_data.give_id));
				ItemsElement->ToElement()->Attribute("give_type", (int*)&(out_data.give_type));
				ItemsElement->ToElement()->Attribute("cnt", (int*)&(out_data.count));
				strcpy(out_data.name, ItemsElement->ToElement()->Attribute("name"));
				out_items_data.datas.push_back(out_data);
				ItemsElement = ItemsElement->NextSibling("Item");
			}
			data.out_maps[out_items_data.id] = out_items_data;
			OutItemsElement = OutItemsElement->NextSibling("OutItem");
		}
		task_datas[data.task_id] = data;
		TaskElement = TaskElement->NextSibling("Task");
	}


	return true;
}