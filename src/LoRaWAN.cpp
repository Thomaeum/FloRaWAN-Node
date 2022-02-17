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

/*
*   basic configuration
*/
osjob_t sendjob;
const unsigned TX_INTERVAL = 60;

/*
*   sending data
*/
void send(osjob_t* j) {
    
    /*
    *   send battery status
    *   using: LMIC_setBatteryLevel(), compare 2.5.30
    */

    if (LMIC_queryTxReady()) {
        /*  do all sending tasks here!!!
        *   submit all data to send using: LMIC_setTxData2();
        *   compare 2.5.15 for details
        */
    } else {
        /* something is already happening, probably sending has already been initiaed */
    }

}

/*
*   registering event callback
*/
void eventCb(void *pUserData, ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            Serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));
            LMIC_setLinkCheckMode(0);
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
            if (LMIC.dataLen) {
              Serial.print(F("Received "));
              Serial.print(LMIC.dataLen);
              Serial.println(F(" bytes of payload"));
            }
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), send);
            break;
        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
        case EV_TXSTART:
            Serial.println(F("EV_TXSTART"));
            break;
        case EV_TXCANCELED:
            Serial.println(F("EV_TXCANCELED"));
            break;
        case EV_RXSTART:
            /* do not print anything -- it wrecks timing */
            break;
        case EV_JOIN_TXCOMPLETE:
            Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
            break;
        default:
            Serial.print(F("Unknown event: "));
            Serial.println((unsigned) ev);
            break;
    }
}

/*
*   setup
*/
void setup() {

    os_init_ex(&lmic_pins);

    // replace in case of sleep mode use: reload session details here
    if (true)
    {
        LMIC_reset();
        LMIC_registerEventCb(eventCb, NULL);
        LMIC_startJoining();
        send(&sendjob);
    } else {
        /*  LMIC_setSession();
        *   compare section 2.5.4
        */
    }
    

}

/*
*   loop
*/
void loop() {

    os_runloop_once();

}