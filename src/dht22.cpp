#include <Arduino.h>
#include <DHT.h>

class cDHT22 {
    private:
        int port = 0;
        DHT dht = DHT(this->port, DHT22);

    public:
        cDHT22(int pPort) {
            port = pPort;
            dht = DHT(this->port, DHT22);
            dht.begin();
        }

        float getHumidity() {
            return dht.readHumidity();
            
        }

        float getTemperature() {
            return dht.readTemperature();
        }
};