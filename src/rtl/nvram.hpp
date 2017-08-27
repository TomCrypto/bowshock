#pragma once

/// @file
///
/// @brief Non-volatile memory definitions.
///
/// @warning This is not "true" non-volatile memory, this NVRAM simply refers to memory that is not cleared as part of
///          the runtime initialization. This allows reset information state to be preserved across software resets.

#include <rtl/platform.hpp>

namespace rtl
{

/// @brief Attribute to mark memory as part of the NVRAM section. Applications can define NVRAM variables.
///
/// @warning NVRAM variables **cannot** be default-initialized. You must initialize them on a hardware reset.
///
/// @warning The contents of NVRAM variables are preserved across *software resets*, that is, any reset triggered by
///          an assertion failure, a CPU fault, or any of the other RTL software reset functions. Any other type of
///          reset leaves such variables with unspecified contents.
#define nvram section(".nvram")

}
