#ifndef ANDY_ASYM_AUTO_PTR_HPP
#define ANDY_ASYM_AUTO_PTR_HPP

extern "C" {
#include <stdint.h>
}

// The Asym_auto_ptr holds a pointer which points to a block of
// memory allocated as of type uint8_t, but assigned to it as of type T
template<typename T>
class Asym_auto_ptr {
public:
   Asym_auto_ptr(T* p = 0) throw() { m_ptr = p; }
   ~Asym_auto_ptr() throw() { destroy(); }
   void reset(T* p = 0) throw() { if ( p != m_ptr ) { destroy(); m_ptr = p; } }
   T* get() const throw() { return m_ptr; }
   T* operator->() const throw() { return m_ptr; }
   T& operator*() const throw() { return *m_ptr; }
private:
   void destroy() throw() { delete [] reinterpret_cast<uint8_t *>(m_ptr); }
   T* m_ptr;
};

#endif // ANDY_ASYM_AUTO_PTR_HPP
