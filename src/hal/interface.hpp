#pragma once

/// @file
///
/// @brief Hardware abstraction layer for byte-oriented interfaces.

#include <rtl/waitable.hpp>
#include <rtl/buffer.hpp>

namespace hal {

template <class T>
class byte_interface {
public:
  /// @brief Reads \c size bytes from the interface into \c data.
  ///
  /// @remarks The \c data buffer must remain valid until the waitable has completed.
  auto read(rtl::buffer buffer) {
    return static_cast<T*>(this)->read(buffer);
  }

  /// @brief Writes \c size bytes from \c data to the interface.
  ///
  /// @remarks The \c data buffer must remain valid until the waitable has completed. 
  auto write(rtl::buffer buffer) {
    return static_cast<T*>(this)->write(buffer);
  }
};

}
