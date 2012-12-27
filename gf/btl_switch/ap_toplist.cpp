#include "ap_toplist.hpp"

ap_toplist ap_list1;
ap_toplist ap_list2;
timer_head_t g_events;

bool ap_toplist::init(const char* file)
{
	char text[1024] = { 0 };
	FILE* fp = fopen(file, "rb");
	if(fp == NULL)return false;

	while(!feof(fp))
	{
		ap_data data;
		fgets(text, sizeof(text), fp);
		//米米号,角色号,玩家名字,成就点,最后更新时间，
		int ret = sscanf(text, "%u\t%u\t%s\t%u\t%u", 
							&data.userid_,
							&data.role_regtime_,
							data.nick_,
							&data.ap_point_,
							&data.get_time_
			   			);
		if(ret == 5)
		{
			//m_datas.push_back(data);
			add_ap_data(&data);
		}
	}
	fclose(fp);
	return true;
}

bool ap_toplist::final(const char* file)
{
	FILE* fp = fopen(file, "wb");
	if(fp == NULL)return false;

	std::list<ap_data>::iterator pItr = m_datas.begin();
	for( ; pItr != m_datas.end(); ++pItr)
	{
		ap_data* pdata= &(*pItr);
		fprintf(fp, "%u\t%u\t%s\t%u\t%u\n",
				    pdata->userid_,
					pdata->role_regtime_,
					pdata->nick_,
					pdata->ap_point_,
					pdata->get_time_
				);
	}
	fclose(fp);
	return true;
}

bool ap_toplist::add_ap_data(ap_data* pdata)
{
	if( m_datas.size() == 0){
		m_datas.push_back(*pdata);
		return true;
	}

	std::list<ap_data>::iterator pItr = m_datas.begin();
	for( ; pItr != m_datas.end(); ++pItr)
	{
		ap_data* p_old = &(*pItr);
		if( p_old->userid_ == pdata->userid_ && p_old->role_regtime_ == pdata->role_regtime_)
		{
			m_datas.erase(pItr);
			break;
		}
	}


	pItr = m_datas.begin();

	for( ; pItr != m_datas.end(); ++pItr)
	{
		ap_data* p_old = &(*pItr);
		if( pdata->compare(p_old) >= 0)
		{
			m_datas.insert(pItr, *pdata);
			if(m_datas.size() > AP_TOPLIST_COUNT)
			{
				m_datas.pop_back();
			}
			return true;
		}
	}

	m_datas.push_back(*pdata);
	if(m_datas.size() > AP_TOPLIST_COUNT)
	{
		m_datas.pop_back();
		return true;
	}
	return true;
}

void ap_toplist::get_ap_data(uint32_t begin_index, uint32_t end_index,  std::vector<ap_data>& datas)
{
	uint32_t i = begin_index;
	uint32_t j = begin_index;
	if(i <= m_datas.size())
	{
		std::list<ap_data>::iterator pItr = m_datas.begin();
		while(i > 1){
			++pItr;
			i--;
		}	

		std::list<ap_data>::iterator pItr2 = pItr;
		for(; pItr2 != m_datas.end() && j <= end_index; ++pItr2)
		{
			datas.push_back(*pItr2);
			j++;
		}
	}
}

int save_ap_toplist_data( void* owner, void* data)
{
	ap_list2.final("top_list.txt");
	ap_list1 = ap_list2;
	ADD_TIMER_EVENT(&g_events, save_ap_toplist_data,  reinterpret_cast<void*>(1), get_now_tv()->tv_sec + AP_SAVE_TIME);
	return 0;
}
