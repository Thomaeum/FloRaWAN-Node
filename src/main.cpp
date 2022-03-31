#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>

#include <soil_moisture.cpp>
#include <photo_resistor.cpp>
#include <dht22.cpp>
#include <bmp280.cpp>

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
const unsigned SEND_INTERVAL = 30; //in seconds
const unsigned us_to_s = 1000000;

//required for session restore
RTC_DATA_ATTR bool joined = false;
RTC_DATA_ATTR u4_t netidR;
RTC_DATA_ATTR devaddr_t devaddrR;
RTC_DATA_ATTR u1_t* nwkKeyR;
RTC_DATA_ATTR u1_t* artKeyR;
RTC_DATA_ATTR u4_t seqnoUpR;
RTC_DATA_ATTR u4_t seqnoDnR;

cSoilMoisture mySoilSensor = cSoilMoisture(32);
cPhotoResistor myPhotoSensor = cPhotoResistor(25);
cDHT22 myDHT22 = cDHT22(15);
//cBMT280 myBMP280 = cBMT280();

/*
*   sending data
*/
void send(osjob_t* j) {
    
    /*
    *   send battery status
    *   using: LMIC_setBatteryLevel(), compare 2.5.30
    */

    if (LMIC_queryTxReady()) {

        uint8_t sm = mySoilSensor.getSensorData_uint8_t();
        uint8_t pr = myPhotoSensor.getSensorData_uint8_t();
        uint8_t dht22_temp = myDHT22.getTemperature_uint8_t();
        uint8_t dht22_hum = myDHT22.getHumidity_uint8_t();
        //uint8_t bmp280_temp = myBMP280.getTemperature();
        //uint8_t bmp280_press = myBMP280.getPressure();

        //uint8_t mydata[6] = {sm, pr, dht22_temp, dht22_hum, bmp280_temp, bmp280_press};
        uint8_t mydata[4] = { sm, pr, dht22_temp, dht22_hum };

        /*Serial.println(sm);
        Serial.println(pr);
        Serial.println(dht22_temp);
        Serial.println(dht22_hum);*/

        LMIC_setTxData2(1, mydata, sizeof(mydata), 0);
        //Serial.println(F("Packet queued"));
    } else {
        /* something is already happening, probably sending has already been initiaed */
    }
    //os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(SEND_INTERVAL), send);

}

void initatite_sleep() {
    //store session information here
    seqnoDnR = LMIC.seqnoDn;
    seqnoUpR = LMIC.seqnoUp;
    netidR = LMIC.netid;
    devaddrR = LMIC.devaddr;
    nwkKeyR = LMIC.nwkKey;
    artKeyR = LMIC.artKey;

    //gracefully shutdown the library
    LMIC_shutdown();

    //sending to deep_sleep
    esp_deep_sleep(SEND_INTERVAL * us_to_s);
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
            joined = true;
            os_setCallback(&sendjob, send);
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
            initatite_sleep();
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

    Serial.begin(9600);
    Serial.println(F("Starting"));

    os_init_ex(&lmic_pins);
    LMIC_reset();
    LMIC_registerEventCb(eventCb, NULL);

    if (joined) {
        LMIC_setSession(netidR, devaddrR, nwkKeyR, artKeyR);
        LMIC.seqnoDn = seqnoDnR;
        LMIC.saveIrqFlags = seqnoUpR;
        // perhapts, send cb needs to be called here
            // os_setCallback(&sendjob, send);
    } else {
        LMIC_startJoining();
    }
    
}

/*
*   loop
*/
void loop() {

    os_runloop_once();

}