#pragma once

#include <cassert>
/* #define MU_ASSERT(A) assert((A) "MovUtl assertion failed!  : " #A  __FILE__ __LINE__ ) */
#ifndef MU_ASSERT
#define MU_ASSERT(A) assert(A) 
#endif

#ifndef DISP
#define DISP(A) std::cout << #A << " = " << (A) << std::endl
#endif
