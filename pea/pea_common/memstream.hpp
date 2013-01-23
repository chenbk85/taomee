#ifndef MEM_STREAM_H_
#define MEM_STREAM_H_


class mem_stream
{
public:
	mem_stream(uint32_t max_len);
	~mem_stream();
public:
	void init(const char* mem, uint32_t mem_len);
	void init(uint8_t *mem, uint32_t mem_len);
	void clear();
	uint32_t get_max_buf_len();
	uint32_t get_cur_buf_len();
	const char* get_buf();
	bool seek(int offset);	
public:
	bool put_uint32(uint32_t value);
	bool put_uint16(uint16_t value);
	bool put_uint8(uint8_t value);
	bool put_mem(uint8_t *mem, uint32_t mem_len);	

	bool get_uint32(uint32_t* value);
	bool get_uint16(uint16_t* value);
	bool get_uint8(uint8_t* value);
	bool get_mem(uint8_t* mem, uint32_t mem_len);
public:
	bool put_uint32_h(uint32_t value);
	bool put_uint16_h(uint16_t value);
	bool put_uint8_h(uint8_t value);

	bool get_uint32_h(uint32_t* value);
	bool get_uint16_h(uint16_t* value);
	bool get_uint8_h(uint8_t* value);


private:
	char* 		buf;
	uint32_t 	max_buf_len;
	uint32_t    write_len;
	uint32_t    read_len;	
};



#endif
