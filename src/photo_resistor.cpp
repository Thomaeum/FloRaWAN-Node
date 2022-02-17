#include <Arduino.h>

class cPhotoResistor {
    private:
        int port;

    public:
        cPhotoResistor(int pPort) {
            port = pPort;
        }

        int getSensorData() {
            return analogRead(port);
        }
};