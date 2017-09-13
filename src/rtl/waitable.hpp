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
  // TODO: move this elsewhere (e.g. rtl::wait_state)
  enum class status {
    pending,
    complete,
    failed
  };

  /// @brief Returns whether this waitable is complete.
  auto is_complete() const {
    return static_cast<const T*>(this)->is_complete();
  }

  /// @brief Returns whether this waitable is failed.
  auto is_failed() const {
    return static_cast<const T*>(this)->is_failed();
  }

  /// @brief Returns whether this waitable is pending.
  auto is_pending() const {
    return static_cast<const T*>(this)->is_pending();
  }

  /// @brief Waits for this waitable to complete or fail.
  auto wait() const {
    waitable::wait_all(*static_cast<const T*>(this));
  }

  // TODO: replace wfi with the correct "wait for interrupt/event" intrinsic for the platform
  // e.g. rtl::wait or rtl::pause

  /// @brief Waits for all of the waitables passed to complete or fail.
  template <typename... waitables> static auto wait_all(waitables&&... list) {
    while (!wait_all_inner(std::forward<waitables>(list)...)) { __asm__("wfi"); }
  }

  /// @brief Waits for any of the waitables passed to complete or fail.
  template <typename... waitables> static auto wait_any(waitables&&... list) {
    while (!wait_any_inner(std::forward<waitables>(list)...)) { __asm__("wfi"); }
  }

private:
  template <typename T2, template <typename> class waitable>
  static auto wait_all_inner(const waitable<T2>& head) {
    return !head.is_pending();
  }

  template <typename T2, template <typename> class waitable, typename... waitables>
  static auto wait_all_inner(const waitable<T2>& head, waitables&&... tail) {
    return !head.is_pending() && wait_all_inner(std::forward<waitables>(tail)...);
  }

  template <typename T2, template <typename> class waitable>
  static auto wait_any_inner(const waitable<T2>& head) {
    return !head.is_pending();
  }

  template <typename T2, template <typename> class waitable, typename... waitables>
  static auto wait_any_inner(const waitable<T2>& head, waitables&&... tail) {
    return !head.is_pending() || wait_any_inner(std::forward<waitables>(tail)...);
  }
};

}
