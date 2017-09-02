#pragma once

/// @file
///
/// @brief Functional stuff

namespace rtl
{

/// @brief Interrupt context bridge.
///
/// This structure holds a so-called "context" in which an ordinarily stateless interrupt handler executes. It will
/// wrap a member function and a \c this pointer together in variables which can be accessed from within a handler.
///
/// @remarks The contexts wrapped by this class do not return anything, as interrupt handlers could not anything with
///          the result anyway. They are meant as a low-level mechanism to bridge the gap between stateless IRQs and
///          stateful application code.
template <typename ...Args> struct interrupt_context : private rtl::noncopyable {
private:
  using fn_t = void (*)(void*, Args&& ...args);

public:
  /// @brief Defines a stateless member function wrapper function used for forwarding.
  ///
  /// @remark By default, this helper targets a void function called "interrupt" on the class. Use the second template
  ///         argument to override this and select another member function instead.
  template <typename T, void (T::*Fn)(Args...) = &T::interrupt> static auto member_function(void* object,
                                                                                            Args&& ...args) {
    return ((static_cast<T*>(object))->*Fn)(std::forward<Args>(args)...);
  }

  /// @brief Initializes the interrupt context with a function and a context pointer.
  constexpr interrupt_context(fn_t function, void* context = nullptr) : function(function), context(context) {}
  constexpr interrupt_context() {}

  /// @brief Resets the context pointer. If none is passed, the interrupt context becomes invalid.
  auto reset (void* context = nullptr) {
    this->context = context;
  }

  /// @brief Returns whether the interrupt context contains a valid (initialized) state.
  auto valid () {
    return context != nullptr;
  }

  /// @brief Calls the interrupt context. Undefined if the context is not valid.
  auto operator()(Args ...args) {
    return function(context, std::forward<Args>(args)...);
  }

private:
  fn_t function{};
  void* context{};
};

}
