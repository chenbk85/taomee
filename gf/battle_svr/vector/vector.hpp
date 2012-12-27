#ifndef LIBTAOMEEPP_VECTOR_HPP_
#define LIBTAOMEEPP_VECTOR_HPP_

/**
 *============================================================
 *  @file      vector.hpp
 *  @brief    vector to identify an object's position
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#include <cmath>
#include <cstdio>
#include <limits>
#include <string>

/**
  * @brief 3d vector
  */
class Vector3D
{
public:
	/**
	  * @brief default constructor
	  */
	Vector3D(int x = 0, int y = 0, int z = 0)
		{ init(x, y, z); }

	Vector3D(const Vector3D& rhs) 
	{
		init(rhs.x(), rhs.y(), rhs.z());
	}
	/**
	  * @brief set x, y and z
	  */
	void init(int x = 0, int y = 0, int z = 0)
		{ set_x(x); set_y(y); set_z(z); }
	/**
	  * @brief set x
	  */
	void set_x(int x = 0)
		{ x_ = x; }
	/**
	  * @brief set y
	  */
	void set_y(int y = 0)
		{ y_ = y; }
	/**
	  * @brief set z
	  */
	void set_z(int z = 0)
		{ z_ = z; }

	/**
	  * @brief normalizes a vector
	  */
	void normalize();

	/**
	  * @brief truncates a vector so that its length does not exceed max
	  */
	void truncate(int max);

	/**
	  * @brief given a normalized vector this method reflects the vector it is operating upon. (like the path of a ball bouncing off a wall)
	  */
	void reflect(const Vector3D& norm)
		{ *this += 2.0 * this->dot_product(norm) * norm.get_reverse(); }

	/**
	  * @brief return x position of a vector
	  */
	int x() const
		{ return x_; }
	/**
	  * @brief return y position of a vector
	  */
	int y() const
		{ return y_; }
	/**
	  * @brief return z position of a vector
	  */
	int z() const
		{ return z_; }

	/**
	  * @brief returns the length of the vector
	  */
	int length() const
		{ return static_cast<int>(sqrt(squared_length())); }
	/**
	  * @brief returns the squared length of the vector (thereby avoiding the sqrt)
	  */
	int squared_length() const
		{ return (x_ * x_ + y_ * y_ + z_ * z_); }

	/**
	  * @brief returns the vector that is the reverse of this vector
	  */
	Vector3D get_reverse() const
		{ return Vector3D(-this->x_, -this->y_, -this->z_);	}

	/**
	  * @brief returns a normalize version of the given vector 'v'
	  */
	Vector3D get_normalize() const
		{ Vector3D vec(*this); vec.normalize(); return vec; }

	/**
	  * @brief returns the distance between this vector and the one passed as a parameter
	  */
	int distance(const Vector3D& v2) const;
	/**
	  * @brief squared version of above.
	  */
	int squared_distance(const Vector3D& v2) const;

	// TODO: might be bugs
	/**
	  * @brief returns true if this vector is inside the region defined by top_left and bot_rgt (bottom right)
	  */
	bool inside_region(Vector3D top_left, Vector3D bot_rgt) const
		{ return !((x_ < top_left.x_) || (x_ > bot_rgt.x_) || (y_ < top_left.y_) || (y_ > bot_rgt.y_) || (z_ < bot_rgt.z_) || (z_ > top_left.z_)); }

	/**
	  * @brief calculates the dot product
	  */
	int dot_product(const Vector3D& v2) const
		{ return x_ * v2.x_ + y_ * v2.y_ + z_ * v2.z_; }

	// TODO: support only 2D vector
	/**
	  * @brief returns positive if v2 is clockwise of this vector, negative if anticlockwise
	  *           (assuming the Y axis is pointing down, X axis to right)
	  */
	int calc_clockwise(const Vector3D& v2) const
		{ return (y_ * v2.x_ > x_ * v2.y_) ? -1 : 1; }
	// TODO: 2D only
	/**
	  * @brief Returns a vector perpendicular to this vector
	  */
	Vector3D perpendicular() const
		{ return Vector3D(-y_, x_); }

	/**
	  * @brief returns true if x, y and z are zero
	  */
	bool is_zero() const
		{ return !(x_ || y_ || z_); }

	std::string to_string() const
		{ static char buf[100]; snprintf(buf, sizeof(buf), "(%f, %f, %f)", x_, y_, z_); return std::string(buf); }

public:
	// Operator Overloads
	const Vector3D& operator+=(const Vector3D& rhs)
		{ x_ += rhs.x_; y_ += rhs.y_; z_ += rhs.z_; return *this; }

	const Vector3D& operator-=(const Vector3D& rhs)
		{ x_ -= rhs.x_; y_ -= rhs.y_; z_ -= rhs.z_; return *this; }

	const Vector3D& operator*=(int rhs)
		{ x_ *= rhs; y_ *= rhs; z_ *= rhs; return *this; }

	const Vector3D& operator/=(int rhs)
		{ x_ /= rhs; y_ /= rhs; z_ /= rhs; return *this; }

	bool operator==(const Vector3D& rhs) const
		{ return (x_ == rhs.x_) && (y_ == rhs.y_) && (z_ == rhs.z_); }

	bool operator!=(const Vector3D& rhs) const
		{ return !(*this == rhs); }

	friend Vector3D operator+(const Vector3D& lhs, const Vector3D& rhs);
	friend Vector3D operator-(const Vector3D& lhs, const Vector3D& rhs);
	friend Vector3D operator*(const Vector3D& lhs, int rhs);
	friend Vector3D operator*(int lhs, const Vector3D& rhs);
	friend Vector3D operator/(const Vector3D& lhs, int val);

private:
	double		x_;
	double		y_;
	double		z_;
};

//------------------------------------------------------------------------
// Public Methods
inline
void Vector3D::normalize()
{ 
	int vector_length = length();

	if (vector_length > 0) {
		x_ /= vector_length;
		y_ /= vector_length;
		z_ /= vector_length;
	}
}

inline
void Vector3D::truncate(int max)
{
	if (length() > max) {
		normalize();
		*this *= max;
	} 
}

inline
int Vector3D::distance(const Vector3D& v2) const
{
	return static_cast<int>(sqrt(squared_distance(v2)));
}

inline
int Vector3D::squared_distance(const Vector3D& v2) const
{
	double x = v2.x_ - x_;
	double y = v2.y_ - y_;
	double z = v2.z_ - z_;

	return x * x + y * y + z * z;
}

//------------------------------------------------------------------------
// operator overloads

//overload the + operator
inline
Vector3D operator+(const Vector3D& lhs, const Vector3D& rhs)
{
	Vector3D result(lhs);
	result += rhs;
	return result;
}

//overload the - operator
inline
Vector3D operator-(const Vector3D& lhs, const Vector3D& rhs)
{
	Vector3D result(lhs);
	result -= rhs;
	return result;
}

inline
Vector3D operator*(const Vector3D& lhs, int rhs)
{
	Vector3D result(lhs);
	result *= rhs;
	return result;
}

inline
Vector3D operator*(int lhs, const Vector3D& rhs)
{
	Vector3D result(rhs);
	result *= lhs;
	return result;
}

//overload the / operator
inline
Vector3D operator/(const Vector3D& lhs, int val)
{
	Vector3D result(lhs);
	result /= val;
	return result;
}

#endif // LIBTAOMEEPP_VECTOR_HPP_

