#include "RingBuffer.h"

// Store buffer in RTC memory (persists across deep sleep)
RTC_DATA_ATTR RingBuffer<uint8_t, 16> myBuffer;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("RingBuffer Example - Survives Deep Sleep");

    // Check if ESP woke from deep sleep
    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
        Serial.println("Woke from Deep Sleep!");
    } else {
        Serial.println("Fresh Start");
    }
}

void loop() {
    // Example: Push data before sleeping
    myBuffer.push(99);
    Serial.println("Pushed 99");

    // Go to deep sleep
    Serial.println("Going to deep sleep...");
    esp_sleep_enable_timer_wakeup(5000000); // Wake up in 5 sec
    esp_deep_sleep_start();
}
