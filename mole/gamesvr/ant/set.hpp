#ifndef ANT_SET_HPP_
#define ANT_SET_HPP_

#include <set>

namespace ant {

template <typename T>
class Set {
private:
	// typedefs
	typedef std::set<T> ImplType;
public:
	// typedefs
	typedef typename ImplType::size_type size_type;
	//
	bool add(const T& val);
	void del(const T& val)
		{ set_.erase(val); }
	//
	size_type count(const T& val) const
		{ return set_.count(val); }
	size_type size() const
		{ return set_.size(); }
private:
	//
	ImplType set_;
};

template <typename T>
inline bool Set<T>::
add(const T& val)
{
	std::pair<typename ImplType::iterator, bool> ret = set_.insert(val);
	return ret.second;
}

} // end of namespace ant

#endif // ANT_SET_HPP_
