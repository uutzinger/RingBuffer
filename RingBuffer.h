#ifndef RINGBUFFER_MIN
    #define RINGBUFFER_MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <Arduino.h>    // or <cstddef>, <cstring> if not using Arduino
#include <string.h>     // for memcpy

template <typename T, size_t N>
class RingBuffer {
public:
    RingBuffer() : head(0), tail(0), count(0) {}
    // Push data into the ring buffer
    // data: pointer to raw bytes
    // data_len: length of the data in bytes
    // overwrite: if true, older data is overwritten when buffer is full
    size_t push(const T& data, bool overwrite = false);
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

// Push a single element
template <typename T, size_t N>
size_t RingBuffer<T, N>::push(const T& data, bool overwrite) {
    return push(&data, 1, overwrite);
}

// Push multiple elements
template <typename T, size_t N>
size_t RingBuffer<T, N>::push(const T* data, size_t data_len, bool overwrite) {

    if (data_len == 0) return 0;

    // Check if we have enough space. If not, handle overwrite
    size_t available = N - count;
    if (data_len > available) {
        if (!overwrite) {
            // Not enough space and not allowed to overwrite
            return 0;
        }

        // Overwriting: Need to advance tail to free up space
        size_t overflow = data_len - available;
        tail = (tail + overflow) & (N - 1);
    }

    // Optimized single-character push
    if (data_len == 1) {
        buffer[head] = *data;
    } else {
        // Multi-character push
        size_t firstPart = RINGBUFFER_MIN(data_len, N - head);
        memcpy(&buffer[head], data, firstPart * sizeof(T));

        size_t secondPart = data_len - firstPart;
        if (secondPart > 0) {
            // Wrap around the buffer and write remaining bytes at the beginning
            memcpy(buffer, data + firstPart, secondPart * sizeof(T));
        }
    }

    // Update head and count
    head = (head + data_len) & (N - 1);
    count = RINGBUFFER_MIN(static_cast<size_t>(N), static_cast<size_t>(count + data_len));

    return data_len;
}

// Pop a single element
template <typename T, size_t N>
size_t RingBuffer<T, N>::pop(T& output) {
    return pop(&output, 1);
}

// Pop multiple elements
template <typename T, size_t N>
size_t RingBuffer<T, N>::pop(T* output, size_t len) {
    if (count == 0) return 0; // Buffer empty

    size_t charsToRead = RINGBUFFER_MIN(len, static_cast<size_t>(count));
    size_t firstPart = RINGBUFFER_MIN(charsToRead, N - tail);

    if (charsToRead == 1) {
        *output = buffer[tail];
    } else {
        memcpy(output, &buffer[tail], firstPart*sizeof(T));
        size_t secondPart = charsToRead - firstPart;
        if (secondPart > 0) {
            // Wrap around the buffer and read remaining bytes from the beginning
            memcpy(output + firstPart, buffer, secondPart * sizeof(T));
        }
    }

    tail = (tail + charsToRead) & (N - 1);

    count -= charsToRead;

    // Reset tail & head when buffer becomes empty
    if (count == 0) {
        tail = 0;
        head = 0;
    }

    return charsToRead;
}

template <typename T, size_t N>
size_t RingBuffer<T, N>::peek(T& output) const {
    if (count == 0) return 0; // Buffer empty

    output = buffer[tail]; // Read the element at tail without modifying it
    return 1;
}

template <typename T, size_t N>
size_t RingBuffer<T, N>::peek(T* output, size_t len) const {
    if (count == 0 || len == 0) return 0; // Nothing to peek

    size_t peekLen = RINGBUFFER_MIN(len, static_cast<size_t>(count));
    size_t firstPart = RINGBUFFER_MIN(peekLen, N - tail);

    // Copy first segment
    memcpy(output, &buffer[tail], firstPart * sizeof(T));

    // Copy second segment if wrap-around occurs
    size_t secondPart = peekLen - firstPart;
    if (secondPart > 0) {
        memcpy(output + firstPart, buffer, secondPart * sizeof(T));
    }

    return peekLen;
}

// Clear the buffer
template <typename T, size_t N>
void RingBuffer<T, N>::clear() {
    head = 0;
    tail = 0;
    count = 0;
}

#endif // RINGBUFFER_H
