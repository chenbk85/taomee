#ifndef st_h__
#define st_h__

struct st_ShowOutEdit
{
	char* pszMsg;
	unsigned long nMsgLen;
	char* pszData;
	unsigned long nDataLen;
	st_ShowOutEdit()
	{
		Init();
	}
	void Init()
	{
		Init(NULL,0,NULL,0);
	}
	void Init(char* pszMsg,unsigned long nMsgLen,char* pszData,unsigned long nDataLen)
	{
		this->pszMsg = pszMsg;
		this->nMsgLen = nMsgLen;
		this->pszData = pszData;
		this->nDataLen = nDataLen;
	}
};

#endif // st_h__