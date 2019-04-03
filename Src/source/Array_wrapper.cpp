/*
 * Array_wrapper.cpp
 *
 *  Created on: 2019/04/03
 *      Author: shun2
 */


#include <initializer_list>
#include <algorithm>
#include <type_traits>
#include <cassert>
#include <stdint.h>
#include "Array_wrapper.hpp"

template<class V, class IX>
class array_wrapper {
private:
  static_assert(std::is_enum<IX>::value, "Index class is not enum type.");
  size_t size;
public:
  typedef V  value_type;
  typedef IX index_type;
  typedef typename std::underlying_type<IX>::type underlying_type;
  value_type *raw_array;

  array_wrapper(std::initializer_list<V> il)
    : size (il.size()), raw_array (new V[size]) {
    std::copy(il.begin(), il.end(), raw_array);
  }

  value_type& operator[](const index_type index) const {
    underlying_type index_number = static_cast<underlying_type>(index);
    assert(index_number < size);
    return raw_array[index_number];
  }

  ~array_wrapper() {
    delete[] raw_array;
  }
};
