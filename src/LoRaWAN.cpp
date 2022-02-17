#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>

/*
* PIN mapping
*/
const lmic_pinmap lmic_pins = {
    .nss = 27,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 33,
    .dio = {26, 4, 2},
};

/*
 * Provisioning API
 */
// little-endian format
static const u1_t PROGMEM APPEUI[8]={ 0x4F, 0x4C, 0x4E, 0x4F, 0x4F, 0x93, 0x34, 0x4C };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// little-endian format
static const u1_t PROGMEM DEVEUI[8]={ 0x77, 0xBE, 0x04, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// big-endian format
static const u1_t PROGMEM APPKEY[16] = { 0x3F, 0x92, 0xDA, 0x98, 0x9C, 0xE2, 0xE1, 0x00, 0x41, 0xEE, 0x07, 0x48, 0x17, 0x03, 0xE1, 0x7E };
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

void send() {
    /*  do all sending tasks here!!!
    *   submit all data to send using: LMIC_setTxData2();
    */
}

void setup() {

    os_init_ex(&lmic_pins);

    // replace in case of sleep mode use: reload session details here
    if (true)
    {
        LMIC_reset();
        LMIC_startJoining();
    } else {
        //LMIC_setSession();
    }
    

}

void loop() {

    os_runloop_once();

}