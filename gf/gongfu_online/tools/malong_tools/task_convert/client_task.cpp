#include "client_task.h"
#include "tinyxml.h"

bool load_client_task(const char* xml_name,  std::map<int, client_task>& task_datas)
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
	TiXmlNode* tasks_node =NULL;
	TiXmlNode* task_node =NULL;
	TiXmlNode* condition_node =NULL;
	TiXmlNode* rewards_node =NULL;
	TiXmlNode* pro_node =NULL;
	TiXmlNode* normal_node =NULL;
	TiXmlNode* select_node =NULL;
	TiXmlNode* fix_node =NULL;
	TiXmlNode* doc_node =NULL;
	TiXmlNode* end_doc_node =NULL;

	char *temp = NULL;

	root = doc.FirstChild("root");
	if(root == NULL)return false;
	tasks_node = root->FirstChild("tasks");
	if(tasks_node == NULL)return false;
	task_node = tasks_node->FirstChild("task");
	
	while(task_node)
	{
		client_task task;
		//--------------task_node---------------//
		task_node->ToElement()->Attribute("id", (int*)&(task.task_id));
		temp = (char*)task_node->ToElement()->Attribute("name");
		if(temp)
		{
			strcpy(task.name, temp);
		}
		
		task_node->ToElement()->Attribute("parent", (int*)&(task.parent), -1);
		task_node->ToElement()->Attribute("type", (int*)&(task.type));
		
		temp = (char*)task_node->ToElement()->Attribute("alert");
		if(temp)
		{
			strcpy(task.alert, temp);
		}
		
		task_node->ToElement()->Attribute("taskLevel", (int*)&(task.task_level));
		task_node->ToElement()->Attribute("roleLevel", (int*)&(task.role_level));

		temp = (char*)task_node->ToElement()->Attribute("roleType");
		if(temp)
		{
			strcpy(task.role_type, temp);
		}

		temp = (char*)task_node->ToElement()->Attribute("start");
		if(temp)
		{
			strcpy(task.start, temp);
		}

		temp = (char*)task_node->ToElement()->Attribute("end");
		if(temp)
		{
			strcpy(task.end, temp);
		}

		temp = (char*)task_node->ToElement()->Attribute("startTran");
		if(temp)
		{
			strcpy(task.start_tran, temp);
		}

		temp = (char*)task_node->ToElement()->Attribute("endTran");
		if(temp)
		{
			strcpy(task.end_tran, temp);
		}
		
		task_node->ToElement()->Attribute("nextAccept", (int*)&(task.next_accept));
		task_node->ToElement()->Attribute("showAcceptAnimation", (int*)&(task.show_accept_animation));
		///////////////condition node/////////////////////
		condition_node  = task_node->FirstChild("condition");
		if(condition_node)
		{
			pro_node = 	condition_node->FirstChild("pro");
			while(pro_node)
			{
				pro_data pro;
				pro_node->ToElement()->Attribute("id", (int*)&(pro.id));
				pro_node->ToElement()->Attribute("parent", (int*)&(pro.parent), -1);
				pro_node->ToElement()->Attribute("visible", (int*)&(pro.visible));
				
				temp =  (char*)pro_node->ToElement()->Attribute("doc");
				if(temp)
				{
					strcpy(pro.doc,  temp);
				}
				
				temp = (char*)pro_node->ToElement()->Attribute("proc");
				if(temp)
				{
					strcpy(pro.proc,  temp);
				}
				
				temp = (char*)pro_node->ToElement()->Attribute("params");
				if(temp)
				{
					strcpy(pro.params, temp);
				}

				temp = (char*)pro_node->ToElement()->Attribute("alert");
				if(temp)
				{
					strcpy(pro.alert, temp);
				}
				
				temp = (char*)pro_node->ToElement()->Attribute("tran");
				if(temp)
				{
					strcpy(pro.tran, temp);	
				}
				
				task.condition_data.condition_datas.push_back(pro);

				pro_node = pro_node->NextSibling("pro");
			}
		}
		////////////////////rewards node///////////////////////////
		rewards_node = task_node->FirstChild("rewards");
		if(rewards_node)
		{
			normal_node = rewards_node->FirstChild("normal");
			if(normal_node)
			{
				normal_node->ToElement()->Attribute("exp", (int*)&(task.rewards_data.normal.exp));	
				normal_node->ToElement()->Attribute("cash", (int*)&(task.rewards_data.normal.cash));
			}

			select_node = rewards_node->FirstChild("selected");
			while(select_node)
			{
				selected_data select;
				select_node->ToElement()->Attribute("id", (int*)&(select.id));
				select_node->ToElement()->Attribute("role", (int*)&(select.role));	
				select_node->ToElement()->Attribute("count", (int*)&(select.count));
				strcpy(select.name, select_node->ToElement()->Attribute("name"));	
				task.rewards_data.select.push_back(select);

				select_node = select_node->NextSibling("selected");
			}

			fix_node  =   rewards_node->FirstChild("fixed");
			while(fix_node)
			{
				selected_data select;	
				fix_node->ToElement()->Attribute("id", (int*)&(select.id));
				fix_node->ToElement()->Attribute("role", (int*)&(select.role));	
				fix_node->ToElement()->Attribute("count", (int*)&(select.count));
				strcpy(select.name, fix_node->ToElement()->Attribute("name"));	
				task.rewards_data.fix.push_back(select);

				fix_node = fix_node->NextSibling("fixed");
			}
		}
		/////////////////////doc node//////////////////////////
		doc_node = task_node->FirstChild("doc");
		if(doc_node)
		{
			TiXmlNode* pText = doc_node->FirstChild();
			strcpy(task.doc, pText->Value());
		}
		/////////////////////end doc node//////////////////////
		end_doc_node = task_node->FirstChild("endDoc");
		if(end_doc_node)
		{
			TiXmlNode* pText = end_doc_node->FirstChild();
			strcpy(task.end_doc, pText->Value());
		}

		task_datas[task.task_id] = task;
		task_node = task_node->NextSibling("task");
	}
	return true;
}


bool save_client_task(const char* xml_name,  std::map<int, client_task>& task_datas)
{
	if(xml_name == NULL)return false;
	TiXmlDocument* pdoc = new TiXmlDocument;
	TiXmlElement* root = new TiXmlElement("root");
	pdoc->LinkEndChild(root);
	TiXmlElement* tasks = new TiXmlElement("tasks");
	
	
	std::map<int, client_task>::iterator pItr = task_datas.begin();

	for(; pItr != task_datas.end(); ++pItr)
	{
		client_task* p_task = &(pItr->second);
		TiXmlElement* task_node = new TiXmlElement("task");
		
		task_node->SetAttribute("id", p_task->task_id);
		task_node->SetAttribute("name", p_task->name);
		if(p_task->parent != -1)
		{
			task_node->SetAttribute("parent", p_task->parent);
		}
		task_node->SetAttribute("type", p_task->type);
		task_node->SetAttribute("alert", p_task->alert);
		task_node->SetAttribute("taskLevel", p_task->task_level);
		task_node->SetAttribute("roleLevel", p_task->role_level);
		task_node->SetAttribute("roleType", p_task->role_type);
		task_node->SetAttribute("start", p_task->start);
		task_node->SetAttribute("end", p_task->end);
		task_node->SetAttribute("startTran", p_task->start_tran);
		task_node->SetAttribute("endTran", p_task->end_tran);
		task_node->SetAttribute("nextAccept", p_task->next_accept);
		task_node->SetAttribute("showAcceptAnimation", p_task->show_accept_animation);
		task_node->SetAttribute("vip_limit", p_task->vip_limit);
		task_node->SetAttribute("master_task_id", p_task->master_task_id);
		////////////////////condition节点/////////////////////
		TiXmlElement* condition_node = new TiXmlElement("condition");	
		for(unsigned int i=0; i< p_task->condition_data.condition_datas.size(); i++)
		{
			pro_data* p_pro_data = &p_task->condition_data.condition_datas[i];
			TiXmlElement* pro_node = new TiXmlElement("pro");	
			pro_node->SetAttribute("id", p_pro_data->id);
			if(p_pro_data->parent != -1)
			{
				pro_node->SetAttribute("parent", p_pro_data->parent);
			}
			pro_node->SetAttribute("visible", p_pro_data->visible);
			pro_node->SetAttribute("doc", p_pro_data->doc);
			pro_node->SetAttribute("alert", p_pro_data->alert);
			pro_node->SetAttribute("proc", p_pro_data->proc);
			pro_node->SetAttribute("params", p_pro_data->params);
			pro_node->SetAttribute("tran", p_pro_data->tran);
			if( strcmp(p_pro_data->params, "TaskXML_ItemPay") == 0)
			{
				if(p_pro_data->monster_id != 0)
				{
					pro_node->SetAttribute("give_type", p_pro_data->give_type);
					pro_node->SetAttribute("monster_id", p_pro_data->monster_id);
					pro_node->SetAttribute("stage", p_pro_data->stage);
					pro_node->SetAttribute("drop_odds", p_pro_data->drop_odds);
				}
			}
			condition_node->InsertEndChild(*pro_node);
		}
		task_node->InsertEndChild(*condition_node);
		//////////////////rewards节点/////////////////////////////
		TiXmlElement* rewards_node = new TiXmlElement("rewards");	
		for(unsigned int i =0; i< p_task->rewards_data.fix.size(); i++)
		{
			selected_data* p_fix = &p_task->rewards_data.fix[i];
			TiXmlElement* fix_node = new TiXmlElement("fixed");
			fix_node->SetAttribute("id", p_fix->id);
			fix_node->SetAttribute("role", p_fix->role);
			fix_node->SetAttribute("name", p_fix->name);
			fix_node->SetAttribute("count", p_fix->count);
			rewards_node->InsertEndChild(*fix_node);
		}
		for(unsigned int i =0; i< p_task->rewards_data.select.size(); i++)
		{
			selected_data* p_select = &p_task->rewards_data.select[i];
			TiXmlElement* select_node = new TiXmlElement("selected");
			select_node->SetAttribute("id", p_select->id);
			select_node->SetAttribute("role", p_select->role);
			select_node->SetAttribute("name", p_select->name);
			select_node->SetAttribute("count", p_select->count);
			rewards_node->InsertEndChild(*select_node);
		}
		TiXmlElement* normal_node = new TiXmlElement("normal");
		normal_node->SetAttribute("exp", p_task->rewards_data.normal.exp);
		normal_node->SetAttribute("cash", p_task->rewards_data.normal.cash);
		rewards_node->InsertEndChild(*normal_node);
	
		task_node->InsertEndChild(*rewards_node);
		/////////////////////////doc////////////////////////////////////
		TiXmlElement* doc_node = new TiXmlElement("doc");
		TiXmlText *pText = new TiXmlText(p_task->doc);
		pText->SetCDATA(true);
		doc_node->InsertEndChild(*pText);
		task_node->InsertEndChild(*doc_node);
		////////////////////////end doc/////////////////////////////////
		TiXmlElement* end_node = new TiXmlElement("endDoc");
		pText = new TiXmlText(p_task->end_doc);
		pText->SetCDATA(true);
		end_node->InsertEndChild(*pText);
		task_node->InsertEndChild(*end_node);
		///////////////////////////////////////////////////////////////
		tasks->InsertEndChild(*task_node);
	}
	root->InsertEndChild(*tasks);


	return pdoc->SaveFile(xml_name);
}