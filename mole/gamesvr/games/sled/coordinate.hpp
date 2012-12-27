#ifndef ANDY_COORDINATE_HPP
#define ANDY_COORDINATE_HPP

extern "C" {
#include <stdint.h>
}

#ifdef DEBUG
#include <iostream>
#endif
#include <vector>

class Coordinate {
public:
   Coordinate() { }
   Coordinate(uint32_t x, uint32_t y) { m_x = x; m_y = y; }
   void set(uint32_t x, uint32_t y) { m_x = x; m_y = y; }
private:
   uint32_t m_x, m_y;
#ifdef DEBUG
   friend std::ostream& operator<<(std::ostream& out, const Coordinate& coord)
   {
      out << coord.m_x << ", " << coord.m_y << std::endl;

      return out;
   }
#endif
} __attribute__((__packed__));

typedef std::vector<Coordinate> Coordinate_container;

#endif // ANDY_COORDINATE_HPP
