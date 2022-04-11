#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>

#include <soil_moisture.cpp>
#include <photo_resistor.cpp>
#include <dht22.cpp>
#include <bmp280.cpp>

/*
* PIN mapping: configuration of the pins used by the library
*/
const lmic_pinmap lmic_pins = {
    .nss = 27,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 33,
    .dio = {26, 4, 2},
};

// 
// Provisioning API
// all those keys are necessary to join the TTN network using OTAA (over the air authentication)
// 

// App EUI used for joining the network
// use little endian format here
static const u1_t PROGMEM APPEUI[8]={ 0x4F, 0x4C, 0x4E, 0x4F, 0x4F, 0x93, 0x34, 0x4C };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// dev EUI used for joining the network
// use little-endian format here
static const u1_t PROGMEM DEVEUI[8]={0x77, 0xBE, 0x04, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// use big-endian format here
static const u1_t PROGMEM APPKEY[16] = {0x3F, 0x92, 0xDA, 0x98, 0x9C, 0xE2, 0xE1, 0x00, 0x41, 0xEE, 0x07, 0x48, 0x17, 0x03, 0xE1, 0x7E};
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

// 
// basic configuration
// 
osjob_t sendjob;
// the frequency at which to send data
const unsigned SEND_INTERVAL = 30; //in seconds
// the conversion rate from microseconds to seconds as the timer requires microseconds
const unsigned us_to_s = 1000000;

// 
// configuring deep sleep
// settings & functions used to manage power-reduction 
// 

// all those information must be restored after wake up
// thus it is stores in RTC memory
// that is the memory of the Real Time Clock - the only one kept alive while sleeping
// thus, rejoining can be avoided, the network usage reduced, and power-used reduced
RTC_DATA_ATTR bool joined = false;
RTC_DATA_ATTR u4_t netidR;
RTC_DATA_ATTR devaddr_t devaddrR;
RTC_DATA_ATTR u1_t* nwkKeyR;
RTC_DATA_ATTR u1_t* artKeyR;
RTC_DATA_ATTR u4_t seqnoUpR;
RTC_DATA_ATTR u4_t seqnoDnR;

// this function stores all required information in safe memory, shuts the library down, and sends the ESP32 to sleep
void initatite_sleep() {
    // session information is stores in RTC memory
    seqnoDnR = LMIC.seqnoDn;
    seqnoUpR = LMIC.seqnoUp;
    netidR = LMIC.netid;
    devaddrR = LMIC.devaddr;
    nwkKeyR = LMIC.nwkKey;
    artKeyR = LMIC.artKey;

    // the LoRaWAN library is shut down gracefully
    LMIC_shutdown();

    // deep sleep is activated for the requested interval
    esp_deep_sleep(SEND_INTERVAL * us_to_s);
}


// 
// settings up sensors
// those objects provide unified and simplified access to sensor data
// 
cSoilMoisture mySoilSensor = cSoilMoisture(32);
cPhotoResistor myPhotoSensor = cPhotoResistor(25);
cDHT22 myDHT22 = cDHT22(15);

// 
// this function is called when sensor data is to be sent via the network
// it collects data and queues it for sending
// 
void send(osjob_t* j) {
    
    // 
    // send battery status
    // using: LMIC_setBatteryLevel(), compare 2.5.30
    // 

    // check, wheter the library is ready to receive new sensor data
    if (LMIC_queryTxReady()) {

        // sensor measurements are collected from the sensor objects in uint8_t format as required by the library send function
        uint8_t sm = mySoilSensor.getSensorData_uint8_t();
        uint8_t pr = myPhotoSensor.getSensorData_uint8_t();
        uint8_t dht22_temp = myDHT22.getTemperature_uint8_t();
        uint8_t dht22_hum = myDHT22.getHumidity_uint8_t();

        // the sensor data is combined into one array
        uint8_t mydata[4] = { sm, pr, dht22_temp, dht22_hum };

        // the array is submited to the library and queued for sending
        LMIC_setTxData2(1, mydata, sizeof(mydata), 0);

    } else {
        // the library is unable to accept new sensor data at the moment
        // this is caused by some undefined error associated with the library being busy
    }

}

// 
// this function is called following every event registered by the LoRaWAN library
// is is used (a) for debugging, (b) sending after joining the network, and (c) to initiate a deep sleep after sending
// 
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
            // this is called when the node successfully connects to the Things Network
            Serial.println(F("EV_JOINED"));
            LMIC_setLinkCheckMode(0);
            // when the node joined the network, the joined parameter is set true
            // thus, the software knows not to rejoin when waking up from deep sleep and instead to use the stores session information
            joined = true;
            // the send function is called by the library
            // this means: after joining the network, the sensor data is sent via the network
            os_setCallback(&sendjob, send);
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
        case EV_TXCOMPLETE:
            // this is called when the node successfully sent data via the network
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
            if (LMIC.dataLen) {
              Serial.print(F("Received "));
              Serial.print(LMIC.dataLen);
              Serial.println(F(" bytes of payload"));
            }
            // after successful sending, the node is sent to sleep
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

// 
// setup
// 
void setup() {

    Serial.begin(9600);
    Serial.println(F("Starting"));

    // initial configuration of the library
    // configuring the pinmap
    os_init_ex(&lmic_pins);
    // resetting all information stored in the library and the radio module -> enables a clean start of the software
    LMIC_reset();
    // registers the callback funciton with the library
    LMIC_registerEventCb(eventCb, NULL);

    // changes behavior depennding on whether the node has already joined the network
    if (joined) {
        // called when the node has already joined the network
        // all session informaiton is restores from RTC memory

        // first, basic session parameters are set (those are callec precomputed parameters)
        LMIC_setSession(netidR, devaddrR, nwkKeyR, artKeyR);
        // then, additional information (sequence counters) are restores
        // this is necessary as not only 'precomputed' parameters are given but also a previous session is restored
        LMIC.seqnoDn = seqnoDnR;
        LMIC.saveIrqFlags = seqnoUpR;
        // perhapts, send cb needs to be called here, subject to testing
        os_setCallback(&sendjob, send);
    } else {
        // called when the node did not join the network yet
        // joins the network
        LMIC_startJoining();
    }
    
}

// 
// loop
// 
void loop() {

    // this calls the library-internal loop
    os_runloop_once();

}