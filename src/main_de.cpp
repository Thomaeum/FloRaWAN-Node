#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>

const lmic_pinmap lmic_pins = {
    .nss = 2,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 1,
    .dio = {7, 6, 5},
};

void setup() {
    Serial.begin(9600);
    Serial.println(F("Starting"));

    os_init_ex(&lmic_pins);
}

void loop() {
    Serial.println(F("Starting"));
}
