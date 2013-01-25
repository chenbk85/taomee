#include "memorypool.h"

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

void* MemoryPool::GetMemory()
{
	if(m_freeNum > 0)
	{
		--m_freeNum;
	
		return &(m_pIntObjects[m_PagesNum*m_pfreeStack[m_freeNum]]);
	}
	else 
	{
		return NULL;
	}
}

void MemoryPool::FreeMemory( void* pObject)
{
	m_pfreeStack[m_freeNum++]= ((unsigned int*)pObject - m_pIntObjects)/m_PagesNum;
}


void MemoryPool::InitMemoryPool(unsigned int count,unsigned int objectSize)
{
	m_freeNum=count;
	m_totalNum=count;
	m_objectSize=objectSize;
	m_PagesNum=0;
	int last = m_objectSize%sizeof(unsigned int);
	if( last != 0)
	{
		m_PagesNum = m_objectSize/sizeof(unsigned int)+1;
	}
	else
	{
		m_PagesNum = m_objectSize/sizeof(unsigned int);
	}
	m_NewObjectSize = sizeof(unsigned int)*m_PagesNum;
	
	m_totalSize = count*m_NewObjectSize;

	m_pIntObjects = new unsigned int[count*m_PagesNum];

	m_pfreeStack = new unsigned int[count];
	for ( unsigned int i = 0; i < count; ++i)
	{
		m_pfreeStack[i] = i;
	}

}

MemoryPool::MemoryPool()
{
	m_freeNum = 0;
	m_PagesNum=0;
	m_NewObjectSize =0;
	m_totalNum = 0;
	m_objectSize = 0;
	m_totalSize = 0;

	m_pIntObjects = 0;
	m_pfreeStack = 0;
}

MemoryPool::~MemoryPool()
{
	if(m_pfreeStack)
	{
		delete [] m_pfreeStack;
	}
	if (m_pIntObjects)
	{
		delete [] m_pIntObjects;
	}
	m_freeNum = 0;
	m_PagesNum=0;
	m_NewObjectSize =0;
	m_totalNum = 0;
	m_objectSize = 0;
	m_totalSize = 0;
}

