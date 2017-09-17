#pragma once

/// @file
///
/// @brief Rational arithmetic support.

#if defined(RTL_CORTEX_M0)
#include <rtl/cortex-m0/math/rational.hpp>
#else
#error "No platform selected for the RTL."
#endif
