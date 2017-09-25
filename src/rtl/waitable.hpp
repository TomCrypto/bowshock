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
///
/// Waitable functors are those functors that return a waitable state and can be passed to a waitable generator like
/// UART send or recv. These functors get called repeatedly with some data to operate on, and return whether to
/// continue, stop, or fail. There are some operations that can be carried out on these functors, such as sequencing.

#include <rtl/base.hpp>
#include <rtl/intrinsics.hpp>

namespace rtl
{

struct waitable {
public:
  enum class status {
    pending,
    complete,
    failed
  };

  /// @brief Waits for all of the waitables passed to complete or fail.
  template <typename... waitables> static auto wait_all(waitables&&... list) {
    while (!wait_all_inner(std::forward<waitables>(list)...)) {
      rtl::intrinsics::wait_for_interrupt();
    }
  }

  /// @brief Waits for any of the waitables passed to complete or fail.
  template <typename... waitables> static auto wait_any(waitables&&... list) {
    while (!wait_any_inner(std::forward<waitables>(list)...)) {
      rtl::intrinsics::wait_for_interrupt();
    }
  }

  // The sequencing operator takes N functors, calls the first one until it returns something other than pending, then:
  //  - if it returns failure, returns failure
  //  - if it returns complete, returns pending and resumes at the second argument functor
  // when the last functor is reached, returns whatever that functor returns
  // (basically it calls them in sequence until one fails or they all complete)

  // TODO: I REALLY want to do this by passing generators of those functors instead of the functors themselves. It will
  // use less memory and allow them to be "reset" as well if needed, unfortunately lambdas don't like being used that
  // way it seems. To investigate.

  template <typename... Args> static auto sequence() {
    return [](auto&&...) { return true; };
  }

  template <typename F1> static auto sequence(F1 f1) {
    return f1;
  }

  template <typename F1, typename F2> static auto sequence(F1 f1, F2 f2) {
    return [f1, f2, n = int{0}](auto&&... args) mutable {
      switch (n) {
        case  0: return f1(std::forward<decltype(args)>(args)...) ? (++n, true) : false;
        default: return f2(std::forward<decltype(args)>(args)...);
      }
    };
  }

  template <typename F1, typename F2, typename F3> static auto sequence(F1 f1, F2 f2, F3 f3) {
    return [f1, f2, f3, n = int{0}](auto&&... args) mutable {
      switch (n) {
        case  0: return f1(std::forward<decltype(args)>(args)...) ? (++n, true) : false;
        case  1: return f2(std::forward<decltype(args)>(args)...) ? (++n, true) : false;
        default: return f3(std::forward<decltype(args)>(args)...);
      }
    };
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
