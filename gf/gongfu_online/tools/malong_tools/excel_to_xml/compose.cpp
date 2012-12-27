#include "stdafx.h"
#include "compose.h"
#include "tinyxml.h"
#include "character_set.h"




bool method_mgr::init_from_excel(const char* excel)
{
	CSpreadSheet ss(excel, "Sheet1");

	std::map<int, CString> column_names;
	for(int i = 1; i<= ss.GetTotalColumns(); i++)
	{
		CString name;
		ss.ReadCell(name, i, 1);
		column_names[i] = name;
	}
	for(int i = 2; i<= ss.GetTotalRows(); i++)
	{
		method_data data;
		for(int j = 1; j< ss.GetTotalColumns(); j++)
		{
			CString value;
			ss.ReadCell(value, j, i);
			if( column_names[j] == "Name" && value != "")
			{
				std::string utf_str = GBToUTF8(value.GetBuffer(0));
				strcpy(data.name, utf_str.c_str());
			}
			
			if( column_names[j] == "Roletype" && value != "")
			{
				data.role_type = atoi(value);
				continue;
			}

			if( column_names[j] == "Broadcast" && value != "")
			{
				data.broadcast = atoi(value);
				continue;
			}

			if( column_names[j] == "MethodID" && value != "")
			{
				data.method_id = atoi(value);
				continue;
			}

			if( column_names[j] == "Coins" && value != "")
			{
				data.coins = atoi(value);
				continue;
			}

			if( column_names[j] == "NeedVitality" && value != "")
			{
				data.need_viatality = atoi(value);
				continue;
			}
			
			if( column_names[j] == "NeedSecondProLv" && value != "")
			{
				data.need_pro_lv = atoi(value);
				continue;
			}

			if( column_names[j] == "SecondProExp" && value != "")
			{
				data.add_pro_exp = atoi(value);
				continue;
			}

			if( column_names[j].Find("InItemID") != -1 && value != "")
			{
				input_item in;
				char buf[1024];
				strcpy(buf, value);
				sscanf(buf, "%u %u", &in.item_id, &in.item_count);
				data.ins.push_back(in);
				continue;
			}


			if( column_names[j].Find("OutItemID") != -1 && value != "")
			{
				output_item out;
				out.item_id = atoi(value);
				ss.ReadCell(value, j+1, i);
				out.odds = value;
				data.outs.push_back(out);
				continue;
			}
		}
		add_data(data);
	}
	return true;
}

bool method_mgr::final_to_xml(const char* xml)
{
	if(xml == NULL)return false;
	TiXmlDocument* pdoc = new TiXmlDocument;

	TiXmlElement* root = new TiXmlElement("Methods");
	pdoc->LinkEndChild(root);
	
	map<unsigned int, method_data>::iterator pItr = m_maps.begin();

	for(; pItr != m_maps.end(); ++pItr)
	{
		method_data* p_data= &pItr->second;
		TiXmlElement* Method = new TiXmlElement("Method");
		Method->SetAttribute("MethodID", p_data->method_id);
		Method->SetAttribute("Coins", p_data->coins);
		Method->SetAttribute("NeedVitality", p_data->need_viatality);
		Method->SetAttribute("NeedSecondProLv", p_data->need_pro_lv);
		Method->SetAttribute("SecondProExp", p_data->add_pro_exp);
		Method->SetAttribute("Name", p_data->name);
		Method->SetAttribute("RoleType", p_data->role_type);
		Method->SetAttribute("Broadcast", p_data->broadcast);
		Method->SetAttribute("Profession", 2);

		for(unsigned int i =0; i < p_data->ins.size(); i++)
		{
			input_item* in = &p_data->ins[i];
			TiXmlElement* InNode = new TiXmlElement("InItem");
			InNode->SetAttribute("ID", in->item_id);
			InNode->SetAttribute("Cnt", in->item_count);
			Method->InsertEndChild(*InNode);
		}

		for(unsigned int i = 0; i < p_data->outs.size(); i++)
		{
			output_item* out = &p_data->outs[i];
			TiXmlElement* OutNode = new TiXmlElement("OutItem");
			OutNode->SetAttribute("ID", out->item_id);
			OutNode->SetAttribute("Odds", out->odds.c_str());
			Method->InsertEndChild(*OutNode);
		}
		root->InsertEndChild(*Method);
	}
	return pdoc->SaveFile(xml);;
}

bool method_mgr::add_data(method_data& data)
{
	if(m_maps.find(data.method_id) == m_maps.end())
	{
		m_maps[data.method_id] = data;
		return true;
	}
	return false;
}