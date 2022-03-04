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
            return (uint8_t) map(getSensorData_int(), 2500, 450, 0, 100);
        }
};