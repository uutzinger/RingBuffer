#include "RingBuffer.h" // for RingBuffer class
#include <string.h>     // for memcpy

// Constructor: Initialize buffer
template <typename T, size_t N>
RingBuffer<T, N>::RingBuffer() : head(0), tail(0), count(0) {}

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
        size_t firstPart = min(data_len, N - head);
        memcpy(&buffer[head], data, firstPart * sizeof(T));

        size_t secondPart = data_len - firstPart;
        if (secondPart > 0) {
            // Wrap around the buffer and write remaining bytes at the beginning
            memcpy(buffer, data + firstPart, secondPart * sizeof(T));
        }
    }

    // Update head and count
    head = (head + data_len) & (N - 1);
    count = min(N, count + data_len); 

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

    size_t charsToRead = min(len, count);
    size_t firstPart = min(charsToRead, N - tail);

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

    size_t peekLen = (len > count) ? count : len; // Limit peek length to available elements
    size_t firstPart = min(peekLen, N - tail);

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