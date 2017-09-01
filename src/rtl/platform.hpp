#pragma once

//
// Defines compiler-level utilities.
//

#include <rtl/base.hpp>

#define section(x) __attribute__((section(x)))

#define STRINGIZE_HELPER(x) #x
#define STRINGIZE(x) STRINGIZE_HELPER(x)
