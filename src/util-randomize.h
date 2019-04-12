/*
 *  libautomusic (Library for Image-based Algorithmic Musical Composition)
 *  Copyright (C) 2018, automusic.
 *
 *  THIS PROJECT IS FREE SOFTWARE; YOU CAN REDISTRIBUTE IT AND/OR
 *  MODIFY IT UNDER THE TERMS OF THE GNU LESSER GENERAL PUBLIC LICENSE(GPL)
 *  AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION; EITHER VERSION 2.1
 *  OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 *
 *  THIS PROJECT IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL,
 *  BUT WITHOUT ANY WARRANTY; WITHOUT EVEN THE IMPLIED WARRANTY OF
 *  MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  SEE THE GNU
 *  LESSER GENERAL PUBLIC LICENSE FOR MORE DETAILS.
 */
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
