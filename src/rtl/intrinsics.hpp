#pragma once

/// @file
///
/// @brief Generic intrinsics.
///
/// The following intrinsics must be provided by the platform:
///  * \c disable_interrupts
///  * \c enable_interrupts
///  * \c wait_for_interrupt

#if defined(RTL_CORTEX_M0)
#include <rtl/cortex-m0/intrinsics.hpp>
#else
#error "No platform selected for the RTL."
#endif
