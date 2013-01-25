
#ifndef MY_MEMORY_POOL
#define MY_MEMORY_POOL

class MemoryPool
{
public:
	MemoryPool();
	~MemoryPool();
private:
	unsigned int m_NewObjectSize;

	unsigned int m_freeNum;
	unsigned int m_totalNum;
	unsigned int m_objectSize;

	unsigned int m_totalSize;
	
	unsigned int* m_pfreeStack;
	unsigned int* m_pIntObjects;

	int		m_PagesNum;
	
public:
	unsigned int GetTotalNum()
	{
		return m_totalNum;
	};
	unsigned int GetBufferSize()
	{
		return m_objectSize;
	};

	void* GetMemory();
	void FreeMemory( void* pObject);

	
	void InitMemoryPool(unsigned int count,unsigned int objectSize);
};

#endif