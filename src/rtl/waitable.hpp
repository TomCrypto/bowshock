#pragma once

/// @file
///
/// @brief Waitable functionality.
///
/// The waitable object is a core concept representing an asynchronous operation.
///
/// There are different kinds of waitables, for instance *interrupt-driven* waitables are progressed in the context of
/// an interrupt handler, while *higher-order* waitables are driven by functions that depend on other waitables.
/// Ultimately all waitables **must** depend on some asynchronous event external to the processor.

#include <rtl/base.hpp>

namespace rtl
{

/// @remarks The default void specialization is only intended for easy access to static member helper functions.
template <typename T = void> struct waitable {
public:
  enum class status {
    pending,
    complete,
    failed
  };

  auto is_complete() const {
    return static_cast<T*>(this)->is_complete();
  }

  auto is_failed() const {
    return static_cast<T*>(this)->is_failed();
  }

  auto is_pending() const {
    return static_cast<T*>(this)->is_pending();
  }

  void wait() const {
    waitable::wait_all(*this);
  }

  /// @brief Waits for all of the waitables passed to complete or fail.
  template <typename... waitables> static void wait_all(waitables... list) {
    while (!wait_all_inner(std::forward<waitables>(list)...)) { __WFI(); }
  }

  /// @brief Waits for any of the waitables passed to complete or fail.
  template <typename... waitables> static void wait_any(waitables... list) {
    while (!wait_any_inner(std::forward<waitables>(list)...)) { __WFI(); }
  }

private:
  template <typename T2, template <typename> class waitable>
  static auto wait_all_inner(const waitable<T2>& head) {
    return !head.is_pending();
  }

  template <typename T2, template <typename> class waitable, typename... waitables>
  static auto wait_all_inner(const waitable<T2>& head, waitables... tail) {
    return !head.is_pending() && wait_all_inner(std::forward<waitables>(tail)...);
  }

  template <typename T2, template <typename> class waitable>
  static auto wait_any_inner(const waitable<T2>& head) {
    return !head.is_pending();
  }

  template <typename T2, template <typename> class waitable, typename... waitables>
  static auto wait_any_inner(const waitable<T2>& head, waitables... tail) {
    return !head.is_pending() || wait_any_inner(std::forward<waitables>(tail)...);
  }
};

}
