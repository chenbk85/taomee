#ifndef GAMESERV_COORDINATE_HPP_
#define GAMESERV_COORDINATE_HPP_

#include <iostream>

namespace ant {

template <typename T>
class Coordinate2D {
public:
	Coordinate2D(const T& x = 0, const T& y = 0)
		{ set(x, y); }
	void set(const T& x, const T& y)
		{ x_ = x; y_ = y; }
	const T& x() const
		{ return x_; }
	const T& y() const
		{ return y_; }
	// Overloading Operators
	bool operator==(const Coordinate2D& coord) const
		{ return ((x_ == coord.x_) && (y_ == coord.y_)); }
	bool operator<(const Coordinate2D& coord) const
		{ return ((x_ < coord.x_) || ((x_ == coord.x_) && (y_ < coord.y_))); }
	friend std::ostream& operator<<(std::ostream& out, const Coordinate2D& coord)
		{ out << "(" << coord.x_ << ", " << coord.y_ << ")"; return out; }
private:
	T x_, y_;
};

} // end of namespace ant

#endif // GAMESERV_COORDINATE_HPP_
