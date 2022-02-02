#include <Arduino_LoRaWAN_ttn.h>

class cMyLoRaWAN : public Arduino_LoRaWAN_ttn {
    public:
        cMyLoRaWAN() {};

    protected:
        // you'll need to provide implementations for this.
        virtual bool GetOtaaProvisioningInfo(Arduino_LoRaWAN::OtaaProvisioningInfo*) override;
        // if you have persistent storage, you can provide implementations for these:
        //virtual void NetSaveSessionInfo(const SessionInfo &Info, const uint8_t *pExtraInfo, size_t nExtraInfo) override;
        //virtual void NetSaveSessionState(const SessionState &State) override;
        //virtual bool NetGetSessionState(SessionState &State) override;
};

//pin mapping
const cMyLoRaWAN::lmic_pinmap myPinMap = {
     .nss = 27,
     .rxtx = cMyLoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN,
     .rst = 33,
     .dio = { 26, 4, 2 },
};

// set up the data structures.
cMyLoRaWAN myLoRaWAN {};

void setup() {
    myLoRaWAN.begin(myPinMap);
}

void loop() {
    myLoRaWAN.loop();
}

// this method is called when the LMIC needs OTAA info.
// return false to indicate "no provisioning", otherwise
// fill in the data and return true.
bool
cMyLoRaWAN::GetOtaaProvisioningInfo(
    OtaaProvisioningInfo *pInfo
    ) {
    return false;
}

/*void
cMyLoRaWAN::NetSaveSessionInfo(
    const SessionInfo &Info,
    const uint8_t *pExtraInfo,
    size_t nExtraInfo
    ) {
    // save Info somewhere.
}

void
cMyLoRaWAN::NetSaveSessionState(const SessionState &State) {
    // save State somwwhere. Note that it's often the same;
    // often only the frame counters change.
}

bool
cMyLoRaWAN::NetGetSessionState(SessionState &State) {
    // either fetch SessionState from somewhere and return true or...
    return false;
}*/