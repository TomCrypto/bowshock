#pragma once

/// @file
///
/// @brief Hardware abstraction layer for byte-oriented interfaces.

#include <rtl/waitable.hpp>
#include <rtl/functional.hpp>

namespace hal {

template <class T>
class byte_interface {
public:
  /// @brief Reads from the interface in the context of \c context. Returns a waitable.
  ///
  /// @remarks The context must be a functor taking a const reference to an \c rtl::u8 and returning the new status of
  ///          the waitable. The read operation will continue until the returned status is no longer pending.
  template <typename T2> auto read(T2 context) {
    return static_cast<T*>(this)->read(context);
  }

  /// @brief Writes to the interface in the context of \c context. Returns a waitable.
  ///
  /// @remarks The context must be a functor taking a reference to an \c rtl::u8 and returning the new status of the
  ///          waitable. The write operation will continue until the returned status is no longer pending. If the
  ///          returned status is \c pending, then the \c rtl::u8 passed will be written out.
  template <typename T2> auto write(T2 context) {
    return static_cast<T*>(this)->write(context);
  }
};

}
