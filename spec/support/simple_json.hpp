#pragma once

#include <rtl/base.hpp>

namespace spec::json
{

// really simple (and incomplete) JSON output class

namespace detail
{

template <typename T> constexpr auto max(T x, T y) {
  return (x < y) ? y : x;
}

}

template <typename... Elements> class object {
public:
  constexpr object(Elements&&... elements) : elements(std::forward<Elements>(elements)...) {}

  constexpr std::size_t size() const {
    return detail::max(2, 1 + 4 * element_sizes(index_sequence()));
  }

  char* dump(char* buffer) const {
    *buffer++ = '{';

    for_each_element([&](auto&& element){
      *buffer++ = '\"'; // "
      buffer = copy_str(buffer, element.first);
      *buffer++ = '\"'; // "
      *buffer++ = ':';
      buffer = element.second.dump(buffer);
      *buffer++ = ',';
    }, index_sequence());

    if (index_sequence().size() > 0) {
      *(buffer - 1) = '}';
    } else {
      *buffer++ = '}';
    }

    *buffer = '\0';
    return buffer;
  }

private:
  std::tuple<Elements...> elements;

  static constexpr auto index_sequence() {
    return std::make_index_sequence<std::tuple_size<decltype(elements)>::value>{};
  }

  template <typename F, std::size_t... Seq> constexpr auto for_each_element(F&& f, std::index_sequence<Seq...>) const {
    (f(std::get<Seq>(elements)), ...);
  }

  template <std::size_t... Seq> constexpr auto element_sizes(std::index_sequence<Seq...>) const {
    return (element_size(std::get<Seq>(elements)) + ...);
  }

  template <typename T> constexpr auto element_size(T&& element) const {
    return strlen(element.first) + element.second.size();
  }

  static auto copy_str(char* buffer, const char* str) {
    while (*str != '\0') {
      *buffer++ = *str++;
    }

    return buffer;
  }
};

template <typename... Elements> class array {
public:
  constexpr array(Elements&&... elements) : elements(std::forward<Elements>(elements)...) {}

  constexpr std::size_t size() const {
    return detail::max(2, 1 + element_sizes(index_sequence()));
  }

  char* dump(char* buffer) const {
    *buffer++ = '[';

    for_each_element([&](auto&& element){
      buffer = element.dump(buffer);
      *buffer++ = ',';
    }, index_sequence());

    if (index_sequence().size() > 0) {
      *(buffer - 1) = ']';
    } else {
      *buffer++ = ']';
    }

    *buffer = '\0';
    return buffer;
  }

private:
  std::tuple<Elements...> elements;

  static constexpr auto index_sequence() {
    return std::make_index_sequence<std::tuple_size<decltype(elements)>::value>{};
  }

  template <typename F, std::size_t... Seq> constexpr auto for_each_element(F&& f, std::index_sequence<Seq...>) const {
    (f(std::get<Seq>(elements)), ...);
  }

  template <std::size_t... Seq> constexpr auto element_sizes(std::index_sequence<Seq...>) const {
    return (std::get<Seq>(elements).size() + ...);
  }
};

class boolean {
public:
  constexpr boolean(bool value) : value(value) {}

  std::size_t size() const {
    return value ? 4 : 5;
  }

  char* dump(char* buffer) const {
    if (value) {
      *buffer++ = 't';
      *buffer++ = 'r';
      *buffer++ = 'u';
      *buffer++ = 'e';
    } else {
      *buffer++ = 'f';
      *buffer++ = 'a';
      *buffer++ = 'l';
      *buffer++ = 's';
      *buffer++ = 'e';
    }

    *buffer = '\0';
    return buffer;
  }

private:
  bool value;
};

class string {
public:
  constexpr string(const char* value) : value(value) {}

  std::size_t size() const {
    return 2 + strlen(value);
  }

  char* dump(char* buffer) const {
    *buffer++ = '\"'; // "

    for (auto i = 0; value[i] != '\0'; ++i) {
      *buffer++ = value[i];
    }

    *buffer++ = '\"'; // "

    *buffer = '\0';
    return buffer;
  }

private:
  const char* value;
};

class number {
public:
  template <typename T> constexpr number(T value) : value(static_cast<rtl::i64>(value)) {}

  constexpr std::size_t size() const {
    auto size = std::size_t{0};
    auto number = value;

    if (number < 0) {
      number = -number;
      ++size;
    }

    do {
      number /= 10;
      ++size;
    } while (number != 0);

    return size;
  }

  char* dump(char* buffer) const {
    auto number = value;

    auto end_pos = buffer + size();
    auto end_pos_copy = end_pos;

    if (number < 0) {
      number = -number;
      *buffer++ = '-';
    }

    do {
      *--end_pos_copy = '0' + (number % 10);
      number /= 10;
    } while (number != 0);

    *end_pos = '\0';
    return end_pos;
  }

private:
  rtl::i64 value;
};

}
