// Example Arduino Sketch using RingBuffer with Interrupts
#include "RingBuffer.h"

// Define a RingBuffer for uint8_t with a size of 16
RingBuffer<uint8_t, 16> myBuffer;

volatile bool dataReady = false;

void ISR_function() {
    noInterrupts(); // Disable interrupts during buffer modification
    myBuffer.push(42); // Example data
    dataReady = true;
    interrupts(); // Re-enable interrupts
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("RingBuffer Example with Interrupts");

    attachInterrupt(digitalPinToInterrupt(2), ISR_function, RISING);
}

void loop() {
    if (dataReady) {
        noInterrupts(); // Disable interrupts during buffer modification
        uint8_t value;
        if (myBuffer.pop(value)) {
            Serial.print("Popped from ISR: ");
            Serial.println(value);
        }
        dataReady = false;
        interrupts(); // Re-enable interrupts
    }
}