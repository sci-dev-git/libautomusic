#ifndef UTIL_RANDOMIZE_H
#define UTIL_RANDOMIZE_H

#include <vector>
#include "typedefs.h"

namespace autocomp
{ namespace util
  {

#define PRAND_MAX 2147483647

void set_rand_seed(int32_t seed);
int32_t get_rand();

template <typename T>
  static inline T random_choice(const std::vector<T> &src)
    {
      std::size_t _rand_index = (std::size_t)( (src.size()-1) * ((float)get_rand()/PRAND_MAX) );
      return src[_rand_index];
    }

/*
 * @brief Choice the element in the container whose position proportion to the total number of elements is equal to factor.
 * @param factor Ranged from [0, 1]. Indicate the proportion of target element position.
 */
template <typename T>
  static inline T factor_choice(const std::vector<T> &src, double factor)
    {
      return src[(std::size_t)( (src.size()-1) * factor )];
    }

template <typename T>
  static inline T random_choice(const T *src, std::size_t size)
    {
      std::size_t _rand_index = (std::size_t)( (size-1) * ((float)get_rand()/PRAND_MAX) );
      return src[_rand_index];
    }

/**
 * @brief Generate a random number ranged from 0 to size-1.
 * This is often used to generate a index for a liner addressed container.
 */
static inline std::size_t random_range(std::size_t size)
  {
    return (std::size_t)( (size-1) * ((float)get_rand()/PRAND_MAX) );
  }

/**
 * @brief Generate a random number in the range of [start, end].
 */
template <typename T>
  static inline T random_range(T start, T end)
    {
      return start + (T)( (end-start) * ((float)get_rand()/PRAND_MAX) );
    }

  }
}

#endif
