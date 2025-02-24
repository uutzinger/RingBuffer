// Example Arduino Sketch using RingBuffer
#include "RingBuffer.h"

// Define a RingBuffer for uint8_t with a size of 16
RingBuffer<uint8_t, 16> myBuffer;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("RingBuffer Example");
}

void loop() {
    // Push data into the buffer
    for (uint8_t i = 1; i <= 10; i++) {
        myBuffer.push(i);
        Serial.print("Pushed: ");
        Serial.println(i);
    }

    // Pop data from the buffer
    Serial.println("\nPopping data:");
    while (myBuffer.available() > 0) {
        uint8_t value;
        myBuffer.pop(value);
        Serial.print("Popped: ");
        Serial.println(value);
    }
    delay(1000); // Delay for readability
    // Clear the buffer
    Serial.println("Clearing Buffer.");
    myBuffer.clear();
    delay(1000); // Delay for readability
}
