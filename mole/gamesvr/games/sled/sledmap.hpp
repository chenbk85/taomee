#ifndef ANDY_SLEDMAP_HPP
#define ANDY_SLEDMAP_HPP

// For better performance, this program is written in a very BAD style!
// I should have it written in C rather than C++!

extern "C" {
#include <netinet/in.h>

#include <gameserv/proto.h>
}

#include <cassert>
#include <algorithm>

#ifdef DEBUG
#include <iostream>
#endif // DEBUG

#include "cmdid.hpp"
#include "coordinate.hpp"

class BarriePkg {
public:
   BarriePkg() { m_len = 0; }
   BarriePkg(uint32_t type) { set_type(type); m_len = 0; }
   void set_type(uint32_t type) { m_type = htonl(type); }
   uint32_t size() const { return m_len; }
   inline void insert(uint32_t x, uint32_t y);
   inline void set_header();
private:
   static const uint32_t max_barries = 300;
   protocol_t m_header;
   uint32_t m_type;    // barrie type
   uint32_t m_len;	   // length of the Coordinate array
   Coordinate m_pos[max_barries]; // Array that holds coordinates of the barrie
#ifdef DEBUG
   friend std::ostream& operator<<(std::ostream& out, const BarriePkg& bar)
   {
      out << ntohl(bar.m_header.len) << ", " << (int)bar.m_header.ver << ", "
          << ntohl(bar.m_header.cmd) << ", " << ntohl(bar.m_header.id)
          << ", " << ntohl(bar.m_header.ret) << endl
          << "type: " << bar.m_type << ", length: " << bar.m_len << std::endl;
	  for ( uint32_t i = 0; i != bar.m_len; ++i )
		 out << bar.m_pos[i];

	  return out;
   }
#endif
} __attribute__((__packed__));

inline void BarriePkg::insert(uint32_t x, uint32_t y)
{
   assert(m_len < max_barries);
   m_pos[m_len++].set(htonl(x), htonl(y));
}

inline void BarriePkg::set_header()
{
	size_t pkg_size = sizeof(protocol_t) + 8 + sizeof(Coordinate) * m_len;
	init_proto_head(reinterpret_cast<uint8_t*>(&m_header), sled_map_info, pkg_size);
	m_len = htonl(m_len);
}

class Sled_map {
public:
   inline Sled_map(game_group_t* gamegrp);
   void generate_barries(int min_interv, int max_interv);
   void send(); // send sled map to the game group
   static void setmapsize(size_t len, size_t width) { m_len = len; m_width = width; }
private:
   // notify clients that all the map info is sent
   inline void send_end_of_map() const;

   static const size_t nbartypes = 6; // type of barrie
   static size_t m_len;     // length of sled map
   static size_t m_nobarlen;
   static size_t m_width;   // width of sled map
   game_group_t* m_gamegrp; // game group
   BarriePkg m_barries[nbartypes]; // Array of barrie package
   // make sure srand is called once and only once
   static bool srand_called;
#ifdef DEBUG
   friend std::ostream& operator<<(std::ostream& out, const Sled_map& bar)
   {
      for ( size_t i = 0; i != nbartypes; ++i ) {
         out << bar.m_barries[i];
      }

      return out;
   }
#endif
};

inline Sled_map::Sled_map(game_group_t* gamegrp)
{
   assert(gamegrp);
   m_gamegrp = gamegrp;
   for (size_t i = 0; i != nbartypes; ++i) {
      m_barries[i].set_type(i);
   }
}

inline void Sled_map::send_end_of_map() const
{
   const size_t pkg_size = sizeof(protocol_t);
   uint8_t pkg[pkg_size];
   // init protcol pkg
   init_proto_head(pkg, end_of_map, pkg_size);
   send_to_group( m_gamegrp, pkg, pkg_size );
}

#endif // ANDY_SLEDMAP_HPP
