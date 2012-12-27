#include <cstring>
#include <stdint.h>
#include <stdio.h>
#include "memstream.hpp"
#include <pdumanip.hpp>

using namespace taomee;

mem_stream::mem_stream(uint32_t max_len)
{
	max_buf_len = max_len;
	buf = new char[max_buf_len];
	memset(buf, 0, max_buf_len);
	read_len = 0;
	write_len = 0;
}

mem_stream::~mem_stream()
{
	delete [] buf;
	buf = NULL;
	read_len = 0;
	write_len = 0;
}

void mem_stream::clear()
{
	read_len = 0;
	write_len = 0;
}

uint32_t mem_stream::get_max_buf_len()
{
	return max_buf_len;
}

uint32_t mem_stream::get_cur_buf_len()
{
	return write_len;	
}
const char* mem_stream::get_buf()
{
	return buf;
}

bool mem_stream::seek(int offset)
{
	int cur_pos = offset + write_len;
	if(  cur_pos >= 0 && cur_pos <= max_buf_len)
	{
		write_len = cur_pos;
		return true;
	}
	return false;
}

void mem_stream::init(const char* mem, uint32_t mem_len)
{
	if(mem_len > max_buf_len)return;
	memcpy(buf, mem, mem_len);
	read_len = 0;
	write_len = mem_len;
}

void mem_stream::init(uint8_t *mem, uint32_t mem_len)
{
	if(mem_len > max_buf_len)return;
	memcpy(buf, mem, mem_len);
	read_len = 0;
	write_len = mem_len;
}


bool mem_stream::put_uint32(uint32_t value)
{
	if( write_len + sizeof(value) > max_buf_len)return false;
	*(uint32_t*)(buf+write_len) = bswap(value);
	write_len += sizeof(value);
	return true;
}

bool mem_stream::put_uint32_h(uint32_t value)
{
	if( write_len + sizeof(value) > max_buf_len)return false;
	*(uint32_t*)(buf+write_len) = value;
	write_len += sizeof(value);
	return true;
}


bool mem_stream::put_uint16(uint16_t value)
{
	if( write_len + sizeof(value) > max_buf_len)return false;
	*(uint16_t*)(buf+write_len) = bswap(value);
	write_len += sizeof(value);
	return true;
}

bool mem_stream::put_uint16_h(uint16_t value)
{
	if( write_len + sizeof(value) > max_buf_len)return false;
	*(uint16_t*)(buf+write_len) = value;
	write_len += sizeof(value);
	return true;
} 


bool mem_stream::put_uint8(uint8_t value)
{
	if( write_len + sizeof(value) > max_buf_len)return false;
	*(uint8_t*)(buf+write_len) = bswap(value);
	write_len += sizeof(value);
	return true;
}

bool mem_stream::put_uint8_h(uint8_t value)                                                                   
{                                                                                                           
	if( write_len + sizeof(value) > max_buf_len)return false;                                               
	*(uint8_t*)(buf+write_len) = value;                                                              
	write_len += sizeof(value);                                                                             
	return true;                                                                                            
}


bool mem_stream::put_mem(uint8_t *mem, uint32_t mem_len)
{
	if( write_len + mem_len > max_buf_len)return false;
	memcpy( buf+ write_len, mem, mem_len);
	write_len += mem_len;
	return true;
}

bool mem_stream::get_uint32(uint32_t* value)
{
	if(read_len + sizeof(uint32_t) > write_len)return false;
	*value = bswap(   *(uint32_t*)(buf+read_len));
	read_len += sizeof(uint32_t);
	return true;
}

bool mem_stream::get_uint32_h(uint32_t* value)                                                                
{                                                                                                           
	if(read_len + sizeof(uint32_t) > write_len)return false;                                                
	*value = (   *(uint32_t*)(buf+read_len));                                                          
	read_len += sizeof(uint32_t);                                                                           
	return true;                                                                                            
}

bool mem_stream::get_uint16(uint16_t* value)
{
	if(read_len + sizeof(uint16_t) > write_len)return false;    
	*value = bswap(   *(uint16_t*)(buf+read_len));
	read_len += sizeof(uint16_t);
	return true;
}

bool mem_stream::get_uint16_h(uint16_t* value)                                                                
{                                                                                                           
	if(read_len + sizeof(uint16_t) > write_len)return false;                                                
	*value = (   *(uint16_t*)(buf+read_len));                                                          
	read_len += sizeof(uint16_t);                                                                           
	return true;                                                                                            
}

bool mem_stream::get_uint8(uint8_t* value)
{
	if(read_len + sizeof(uint8_t) > write_len)return false;	
	*value = bswap(   *(uint8_t*)(buf+read_len));
	read_len += sizeof(uint8_t);
	return true;
}


bool mem_stream::get_uint8_h(uint8_t* value)                                                                  
{                                                                                                           
	if(read_len + sizeof(uint8_t) > write_len)return false;                                                 
	*value = (   *(uint8_t*)(buf+read_len));                                                           
	read_len += sizeof(uint8_t);                                                                            
	return true;                                                                                            
}   


bool mem_stream::get_mem(uint8_t* mem, uint32_t mem_len)
{
	if(read_len + mem_len > write_len )return false;
	memcpy(mem, buf+read_len, mem_len);
	read_len += mem_len;
	return true;
}








