#include "stdafx.h"
#include "item_attire.h"
#include "tinyxml.h"

#include "character_set.h"

bool init_xml_file(const char* xml_name, cat_items_mgr& mgr)
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
	TiXmlNode* CatItemElement = NULL;
	TiXmlNode* ItemElement = NULL;	

	root =doc.FirstChild("Items");
	if(root == NULL)return false;

	CatItemElement = root->FirstChild("Cat");
	if(CatItemElement == NULL)return false;

	while(CatItemElement)
	{
		cat_items  cat;
		CatItemElement->ToElement()->Attribute("ID", (int*)&(cat.cat_id));
		CatItemElement->ToElement()->Attribute("DbCatID", (int*)&(cat.db_cat_id));
		CatItemElement->ToElement()->Attribute("Max", (int*)&(cat.max));
		strcpy(cat.name,  CatItemElement->ToElement()->Attribute("Name"));
		
		ItemElement = CatItemElement->FirstChild("Item");
		if(ItemElement == NULL)return false;
		while(ItemElement)
		{
			item_attire_data data;
			ItemElement->ToElement()->Attribute("ID", (int*)&(data.id));
			strcpy(data.name, ItemElement->ToElement()->Attribute("Name"));
			ItemElement->ToElement()->Attribute("DropLv", (int*)&(data.droplv));
			ItemElement->ToElement()->Attribute("QualityLevel", (int*)&(data.quality_level));
			ItemElement->ToElement()->Attribute("EquipPart", (int*)&(data.equip_part));
			ItemElement->ToElement()->Attribute("Price", (int*)&(data.price));
			ItemElement->ToElement()->Attribute("SellPrice", (int*)&(data.sell_price));	
			ItemElement->ToElement()->Attribute("RepairPrice", (int*)&(data.repair_price));	
			ItemElement->ToElement()->Attribute("UseLv", (int*)&(data.uselv));
			ItemElement->ToElement()->Attribute("Strength", (int*)&(data.strength));
			ItemElement->ToElement()->Attribute("Agility", (int*)&(data.agility));
			ItemElement->ToElement()->Attribute("BodyQuality", (int*)&(data.body_quality));
			ItemElement->ToElement()->Attribute("Stamina", (int*)&(data.stamina));
			const char* p_atk = ItemElement->ToElement()->Attribute("Atk");
			if(p_atk != NULL)
			{
				strcpy(data.atk, p_atk);
			}
			ItemElement->ToElement()->Attribute("Def", (int*)&(data.def));
			ItemElement->ToElement()->Attribute("Duration", (int*)&(data.duration));
			ItemElement->ToElement()->Attribute("Hit", (int*)&(data.hit));
			ItemElement->ToElement()->Attribute("Dodge", (int*)&(data.dodge));
			ItemElement->ToElement()->Attribute("Crit", (int*)&(data.crit));
			ItemElement->ToElement()->Attribute("Hp", (int*)&(data.hp));
			ItemElement->ToElement()->Attribute("Mp", (int*)&(data.mp));
			ItemElement->ToElement()->Attribute("AddHp", (int*)&(data.add_hp));
			ItemElement->ToElement()->Attribute("AddMp", (int*)&(data.add_mp));
			ItemElement->ToElement()->Attribute("Slot",  (int*)&(data.slot));
			ItemElement->ToElement()->Attribute("Tradability",  (int*)&(data.trade_ability), 3);
			ItemElement->ToElement()->Attribute("VipTradability",  (int*)&(data.vip_trade_ability), 3);
			ItemElement->ToElement()->Attribute("Tradable",  (int*)&(data.trade_able));
			ItemElement->ToElement()->Attribute("ExploitValue",  (int*)&(data.exploit_value));
			ItemElement->ToElement()->Attribute("SetID",  (int*)&(data.setid));
			ItemElement->ToElement()->Attribute("honorLevel",  (int*)&(data.honor_level));
			ItemElement->ToElement()->Attribute("LifeTime",  (int*)&(data.life_time));
			ItemElement->ToElement()->Attribute("VipOnly",  (int*)&(data.vip_only));
			ItemElement->ToElement()->Attribute("DailyId",  (int*)&(data.dailyid));
			ItemElement->ToElement()->Attribute("decompose",  (int*)&(data.decompose));
			ItemElement->ToElement()->Attribute("Shop",  (int*)&(data.shop));
			ItemElement->ToElement()->Attribute("UnStorage",  (int*)&(data.un_storage), 1);
			ItemElement->ToElement()->Attribute("resID",  (int*)&(data.res_id));

			TiXmlNode* DescElement;
			DescElement = ItemElement->FirstChild("descript");
			TiXmlNode* pText = DescElement->FirstChild();
			strcpy(data.descipt, pText->Value());
			

			cat.add_item(data);
			ItemElement = ItemElement->NextSibling("Item");
		}
		

		mgr.add_cat_items(cat);
		CatItemElement = CatItemElement->NextSibling("Cat");
	}

	return true;
}
bool final_xml_file(const char* xml_name, cat_items_mgr& mgr)
{
	if(xml_name == NULL)return false;
	TiXmlDocument* pdoc = new TiXmlDocument;

	TiXmlElement* root = new TiXmlElement("Items");
	pdoc->LinkEndChild(root);
	
	std::map<int, cat_items>::iterator pItr = mgr.cat_items_map.begin();
	for(; pItr != mgr.cat_items_map.end(); ++pItr)
	{
		cat_items* p_cat = &(pItr->second);
		TiXmlElement* pCat = new TiXmlElement("Cat");
		pCat->SetAttribute("ID",  p_cat->cat_id);
		pCat->SetAttribute("DbCatID", p_cat->db_cat_id);
		pCat->SetAttribute("Name", p_cat->name);
		pCat->SetAttribute("Max", p_cat->max);
		
		map<int, item_attire_data>::iterator pItr2 = p_cat->item_map.begin();
		for(; pItr2 != p_cat->item_map.end(); ++pItr2)
		{
			item_attire_data* p_data= &(pItr2->second);
			TiXmlElement* pData = new TiXmlElement("Item");	
			pData->SetAttribute("ID", p_data->id);
			pData->SetAttribute("Name", p_data->name);
			pData->SetAttribute("DropLv", p_data->droplv);
			pData->SetAttribute("QualityLevel", p_data->quality_level);
			pData->SetAttribute("EquipPart", p_data->equip_part);
			pData->SetAttribute("Price", p_data->price);
			pData->SetAttribute("SellPrice", p_data->sell_price);
			pData->SetAttribute("RepairPrice", p_data->repair_price);
			pData->SetAttribute("UseLv", p_data->uselv);
			pData->SetAttribute("Strength", p_data->strength);
			pData->SetAttribute("Agility", p_data->agility);
			pData->SetAttribute("BodyQuality", p_data->body_quality);
			pData->SetAttribute("Stamina", p_data->stamina);
			if(strlen(p_data->atk) > 0)
			{
				pData->SetAttribute("Atk", p_data->atk);
			}
			pData->SetAttribute("Def", p_data->def);
			pData->SetAttribute("Duration", p_data->duration);
			pData->SetAttribute("Hit", p_data->hit);
			pData->SetAttribute("Dodge", p_data->dodge);
			pData->SetAttribute("Crit", p_data->crit);
			pData->SetAttribute("Hp", p_data->hp);
			pData->SetAttribute("Mp", p_data->mp);
			pData->SetAttribute("AddHp", p_data->add_hp);
			pData->SetAttribute("AddMp", p_data->add_mp);
			pData->SetAttribute("Slot",  p_data->slot);
			pData->SetAttribute("Tradability", p_data->trade_ability);
			pData->SetAttribute("VipTradability", p_data->vip_trade_ability);
			pData->SetAttribute("Tradable", p_data->trade_able);
			pData->SetAttribute("ExploitValue", p_data->exploit_value);
			pData->SetAttribute("SetID", p_data->setid);
			pData->SetAttribute("honorLevel", p_data->honor_level);
			pData->SetAttribute("LifeTime", p_data->life_time);
			pData->SetAttribute("VipOnly", p_data->vip_only);
			pData->SetAttribute("DailyId", p_data->dailyid);
			pData->SetAttribute("decompose", p_data->decompose);
			pData->SetAttribute("Shop", p_data->shop);
			pData->SetAttribute("UnStorage", p_data->un_storage);
			pData->SetAttribute("resID", p_data->res_id);
			if(strlen(p_data->descipt) > 0)
			{
				TiXmlElement* pdescipt = new TiXmlElement("descript");
				TiXmlText *pText = new TiXmlText(p_data->descipt);
				pText->SetCDATA(true);
				pdescipt->InsertEndChild(*pText);
				pData->InsertEndChild(*pdescipt);
			}
	
			pCat->InsertEndChild(*pData);	
		}
		root->InsertEndChild(*pCat);
	}
	return pdoc->SaveFile(xml_name);
}

bool init_excel_file(const char* excel_name, std::vector<item_attire_data>& items)
{
	char* names[] = {"บ๏", "อร", "ะÜ", "ม๚"};
	for(int i =0; i< sizeof(names)/sizeof(names[0]); i++)
	{
		std::map<int, CString> column_names;
		CSpreadSheet ss(excel_name, names[i]);
		for(int i = 1; i<= ss.GetTotalColumns(); i++)
		{
			CString name;
			ss.ReadCell(name, i, 1);
			column_names[i] = name;
		}

		
		for(int i = 2; i<= ss.GetTotalRows(); i++)
		{
			item_attire_data data;
			for(int j = 1; j< ss.GetTotalColumns(); j++)
			{
				CString value;

				ss.ReadCell(value, j, i);
				if( column_names[j] == "Item ID" && value != "")
				{
					data.id = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "Item ID", i);
				if( column_names[j] == "Name" && value != "")
				{		
					std::string utf_str = GBToUTF8(value.GetBuffer(0));
					//strcpy(data.name,  value.GetBuffer(0));	
					strcpy(data.name, utf_str.c_str());
					continue;
				}
				//ss.ReadCell(value, "Name", i);
				if( column_names[j] == "DropLv" && value != "")
				{
					data.droplv = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "DropLv", i);
				if( column_names[j] == "QualityLevel" && value != "")
				{
					data.quality_level = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "QualityLevel", i);
				if( column_names[j] == "EquipPart" && value != "")
				{
					data.equip_part = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "EquipPart", i);
				if( column_names[j] == "Price" && value != "")
				{
					data.price = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "Price", i);
				if( column_names[j] == "SellPrice" && value != "")
				{
					data.sell_price = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "SellPrice", i);
				if( column_names[j] == "RepairPrice" && value != "")
				{
					data.repair_price = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "RepairPrice", i);
				if( column_names[j] == "UseLv" && value != "")
				{
					data.uselv = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "UseLv", i);
				if( column_names[j] == "Strength" && value != "")
				{
					data.strength = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "Strength", i);
				if( column_names[j] == "BodyQuality" && value != "")
				{
					data.body_quality = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "BodyQuality", i);
				if( column_names[j] == "Stamina" && value != "")
				{
					data.stamina = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "Stamina", i);
				if( column_names[j] == "Atk" && value != "")
				{
					strcpy(data.atk, value.GetBuffer(0));
					continue;
				}
				//ss.ReadCell(value, "Atk", i);
				if( column_names[j] == "Def" && value != "")
				{
					data.def = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "Def", i);
				if( column_names[j] == "Duration" && value != "")
				{
					data.duration = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "Duration", i);
				if( column_names[j] == "Hit" && value != "")
				{
					data.hit = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "Hit", i);
				if( column_names[j] == "Dodge" && value != "")
				{
					data.dodge = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "Dodge", i);
				if( column_names[j] == "Crit" && value != "")
				{
					data.crit = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "Crit", i);
				if( column_names[j] == "Hp" && value != "")
				{
					data.hp = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "Hp", i);
				if( column_names[j] == "Mp" && value != "")
				{
					data.mp = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "Mp", i);
				if( column_names[j] == "AddHp" && value != "")
				{
					data.add_hp = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "AddHp", i);
				if( column_names[j] == "AddMp" && value != "")
				{
					data.add_mp = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "AddMp", i);
				if( column_names[j] == "Slot" && value != "")
				{
					data.slot = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "Slot", i);
				if( column_names[j] == "Tradability" && value != "")
				{
					data.trade_ability = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "Tradability", i);
				if( column_names[j] == "VipTradability" && value != "")
				{
					data.vip_trade_ability = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "VipTradability", i);
				if( column_names[j] == "Tradable" && value != "")
				{
					data.trade_able = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "Tradable", i);
				if( column_names[j] == "SetID" && value != "")
				{
					data.setid = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "SetID", i);
				if( column_names[j] == "DailyId" && value != "")
				{
					data.dailyid = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "DailyId", i);
				if( column_names[j] == "ExploitValue" && value != "")
				{
					data.exploit_value = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "ExploitValue", i);
				if( column_names[j] == "honorLevel" && value != "")
				{
					data.honor_level = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "honorLevel", i);
				if( column_names[j] == "VipOnly" && value != "")
				{
					data.vip_only = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "VipOnly", i);
				if( column_names[j] == "decompose" && value != "")
				{
					data.decompose = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "decompose", i);
				if( column_names[j] == "Shop" && value != "")
				{
					data.shop = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "Shop", i);
				if( column_names[j] == "UnStorage" && value != "")
				{
					data.un_storage = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "UnStorage", i);
				if( column_names[j] == "resID" && value != "")
				{
					data.res_id = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "resID", i);
				if( column_names[j] == "LifeTime" && value != "")
				{
					data.life_time = atoi(value);
					continue;
				}
				//ss.ReadCell(value, "LifeTime", i);
				if( column_names[j] == "descript" && value != "")
				{
					string utf_str = GBToUTF8( value.GetBuffer(0));
					strcpy(data.descipt, utf_str.c_str());
					continue;
				}
				//ss.ReadCell(value, "descript", i);
				continue;
				//
			}//for
			
			if(data.id == 0)
			{
				continue;
			}
			items.push_back(data);
		}

	}


	return true;
}


bool final_excel_file(const char* excel_name)
{
	return true;
}


bool merge_data(cat_items_mgr& dest, cat_items_mgr& mgr)
{
	std::map<int, cat_items>::iterator pItr = mgr.cat_items_map.begin();
	for(; pItr != mgr.cat_items_map.end(); ++pItr)
	{
		cat_items* p_cat = &(pItr->second);
		if( !dest.is_cat_id_exist(p_cat->cat_id))
		{
			dest.cat_items_map[p_cat->cat_id] = *p_cat;
		}
		else
		{
			cat_items* p_dest_cat = dest.get_cat_items_by_id(p_cat->cat_id);
			map<int, item_attire_data>::iterator pItr2 = p_cat->item_map.begin();
			for(; pItr2 != p_cat->item_map.end(); ++pItr2)
			{
				item_attire_data* pdata = &(pItr2->second);
				if(p_dest_cat->is_item_id_exist(pdata->id))
				{
					printf("item_id repeat cat_id = %d, item_id = %d", p_cat->cat_id, pdata->id);
					return false;
				}
				else
				{
					p_dest_cat->add_item(*pdata);
				}
			}
		}
	}

	return true;
}

bool merge_data(cat_items_mgr& dest, std::vector<item_attire_data>& items)
{
	for( unsigned int i=0;  i< items.size(); i++)
	{
		item_attire_data *pdata = &items[i];
		int cat_id = pdata->id / 100000;
		cat_items* p_cat_items = dest.get_cat_items_by_id(cat_id);
		if(p_cat_items == NULL)
		{
			printf("invalid item_id = %u\n", pdata->id);
			return false;
		}
		if(p_cat_items->is_item_id_exist(pdata->id))
		{
			printf("item_id = %u has exist\n", pdata->id);
			return false;
		}
		p_cat_items->add_item(*pdata);
	}
	return true;
}