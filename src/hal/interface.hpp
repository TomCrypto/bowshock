#pragma once

/// @file
///
/// @brief Hardware abstraction layer for byte-oriented interfaces.

#include <rtl/waitable.hpp>

namespace hal {

template <class T>
class byte_interface {
public:
  /// @brief Reads \c size bytes from the interface into \c data.
  ///
  /// @remarks The \c data buffer must remain valid until the waitable has completed.
  rtl::waitable<typename T::waitable_type> read(std::size_t size, void* data);

  /// @brief Writes \c size bytes from \c data to the interface.
  ///
  /// @remarks The \c data buffer must remain valid until the waitable has completed. 
  rtl::waitable<typename T::waitable_type> write(const void* data, std::size_t size);
};

}
