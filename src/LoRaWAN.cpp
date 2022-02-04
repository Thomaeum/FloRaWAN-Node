/*

Module:  simple_sensor_bme280.ino

Function:
    Example app showing how to periodically poll a
    sensor.

Copyright notice and License:
    See LICENSE file accompanying this project.

Author:
    Terry Moore, MCCI Corporation	May 2021

Notes:
    This app compiles and runs on an MCCI Catena 4610 board.

*/

#include <Arduino_LoRaWAN_network.h>
#include <Arduino_LoRaWAN_EventLog.h>
#include <arduino_lmic.h>

/****************************************************************************\
|
|	The LoRaWAN object
|
\****************************************************************************/

class cMyLoRaWAN : public Arduino_LoRaWAN_network {
public:
    cMyLoRaWAN() {};
    using Super = Arduino_LoRaWAN_network;
    void setup();

protected:
    // you'll need to provide implementation for this.
    virtual bool GetOtaaProvisioningInfo(Arduino_LoRaWAN::OtaaProvisioningInfo*) override;
    // if you have persistent storage, you can provide implementations for these:
    virtual void NetSaveSessionInfo(const SessionInfo &Info, const uint8_t *pExtraInfo, size_t nExtraInfo) override;
    virtual void NetSaveSessionState(const SessionState &State) override;
    virtual bool NetGetSessionState(SessionState &State) override;
};


/****************************************************************************\
|
|	The sensor object
|
\****************************************************************************/

/****************************************************************************\
|
|	Globals
|
\****************************************************************************/

// the global LoRaWAN instance.
cMyLoRaWAN myLoRaWAN {};

// the global event log instance
Arduino_LoRaWAN::cEventLog myEventLog;

// pin map
const cMyLoRaWAN::lmic_pinmap myPinMap = {
     .nss = 27,
     .rxtx = cMyLoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN,
     .rst = 33,
     .dio = { 26, 4, 2 },
};

std::uint8_t uplink[7];

int wahrheit;

/****************************************************************************\
|
|	Provisioning info for LoRaWAN OTAA
|
\****************************************************************************/

// deveui, little-endian
static const std::uint8_t deveui[] = { 0x77, 0xBE, 0x04, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };

// appeui, little-endian
static const std::uint8_t appeui[] = { 0x4F, 0x4C, 0x4E, 0x4F, 0x4F, 0x93, 0x34, 0x4C };

// appkey: just a string of bytes, sometimes referred to as "big endian".
static const std::uint8_t appkey[] = { 0x3F, 0x92, 0xDA, 0x98, 0x9C, 0xE2, 0xE1, 0x00, 0x41, 0xEE, 0x07, 0x48, 0x17, 0x03, 0xE1, 0x7E };

/****************************************************************************\
|
|	setup()
|
\****************************************************************************/

void setup() {
    // set baud rate, and wait for serial to be ready.
    Serial.begin(115200);
    while (! Serial)
        yield();

    Serial.println(F("starting setup"));

    // set up the log; do this fisrt.
    myEventLog.setup();

    // set up lorawan.
    myLoRaWAN.setup();

    uplink[0] = std::uint8_t(1);
    uplink[1] = std::uint8_t(2);
    uplink[2] = std::uint8_t(3);
    uplink[3] = std::uint8_t(4);
    uplink[4] = std::uint8_t(5);
    uplink[5] = std::uint8_t(6);
    uplink[6] = std::uint8_t(7);

    wahrheit = false;
}

/****************************************************************************\
|
|	loop()
|
\****************************************************************************/

void loop() {
    // the order of these is arbitrary, but you must poll them all.
    for (size_t i = 1000; i > 0; --i) {
        myLoRaWAN.loop();
        myEventLog.loop();
    }

    if (!wahrheit) {
        wahrheit = true;
        myLoRaWAN.SendBuffer(
            uplink,
            sizeof(uplink),
            [](void *pClientData, bool fSucccess) -> void {
                Serial.println(fSucccess);
                Serial.println("Wir sind krass.");
            },
            uplink,
            false,
            1
        );
    }
}

/****************************************************************************\
|
|	LoRaWAN methods
|
\****************************************************************************/

// our setup routine does the class setup and then registers an event handler so
// we can see some interesting things
void
cMyLoRaWAN::setup() {
    // simply call begin() w/o parameters, and the LMIC's built-in
    // configuration for this board will be used.
    this->Super::begin(myPinMap);

    // LMIC_selectSubBand(0);
    LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);

    this->RegisterListener(
        // use a lambda so we're "inside" the cMyLoRaWAN from public/private perspective
        [](void *pClientInfo, uint32_t event) -> void {
            auto const pThis = (cMyLoRaWAN *)pClientInfo;

            // for tx start, we quickly capture the channel and the RPS
            if (event == EV_TXSTART) {
                // use another lambda to make log prints easy
                myEventLog.logEvent(
                    (void *) pThis,
                    LMIC.txChnl,
                    LMIC.rps,
                    0,
                    // the print-out function
                    [](cEventLog::EventNode_t const *pEvent) -> void {
                        Serial.print(F(" TX:"));
                        myEventLog.printCh(std::uint8_t(pEvent->getData(0)));
                        myEventLog.printRps(rps_t(pEvent->getData(1)));
                    }
                );
            }
            // else if (event == some other), record with print-out function
            else {
                // do nothing.
            }
        },
        (void *) this   // in case we need it.
        );
}

// this method is called when the LMIC needs OTAA info.
// return false to indicate "no provisioning", otherwise
// fill in the data and return true.
bool
cMyLoRaWAN::GetOtaaProvisioningInfo(OtaaProvisioningInfo *pInfo) {
    // these are the same constants used in the LMIC compliance test script; eases testing
    // with the RedwoodComm RWC5020B/RWC5020M testers.

    // initialize info
    memcpy(pInfo->DevEUI, deveui, sizeof(pInfo->DevEUI));
    memcpy(pInfo->AppEUI, appeui, sizeof(pInfo->AppEUI));
    memcpy(pInfo->AppKey, appkey, sizeof(pInfo->AppKey));

    return true;
}

// save Info somewhere (if possible)
// if not possible, just do nothing and make sure you return false
// from NetGetSessionState().
void
cMyLoRaWAN::NetSaveSessionInfo(const SessionInfo &Info, const uint8_t *pExtraInfo, size_t nExtraInfo) {
    // in this example, we can't save, so we just return.
}

// save State somewhere. Note that it's often the same;
// often only the frame counters change.
// if not possible, just do nothing and make sure you return false
// from NetGetSessionState().
void
cMyLoRaWAN::NetSaveSessionState(const SessionState &State) {
    // in this example, we can't save, so we just return.
}

// either fetch SessionState from somewhere and return true or...
// return false, which forces a re-join.
bool
cMyLoRaWAN::NetGetSessionState(SessionState &State) {
    // we didn't save earlier, so just tell the core we don't have data.
    return false;
}


/****************************************************************************\
|
|	Sensor methods
|
\****************************************************************************/