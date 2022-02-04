#include <Arduino.h>
#include <DHT.h>

#define DHTTYPE DHT22
#define DHT22_Pin 2

class cDHTSensor {
    private:
        int port = 2;
        DHT dht = DHT(this->port, DHT22);

    public:
        cDHTSensor(int pPort) {
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