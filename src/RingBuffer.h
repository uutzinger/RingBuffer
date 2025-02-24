#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <Arduino.h>    // or <cstddef>, <cstring> if not using Arduino

template <typename T, size_t N>
class RingBuffer {
public:
    size_t push(const T& data, bool overwrite = false);
    // Constructor to initialize the buffer with a given capacity
    RingBuffer();

    // Push data into the ring buffer
    // data: pointer to raw bytes
    // data_len: length of the data in bytes
    // overwrite: if true, older data is overwritten when buffer is full
    size_t push(T& data, bool overwrite = false);
    size_t push(const T* data, size_t data_len, bool overwrite = false);

    // Pop a specified number of characters from the ring buffer
    size_t pop(T& output);
    size_t pop(T* output, size_t len);
    
    size_t peek(T& output) const;
    size_t peek(T* output, size_t len) const;

    size_t available() const { return count; }
    size_t capacity() const { return N; }
    void clear();

private:
    T buffer[N]; // fixed size buffer

    // Select optimal index type based on buffer size
    using IndexType = typename std::conditional<
        (N <= 256), uint8_t,
        typename std::conditional<(N <= 65536), uint16_t, size_t>::type
    >::type;

    IndexType head;
    IndexType tail;
    IndexType count;

    static constexpr bool isPowerOfTwo(size_t n) { return (n & (n - 1)) == 0; }
    static_assert(isPowerOfTwo(N), "RingBuffer capacity must be a power of 2 for efficiency");

};

#endif // RINGBUFFER_H
