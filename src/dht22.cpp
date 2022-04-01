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

        float getHumidity_float() {
            return dht.readHumidity();
            
        }

        float getTemperature_float() {
            return dht.readTemperature();
        }

        uint8_t getHumidity_uint8_t() {
            return (uint8_t) round(getHumidity_float());
        }

        uint8_t getTemperature_uint8_t() {
            float i = getTemperature_float()*2;
            int k = map(i, -40, 160, 0, 200);
            return (uint8_t) round(k);
        }
};