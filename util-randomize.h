#ifndef UTIL_RANDOMIZE_H
#define UTIL_RANDOMIZE_H

#include <vector>
#include <cstdlib>

namespace autocomp
{ namespace util
  {

template <typename T>
  static inline T random_choice(const std::vector<T> &src)
    {
      std::size_t _rand_index = (std::size_t)( (src.size()-1) * ((float)std::rand()/RAND_MAX) );
      return src[_rand_index];
    }

template <typename T>
  static inline T random_choice(const T *src, std::size_t size)
    {
      std::size_t _rand_index = (std::size_t)( (size-1) * ((float)std::rand()/RAND_MAX) );
      return src[_rand_index];
    }

static inline std::size_t random_range(std::size_t size)
  {
    return (std::size_t)( (size-1) * ((float)std::rand()/RAND_MAX) );
  }

  }
}

#endif
