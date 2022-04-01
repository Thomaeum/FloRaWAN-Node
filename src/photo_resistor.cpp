#include <Arduino.h>

class cPhotoResistor {
    private:
        int port;

    public:
        cPhotoResistor(int pPort) {
            port = pPort;
        }

        int getSensorData_int() {
            return analogRead(port);
        }

        uint8_t getSensorData_uint8_t() {
            return (uint8_t) map(getSensorData_int(), 4095, 0, 0, 255);
        }
};