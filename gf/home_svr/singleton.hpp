#ifndef SINGLETON_HPP
#define SINGLETON_HPP
template <typename T>
class singleton {
 public:
  typedef T object_type;

  static object_type & instance()
  {
    static object_type obj;
    create_object.do_nothing();
	return obj;
  }
		
 private:
  singleton();

  struct object_creator
  {
    object_creator()
	{
      singleton<T>::instance();
	}

	inline void do_nothing()const{}
  };

  static object_creator create_object;
};

template<typename T>
typename singleton<T>::object_creator 
singleton<T>::create_object;
#endif


