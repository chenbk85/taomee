// excel_to_xml.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "excel_to_xml.h"
#include "item_attire.h"
#include "compose.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		/*
		if(argc != 4)
		{
			printf("invalid parameters\n");
			printf("-----example-------\n");
			printf("excel_to_xml.exe 1.xml 2.xls 3.xml");
			printf("-------------------\n");
			return 0;
		}
		char *p1 = argv[1];
		char *p2 = argv[2];
		char *p3 = argv[3];
 		
		cat_items_mgr mgr;
		std::vector<item_attire_data> items;
		init_xml_file(p1, mgr);
		init_excel_file(p2, items);
		bool ret = merge_data(mgr, items);
		if(ret)
		{
			final_xml_file(p3, mgr);
			printf("---convert ok----\n");
		}
		else
		{
			printf("---convert fail----\n");
		}*/
		method_mgr mgr;
		mgr.init_from_excel("compose.xls");
		mgr.final_to_xml("compose.xml");
	}

	return nRetCode;
}
