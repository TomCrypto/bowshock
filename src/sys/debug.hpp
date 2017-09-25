#pragma once

/// @file
///
/// @brief Debugging utilities.

#include <rtl/base.hpp>

namespace sys {

namespace detail {

auto write_str(const char* buf) {
  return [buf, index = std::size_t{0}](rtl::u8& data) mutable {
    if (buf[index] == '\0') {
      return true;
    }
    
    data = buf[index++];
    return false;
  };
}

auto write_int(rtl::i64 n) {
  static char buffer[32] = {};
  std::size_t pos = 31;

  auto negative = (n < 0);
  if (negative) {
    n = -n;
  }

  buffer[pos--] = '\0';

  if (n == 0) {
    buffer[pos--] = '0';
  } else while (n != 0) {
    buffer[pos--] = '0' + (n % 10);
    n /= 10;
  }

  if (negative) {
    buffer[pos--] = '-';
  }

  return write_str(buffer + pos + 1);
}

}

/// @brief Base definition for a debug formatter.
///
/// @remarks Additional specializations over the \c F type with custom format strings can be introduced.
template <typename F> auto debug_formatter(const std::pair<const char*, F>& fragment);

/// @brief String debug formatter.
template <> auto debug_formatter<const char*>(const std::pair<const char*, const char*>& fragment) {
  return detail::write_str(fragment.second);
}

template <> auto debug_formatter<unsigned int>(const std::pair<const char*, unsigned int>& fragment) {
  return detail::write_int(fragment.second);
}

template <> auto debug_formatter<rtl::i16>(const std::pair<const char*, rtl::i16>& fragment) {
  return detail::write_int(fragment.second);
}

template <> auto debug_formatter<rtl::u16>(const std::pair<const char*, rtl::u16>& fragment) {
  return detail::write_int(fragment.second);
}

template <> auto debug_formatter<rtl::i32>(const std::pair<const char*, rtl::i32>& fragment) {
  return detail::write_int(fragment.second);
}

template <> auto debug_formatter<rtl::i64>(const std::pair<const char*, rtl::i64>& fragment) {
  return detail::write_int(fragment.second);
}


template <typename... Args>
struct union_size_of {
  static constexpr auto size = 0;
};

template <typename T, typename... Args>
struct union_size_of<T, Args...> {
  using base_type = typename T::second_type;

  static constexpr auto head = sizeof(decltype(debug_formatter<base_type>(std::declval<const std::pair<const char*, base_type>&>())));
  static constexpr auto tail = union_size_of<Args...>::size;

  static constexpr auto size = (head < tail ? tail : head);
};

template <typename... Args>
struct union_alignof {
  static constexpr auto size = 0;
};

template <typename T, typename... Args>
struct union_alignof<T, Args...> {
  using base_type = typename T::second_type;

  static constexpr auto head = alignof(decltype(debug_formatter<base_type>(std::declval<const std::pair<const char*, base_type>&>())));
  static constexpr auto tail = union_size_of<Args...>::size;

  static constexpr auto size = (head < tail ? tail : head);
};

template <typename F>
auto debug(const std::pair<const char*, F>& fragment) {
  //output.write(debug_formatter(fragment)).wait();
}

//template <typename T, typename F, typename... Args>
//auto debug(T& output, const std::pair<const char*, F>& fragment, Args&&... rest) {
template <typename X, typename Y>
auto debug(const std::pair<const char*, X>& x, const std::pair<const char*, Y>& y) {
  // TODO: need to handle errors somehow!!
  //output.write(debug_formatter(fragment)).wait();
  //debug(output, std::forward<Args>(rest)...);

  #if 0
  constexpr auto alignment = union_alignof<std::pair<const char*, X>, std::pair<const char*, Y>>::size;
  constexpr auto size = union_size_of<std::pair<const char*, X>, std::pair<const char*, Y>>::size;
  alignas(alignment) unsigned char storage[size];

  new (storage) decltype(debug_formatter<X>(std::declval<const std::pair<const char*, X>&>()))(debug_formatter(x));

  return [storage, y, n = std::size_t{0}](auto& data) mutable {
    switch (n) {
      case 0:
        if ((*reinterpret_cast<typename std::add_pointer<decltype(debug_formatter<X>(std::declval<const std::pair<const char*, X>&>()))>::type>(storage))(data)) {
          new (storage) decltype(debug_formatter<Y>(std::declval<const std::pair<const char*, Y>&>()))(debug_formatter(y));
          ++n;
          return false;
        } else {
          return false;
        }
      default:
        if ((*reinterpret_cast<typename std::add_pointer<decltype(debug_formatter<Y>(std::declval<const std::pair<const char*, Y>&>()))>::type>(storage))(data)) {
          return true;
        } else {
          return false;
        }
    }
  };
  #endif

  return rtl::waitable::sequence(debug_formatter(x), debug_formatter(y));
}

/*

lambda:

count = 0 [capture]

switch (count) {
  case 0:
    if (first()) {
      ++count;
      return true;
    } else {
      return false;
    }
  case 1:
}

*/

}
