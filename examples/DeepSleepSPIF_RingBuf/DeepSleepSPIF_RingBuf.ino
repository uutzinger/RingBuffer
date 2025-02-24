// Example Arduino Sketch using RingBuffer with SPIFFS
#include "RingBuffer.h"
#include "FS.h"
#include "SPIFFS.h"

#define BUFFER_SIZE 16
#define BUFFER_FILE "/ringbuffer.dat"

// Define a RingBuffer for uint8_t
RingBuffer<uint8_t, BUFFER_SIZE> myBuffer;

volatile bool dataReady = false;

void ISR_function() {
    noInterrupts(); // Disable interrupts during buffer modification
    myBuffer.push(42); // Example data
    dataReady = true;
    interrupts(); // Re-enable interrupts
}

// Save buffer contents to SPIFFS
void saveBufferToSPIFFS() {
    File file = SPIFFS.open(BUFFER_FILE, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }

    while (myBuffer.available() > 0) {
        uint8_t value;
        myBuffer.pop(value);
        file.write(value);
    }

    file.close();
    Serial.println("Buffer saved to SPIFFS");
}

// Load buffer contents from SPIFFS
void loadBufferFromSPIFFS() {
    File file = SPIFFS.open(BUFFER_FILE, FILE_READ);
    if (!file) {
        Serial.println("No saved buffer found");
        return;
    }

    while (file.available()) {
        uint8_t value = file.read();
        myBuffer.push(value);
    }

    file.close();
    Serial.println("Buffer restored from SPIFFS");
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("RingBuffer Example with SPIFFS");

    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    loadBufferFromSPIFFS(); // Restore buffer from SPIFFS

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

    // Example: Save buffer before sleep
    if (millis() > 10000) { // Every 10 sec
        saveBufferToSPIFFS();
        Serial.println("Going to deep sleep...");
        esp_sleep_enable_timer_wakeup(5000000); // Wake up in 5 sec
        esp_deep_sleep_start();
    }
}
