#pragma once

/// @file
///
/// @brief Assert functionality.
///
/// @remarks All RTL assert functionality can be disabled by defining the \c NO_ASSERT macro.

#include <rtl/base.hpp>
#include <rtl/platform.hpp>
#include <rtl/nvram.hpp>

namespace rtl
{

#if defined(NO_ASSERT)
#define TRACE(what) nullptr
void assert(bool condition, const char* message) {}
#else

/// @brief Decorates a string literal with the file and line where the macro is invoked.
#define TRACE(what) (__FILE__ ":" STRINGIZE(__LINE__) ": " what)

/// @brief Issues an assert condition and terminates execution if the condition is false.
inline auto assert(bool condition, const char* message) {
  extern nvram const char* assert_message;

  if (!condition) {
    assert_message = message;
    abort();
  }
}

#endif

}
