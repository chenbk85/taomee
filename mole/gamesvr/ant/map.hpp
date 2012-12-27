#ifndef ANT_MAP_HPP_
#define ANT_MAP_HPP_

#include <map>

namespace ant {

template <typename KeyT, typename ValT>
class Map {
private:
	// typedefs
	typedef std::map<KeyT, ValT> ImplType;
public:
	// typedefs
	typedef typename ImplType::size_type size_type;
	//
	bool add(const KeyT& key, const ValT& val);
private:
	std::map<KeyT, ValT> map_;
};

template <typename KeyT, typename ValT>
inline bool Map<KeyT, ValT>::
add(const KeyT& key, const ValT& val)
{
	std::pair<typename ImplType::iterator, bool> ret
				= map_.insert(ImplType::value_type(key, val));
	return ret.second;
}

}

#endif // ANT_MAP_HPP_
