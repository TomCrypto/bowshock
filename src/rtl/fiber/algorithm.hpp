#pragma once

/// @file
///
/// @brief Waitable fiber algorithmic primitives.
///
/// A *waitable fiber* is any functor that returns a \c waitable::status, regardless of the arguments it takes. Two
/// waitable fibers are *compatible* if they take the same argument types. A *fiber generator* is a functor that takes
/// no arguments and returns an instance of a waitable fiber.
///
/// These fibers are called in the context of interrupt handlers in order to perform some action. They can be waited on
/// by the main thread, and more importantly, they can be composed together to carry out more elaborate actions.

namespace rtl {

inline constexpr auto sequence() {
  return [](auto&&...) { return rtl::waitable::status::complete; };
}

template <typename G1> constexpr auto sequence(G1 g1) {
  return [f1 = g1()](auto&&... args) mutable { return f1(std::forward<decltype(args)>(args)...); };
}

/// @brief Sequence primitive.
///
/// This primitive takes N fibers and completes them in sequence, or fails on the first failure.
template <typename G1, typename G2> constexpr auto sequence(G1 g1, G2 g2) {
  union {
    decltype(g1()) f1;
    decltype(g2()) f2;
  } storage{g1()};

  return [g2, storage, n = std::size_t{0}](auto&&... args) mutable {
    switch (n) {
      case  0:
        switch (storage.f1(std::forward<decltype(args)>(args)...)) {
          case rtl::waitable::status::pending:
            return rtl::waitable::status::pending;
          case rtl::waitable::status::failed:
            return rtl::waitable::status::failed;
          case rtl::waitable::status::complete:
            (&storage.f1)->~decltype(g1())();
            new (&storage.f2) decltype(g2())(g2());
            ++n;
        }
      default:
        return storage.f2(std::forward<decltype(args)>(args)...);
    }
  };
}

template <typename G1, typename G2, typename G3> constexpr auto sequence(G1 g1, G2 g2, G3 g3) {
  union {
    decltype(g1()) f1;
    decltype(g2()) f2;
    decltype(g3()) f3;
  } storage{g1()};

  return [g2, g3, storage, n = std::size_t{0}](auto&&... args) mutable {
    switch (n) {
      case  0:
        switch (storage.f1(std::forward<decltype(args)>(args)...)) {
          case rtl::waitable::status::pending:
            return rtl::waitable::status::pending;
          case rtl::waitable::status::failed:
            return rtl::waitable::status::failed;
          case rtl::waitable::status::complete:
            (&storage.f1)->~decltype(g1())();
            new (&storage.f2) decltype(g2())(g2());
            ++n;
        }
      case  1:
        switch (storage.f2(std::forward<decltype(args)>(args)...)) {
          case rtl::waitable::status::pending:
            return rtl::waitable::status::pending;
          case rtl::waitable::status::failed:
            return rtl::waitable::status::failed;
          case rtl::waitable::status::complete:
            (&storage.f2)->~decltype(g2())();
            new (&storage.f3) decltype(g3())(g3());
            ++n;
        }
      default:
        return storage.f3(std::forward<decltype(args)>(args)...);
    }
  };
}

}
