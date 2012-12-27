#include <stdio.h>

#include <libtaomee++/conf_parser/xmlparser.hpp>
using namespace taomee;

static const int virtual_level_max_gf_calc   =600;
static const int level_max_gf_calc						=100;

static const int strength_pos_gf_calc 				=0;
static const int agility_pos_gf_calc  				=1;
static const int body_quality_pos_gf_calc    =2;
static const int stamina_pos_gf_calc   			=3;
static const int mp_pos_gf_calc  					  =4;
static const int hp_pos_gf_calc  					  =5;
static const int exp_pos_gf_calc      			  =6;
static const int atk_pos_gf_calc      			  =7;
static const int def_rate_pos_gf_calc				=8;

static const int max_attr_type_gf_calc  			=9;

static const int type_monkey		=0;
static const int type_rabbit		=1;
static const int type_panda		=2;

static const int max_role_type_gf_calc  			=3;

static double attribute_arr[virtual_level_max_gf_calc+1][max_attr_type_gf_calc] = {{0},{0}};
static char header_declare_arr[100*50]="";
static char header_fun_arr[max_role_type_gf_calc][max_attr_type_gf_calc][64];
//static char extern_pos_arr[(max_attr_type_gf_calc)][64];
//static char extern_role_type_arr[max_role_type_gf_calc][64];
static char declare_api_arr[64*100]="";

static char calc_strength_arr[64*100]="";
static char calc_agility_arr[64*100]="";
static char calc_body_quality_arr[64*100]="";
static char calc_stamina_arr[64*100]="";
static char calc_mp_arr[64*100]="";
static char calc_hp_arr[64*100]="";
static char calc_exp_arr[64*100]="";

static char calc_base_attr[max_role_type_gf_calc][max_attr_type_gf_calc][32]={{""},{""},{""}};

static char pro_body[256*10000]="";

int load_roles(xmlNodePtr cur)
{
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("role"))) {
      uint32_t level=0xFFFFFFFF;
      uint32_t liliang=0xFFFFFFFF;
      uint32_t minjie=0xFFFFFFFF;
      uint32_t tizhi=0xFFFFFFFF;
	  uint32_t qili=0xFFFFFFFF;
	  uint32_t zongqili=0xFFFFFFFF;
	  uint32_t zongxueliang=0xFFFFFFFF;
	  uint32_t jingyan=0xFFFFFFFF;
	  uint32_t zonggongjili=0xFFFFFFFF;
	  double   fangyulv=0;
           // char arr[10];
           // char name[10];
			
			get_xml_prop_def(level, cur, "level", 0,std::dec);
			get_xml_prop_def(liliang, cur, "liliang", 0);
			get_xml_prop_def(minjie, cur, "minjie", 0);
			get_xml_prop_def(tizhi, cur, "tizhi", 0);
			get_xml_prop_def(qili,cur,"qili",0);
			get_xml_prop_def(zongqili,cur,"zongqili",0);
			get_xml_prop_def(zongxueliang,cur,"zongxueliang",0);
			get_xml_prop_def(jingyan,cur,"jingyan",0);
			get_xml_prop_def(zonggongjili,cur,"zonggongjili",0);
			get_xml_prop_def(fangyulv,cur,"fangyulv",0);
			
			if (level == 0xFFFFFFFF)
			{
				cur = cur->next;
				continue;
			}
			attribute_arr[level][strength_pos_gf_calc] = (double)liliang;
			attribute_arr[level][agility_pos_gf_calc] = (double)minjie;
			attribute_arr[level][body_quality_pos_gf_calc] = (double)tizhi;
			attribute_arr[level][stamina_pos_gf_calc] = (double)qili;
			attribute_arr[level][mp_pos_gf_calc] = (double)zongqili;
			attribute_arr[level][hp_pos_gf_calc] = (double)zongxueliang;
			attribute_arr[level][exp_pos_gf_calc] = (double)jingyan;
			attribute_arr[level][atk_pos_gf_calc] = (double)zonggongjili;
			attribute_arr[level][def_rate_pos_gf_calc] = (double)fangyulv;			
			
		}
		cur = cur->next;
	}
	return 0;
}



//test ...
double calc_attr_roles(int level,int role_type,int attr_type)
{
	if (role_type==type_monkey)
		level += 0;
	if (role_type==type_rabbit)
		level += 200;
	if (role_type==type_panda)
		level += 400;
	level += 1;
	return attribute_arr[level][attr_type];
}
void out_calc_attr_roles(const char* str_role_type,const char* str_attr_type,int role_type,int attr_type)
{
	char szNote[64]="";
	
	sprintf(szNote,"%s_%s",str_attr_type,str_role_type);
	

	if (attr_type!=hp_pos_gf_calc && attr_type!=mp_pos_gf_calc)
	{
		sprintf(pro_body+strlen(pro_body),"/** \n");
		sprintf(pro_body+strlen(pro_body),"* @fn get %s of %s\n",str_attr_type,str_role_type);
		sprintf(pro_body+strlen(pro_body),"* @brief  API\n");
		sprintf(pro_body+strlen(pro_body),"*/\n");
		
		if (attr_type==def_rate_pos_gf_calc)
		{
			sprintf(pro_body+strlen(pro_body),"float calc_%s(int lv)\n",szNote);
			sprintf(pro_body+strlen(pro_body),"{\n");
			sprintf(pro_body+strlen(pro_body),"	static float %s_arr[] = {\n",szNote);
		}
		else
		{
			sprintf(pro_body+strlen(pro_body),"int calc_%s(int lv)\n",szNote);
			sprintf(pro_body+strlen(pro_body),"{\n");
			sprintf(pro_body+strlen(pro_body),"	static int %s_arr[] = {\n",szNote);
		}

		for (int i=0;i<level_max_gf_calc;i++)
		{
			if (i!=0 && i%10==0)
			{
				sprintf(pro_body+strlen(pro_body),"\n");
				sprintf(pro_body+strlen(pro_body),"			");
			}
			if (i==0)
			{
				sprintf(pro_body+strlen(pro_body),"			");
			}
				
			if (attr_type==def_rate_pos_gf_calc)
			{
				if (i==99)
					sprintf(pro_body+strlen(pro_body)," %f",calc_attr_roles(i,role_type,attr_type)/100);
				else
					sprintf(pro_body+strlen(pro_body)," %f,",calc_attr_roles(i,role_type,attr_type)/100);
			}
			else
			{
				if (i==99)
					sprintf(pro_body+strlen(pro_body)," %d",(int)calc_attr_roles(i,role_type,attr_type));
				else
					sprintf(pro_body+strlen(pro_body)," %d,",(int)calc_attr_roles(i,role_type,attr_type));
			}
		}
		sprintf(pro_body+strlen(pro_body),"\n	};\n");
		
		sprintf(pro_body+strlen(pro_body),"	if (lv >= 1 && lv <= %d) {\n",level_max_gf_calc);
		if (attr_type==def_rate_pos_gf_calc)
			sprintf(pro_body+strlen(pro_body),"		return (float)%s_arr[lv - 1];\n",szNote);
		else
			sprintf(pro_body+strlen(pro_body),"		return %s_arr[lv - 1];\n",szNote);
		sprintf(pro_body+strlen(pro_body),"	}\n");
		sprintf(pro_body+strlen(pro_body),"	return 0;\n");
		sprintf(pro_body+strlen(pro_body),"}\n");
	
	}
	//printf header file
	
	sprintf(header_declare_arr+strlen(header_declare_arr),"//get %s of %s\n",str_attr_type,str_role_type);
	if (attr_type == def_rate_pos_gf_calc)
		sprintf(header_declare_arr+strlen(header_declare_arr),"float calc_%s_%s(int lv);\n\n",str_attr_type,str_role_type);
	else
		sprintf(header_declare_arr+strlen(header_declare_arr),"int calc_%s_%s(int lv);\n\n",str_attr_type,str_role_type);
	
	
	if (attr_type==strength_pos_gf_calc)
	{
		sprintf(calc_strength_arr+strlen(calc_strength_arr),"	calc_%s,\n",szNote);
	}
	if (attr_type==agility_pos_gf_calc)
	{
		sprintf(calc_agility_arr+strlen(calc_agility_arr),"	calc_%s,\n",szNote);
	}
	if (attr_type==body_quality_pos_gf_calc)
	{
		sprintf(calc_body_quality_arr+strlen(calc_body_quality_arr),"	calc_%s,\n",szNote);
	}
	if (attr_type==stamina_pos_gf_calc)
	{
		sprintf(calc_stamina_arr+strlen(calc_stamina_arr),"	calc_%s,\n",szNote);
	}
	if (attr_type==mp_pos_gf_calc)
	{
		sprintf(calc_mp_arr+strlen(calc_mp_arr),"	calc_%s,\n",szNote);
	}
	if (attr_type==hp_pos_gf_calc)
	{
		sprintf(calc_hp_arr+strlen(calc_hp_arr),"	calc_%s,\n",szNote);
	}
	if (attr_type==exp_pos_gf_calc)
	{
		sprintf(calc_exp_arr+strlen(calc_exp_arr),"	calc_%s,\n",szNote);
	}

	//sprintf(header_fun_arr[role_type][attr_type],"calc_%s",szNote);

		

	if (role_type == type_monkey)
	{
		if (attr_type == def_rate_pos_gf_calc)
		{
			sprintf(declare_api_arr+strlen(declare_api_arr),"extern gf_calc_my_floatfun_def calc_%s[%d];\n",str_attr_type,max_role_type_gf_calc+1);
		}
		else
			sprintf(declare_api_arr+strlen(declare_api_arr),"extern gf_calc_my_fun_def calc_%s[%d];\n",str_attr_type,max_role_type_gf_calc+1);
	}
//	sprintf(extern_pos_arr[attr_type],"%s_pos_gf_calc,\n",str_attr_type);
//	sprintf(extern_role_type_arr[role_type],"type_%s_gf_calc,\n",str_role_type);
}

void out_pro_head()
{
	printf("#include \"player_attr.hpp\"\n\n\n\n");
		/*! base hit rates */
	printf("float hit_rates[] = { 0.0, 0.85, 0.8, 0.9 };\n");
	/*! adjustment for critical attack */
	printf("int crit_adjs[] = { 0, 2, 8, 0 };\n");


}

void sort_by_attr()
{
	printf("gf_calc_my_fun_def ");printf("calc_strength[max_role_type_gf_calc+1]={ \n");
	printf("	0,\n");
	calc_strength_arr[strlen(calc_strength_arr)-2]='\0';
	printf("%s\n",calc_strength_arr);
	printf("};\n");
	
	printf("gf_calc_my_fun_def ");printf("calc_agility[max_role_type_gf_calc+1]={ \n");
	printf("	0,\n");
	calc_agility_arr[strlen(calc_agility_arr)-2]='\0';
	printf("%s\n",calc_agility_arr);
	printf("};\n");
	
	printf("gf_calc_my_fun_def ");printf("calc_body_quality[max_role_type_gf_calc+1]={ \n");
	printf("	0,\n");
	calc_body_quality_arr[strlen(calc_body_quality_arr)-2]='\0';
	printf("%s\n",calc_body_quality_arr);
	printf("};\n");
	
	printf("gf_calc_my_fun_def ");printf("calc_stamina[max_role_type_gf_calc+1]={ \n");
	printf("	0,\n");
	calc_stamina_arr[strlen(calc_stamina_arr)-2]='\0';
	printf("%s\n",calc_stamina_arr);
	printf("};\n");
	
	printf("gf_calc_my_fun_def ");printf("calc_mp[max_role_type_gf_calc+1]={ \n");
	printf("	0,\n");
	calc_mp_arr[strlen(calc_mp_arr)-2]='\0';
	printf("%s\n",calc_mp_arr);
	printf("};\n");
	
	printf("gf_calc_my_fun_def ");printf("calc_hp[max_role_type_gf_calc+1]={ \n");
	printf("	0,\n");
	calc_hp_arr[strlen(calc_hp_arr)-2]='\0';
	printf("%s\n",calc_hp_arr);
	printf("};\n");
	
	printf("gf_calc_my_fun_def ");printf("calc_exp[max_role_type_gf_calc+1]={ \n");
	printf("	0,\n");
	calc_exp_arr[strlen(calc_exp_arr)-2]='\0';
	printf("%s\n",calc_exp_arr);
	printf("};\n");
	
}

void out_header()
{
	
	
	printf("\nstatic const int max_role_type_gf_calc=%d;\nstatic const int max_attr_type_gf_calc=%d;\n\n",max_role_type_gf_calc,max_attr_type_gf_calc);

	printf("static double base_attr[%d][%d]={\n",max_role_type_gf_calc,max_attr_type_gf_calc);
	for (int i=0;i<max_role_type_gf_calc;i++)
	{
		printf("{");
		for (int j=0;j<max_attr_type_gf_calc;j++)
		{
			if (i==(max_role_type_gf_calc-1) && j==(max_attr_type_gf_calc-1))
				printf(" %s}",calc_base_attr[i][j]);
			else
			{
				if (j==(max_attr_type_gf_calc-1))
					printf(" %s},",calc_base_attr[i][j]);
				else
					printf(" %s,",calc_base_attr[i][j]);
			}
		}
		printf("\n");
	}
	printf("};\n");


	
	
	
	
}

void out_cacl_hp_by_attr(const char* str_role_type,int role_type)
{
//	sprintf(declare_api_arr+strlen(declare_api_arr),"extern int calc_hp_%s(int body_quality);\n",str_role_type);

	printf("int calc_hp_%s(int body_quality)\n",str_role_type);
	printf("{\n");
	printf("	return %d + (body_quality - %d) * 25;\n",
		atoi(calc_base_attr[role_type][hp_pos_gf_calc]),
		atoi(calc_base_attr[role_type][body_quality_pos_gf_calc])
	);
	printf("}\n");
	
	
}

void out_cacl_mp_by_attr(const char* str_role_type,int role_type)
{

//	sprintf(declare_api_arr+strlen(declare_api_arr),"extern int calc_mp_%s(int stimina);\n",str_role_type);

	printf("int calc_mp_%s(int stimina)\n",str_role_type);
	printf("{\n");
	//mp = base_mp + (stimina - base_stimina) * 15
	printf("	return %d + (stimina - %d) * 15;\n",
		atoi(calc_base_attr[role_type][mp_pos_gf_calc]),
		atoi(calc_base_attr[role_type][stamina_pos_gf_calc])
	);
	printf("}\n");
}

void out_cacl_inline_by_attr()
{

	
	printf("// copy follow content to head of this file\n");
	printf(header_declare_arr);

	sort_by_attr();
	

	printf("/*\n");
	printf("// copy follow content to player_attr.hpp\n");
	printf("#ifndef GF_PLAYER_ATTR_HPP\n#define GF_PLAYER_ATTR_HPP\n\n\n");
	printf("typedef int (*gf_calc_my_fun_def)(int lv);\n");	
	printf("typedef float (*gf_calc_my_floatfun_def)(int lv);\n");	
	printf("\n// declare API array:\n");
	printf("%s\n",declare_api_arr);


	/*! base hit rates */
	printf("extern float hit_rates[];\n");
	/*! adjustment for critical attack */
	printf("extern int crit_adjs[];\n");

	
	printf("inline int calc_atk(int strength){\n");

	printf("	return (int)(strength * 0.7);\n");
	
	printf("}\n");


	//2:

	printf("inline float calc_def_rate(int body_quality){\n");
	
	printf("	return (body_quality / 2500.0);\n");
	
	printf("}\n");

	//3:

	printf("inline float calc_dodge_rate(int agility){\n");

	printf("	return (agility / 1250.0);\n");

	printf("}\n");

	printf("#endif\n");
	printf("*/\n");






}

void load_base_attr()
{
	for (int i=0;i<max_role_type_gf_calc;i++)
	{
		for (int j=0;j<max_attr_type_gf_calc;j++)
		{
			sprintf(calc_base_attr[i][j],"%f",calc_attr_roles(0,i,j));
		}
	}

}

int main()
{
    load_xmlconf("role_info_out.xml", load_roles);

	load_base_attr();
	
    out_pro_head();
    
    //计算力量 api
    out_calc_attr_roles("monkey","strength",type_monkey,strength_pos_gf_calc);
    out_calc_attr_roles("rabbit","strength",type_rabbit,strength_pos_gf_calc);
    out_calc_attr_roles("panda","strength",type_panda,strength_pos_gf_calc);

   //计算敏捷 api
    out_calc_attr_roles("monkey","agility",type_monkey,agility_pos_gf_calc);
    out_calc_attr_roles("rabbit","agility",type_rabbit,agility_pos_gf_calc);
    out_calc_attr_roles("panda","agility",type_panda,agility_pos_gf_calc);    
    
    //计算体质 api
    out_calc_attr_roles("monkey","body_quality",type_monkey,body_quality_pos_gf_calc);
    out_calc_attr_roles("rabbit","body_quality",type_rabbit,body_quality_pos_gf_calc);
    out_calc_attr_roles("panda","body_quality",type_panda,body_quality_pos_gf_calc);

 	//计算气力 api	
    out_calc_attr_roles("monkey","stamina",type_monkey,stamina_pos_gf_calc);
    out_calc_attr_roles("rabbit","stamina",type_rabbit,stamina_pos_gf_calc);
    out_calc_attr_roles("panda","stamina",type_panda,stamina_pos_gf_calc);

	//计算总气力 api
    out_calc_attr_roles("monkey","mp",type_monkey,mp_pos_gf_calc);
    out_calc_attr_roles("rabbit","mp",type_rabbit,mp_pos_gf_calc);
    out_calc_attr_roles("panda","mp",type_panda,mp_pos_gf_calc);

	//计算总血量 api
    out_calc_attr_roles("monkey","hp",type_monkey,hp_pos_gf_calc);
    out_calc_attr_roles("rabbit","hp",type_rabbit,hp_pos_gf_calc);
    out_calc_attr_roles("panda","hp",type_panda,hp_pos_gf_calc);

	//计算经验 api
    out_calc_attr_roles("monkey","exp",type_monkey,exp_pos_gf_calc);
    out_calc_attr_roles("rabbit","exp",type_rabbit,exp_pos_gf_calc);
    out_calc_attr_roles("panda","exp",type_panda,exp_pos_gf_calc);
    
  //计算攻击 api
//    out_calc_attr_roles("monkey","atk",type_monkey,atk_pos_gf_calc);
//    out_calc_attr_roles("rabbit","atk",type_rabbit,atk_pos_gf_calc);
 //   out_calc_attr_roles("panda","atk",type_panda,atk_pos_gf_calc);  	

  //计算防御率 api
 //   out_calc_attr_roles("monkey","def_rate",type_monkey,def_rate_pos_gf_calc);
 //   out_calc_attr_roles("rabbit","def_rate",type_rabbit,def_rate_pos_gf_calc);
 //   out_calc_attr_roles("panda","def_rate",type_panda,def_rate_pos_gf_calc);  

	out_header();
  
  	out_cacl_hp_by_attr("monkey",type_monkey);
	out_cacl_hp_by_attr("rabbit",type_rabbit);
	out_cacl_hp_by_attr("panda",type_panda);

	out_cacl_mp_by_attr("monkey",type_monkey);
	out_cacl_mp_by_attr("rabbit",type_rabbit);
	out_cacl_mp_by_attr("panda",type_panda);

	out_cacl_inline_by_attr();



	printf(pro_body);
    return 0;
}

