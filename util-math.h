#ifndef UTIL_MATH_H
#define UTIL_MATH_H

#include <cmath>

namespace autocomp
{ namespace util
  {

template <typename T>
  static inline T floor_mod(T op1, T op2)
    {
      return op1 - (op2 * static_cast<T>(std::floor(float(op1)/float(op2))));
    }

  }
}

#endif
