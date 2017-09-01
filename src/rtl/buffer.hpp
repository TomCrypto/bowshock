#pragma once

/// @file
///
/// @brief Generic memory buffer view

namespace rtl
{

// do we want to store buffer position in this class? probably not, we'd want a buffer reader/buffer writer interface
// for it...

// TODO: we need different types for buffer and const_buffer...
// Should we use iterators instead? probably makes more sense, and is a proven abstraction

// SOLUTION: define an iterator base class (basically, reconstruct the STL iterator traits)
// and pass the begin/end iterators BY POINTER?
// memory usage is pretty bad though: 4 bytes on the UART class to store the pointer, then 4 bytes (best case) on the
// caller's stack to keep the iterator around (bad! why does an iterator need to be kept around??)

struct buffer
{
public:
  /// @brief Initializes a new buffer with the given memory address and size in bytes.
  constexpr buffer(void* data, std::size_t size) : data(data), size_(size) {}
  buffer() {}

  template <typename T> auto get(std::size_t index) const {
    return reinterpret_cast<T*>(data)[index];
  }

  template <typename T> auto put(std::size_t index, T value) {
    reinterpret_cast<T*>(data)[index] = value;
  }

  auto size() const {
    return size_;
  }

private:
  void* data;
  std::size_t size_;
};

/// @brief An object that can linearly read through a buffer.
template <typename T> struct linear_reader {
public:
  constexpr linear_reader(buffer buffer) : buffer(buffer), position(0) {}
  constexpr linear_reader() {}

  auto read() {
    return buffer.get<T>(position++ * sizeof(T));
  }

  auto end_reached() const {
    return position * sizeof(T) == buffer.size();
  }

private:
  buffer buffer;
  std::size_t position;
};

/// @brief An object that can linearly write through a buffer.
template <typename T> struct linear_writer {
public:
  constexpr linear_writer(buffer buffer) : buffer(buffer), position(0) {}
  constexpr linear_writer() {}

  auto write(T value) {
    return buffer.put<T>(position++ * sizeof(T), value);
  }

  auto end_reached() const {
    return position * sizeof(T) == buffer.size();
  }

private:
  buffer buffer;
  std::size_t position;
};

}