extern "C" {
#include <gameserv/game.h>

#include <ant/utils.h>
}

#include <cstdlib>
#include <ctime>

using namespace std;

#include "sledmap.hpp"

size_t Sled_map::m_len      = 800;  // default length of map
size_t Sled_map::m_nobarlen = 8;    // the final 8 tiles have no bars
size_t Sled_map::m_width    = 10;   // default width of map
// make sure srand is called once and only once
bool Sled_map::srand_called = ( srand(time(0)), true );

/**
 * generate_barries - generates barries for the sled map
 *  @min_interv: minimum interval between 2 barries
 *  @min_interv: maximum interval between 2 barries
 *
 */
void Sled_map::generate_barries(int min_interv, int max_interv)
{
    uint32_t x = 0, y, type;
	const size_t upbound = m_len - m_nobarlen;
    // randomly generates barries and their positions
    while ( (x += ranged_random(min_interv, max_interv)) < upbound ) {
    	// TODO - make every bit of y representing if a barrie is to be set at the given ypos in the xpos
        y    = rand() % m_width;
        type = rand() % nbartypes;
		m_barries[type].insert(x, y);
    }
	y    = rand() % m_width;
    type = rand() % nbartypes;
	m_barries[type].insert(upbound, y);
}

void Sled_map::send()
{
   for (size_t i = 0; i != nbartypes; ++i) {
      if ( m_barries[i].size() > 0 ) {
         m_barries[i].set_header();
		 protocol_t* buf = reinterpret_cast<protocol_t*>( &m_barries[i] );
		 send_to_group( m_gamegrp, reinterpret_cast<uint8_t*>(buf), ntohl(buf->len) );
      }
   }
   send_end_of_map();
}
