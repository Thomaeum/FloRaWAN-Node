#include <Arduino.h>

class cSoilMoisture {
    private:
        int port;

    public:
        cSoilMoisture(int pPort) {
            port = pPort;
        }
        
        int getSensorData_int() {
            return analogRead(port);
        }

        uint8_t getSensorData_uint8_t() {
            //test-device
            return (uint8_t) map(getSensorData_int(), 2500, 200, 0, 255);
            //test-device2
            //return (uint8_t) map(getSensorData_int(), 2600, 150, 0, 255);
            //test-device3
            //return (uint8_t) map(getSensorData_int(), 2300, 340, 0, 255);
        }
};