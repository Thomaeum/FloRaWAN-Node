#include <Arduino.h>

class cSoilMoisture {
    private:
        int port;

    public:
        cSoilMoisture(int pPort) {
            port = pPort;
        }
        
        int getSensorData() {
            return analogRead(port);
        }
};