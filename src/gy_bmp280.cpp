#include <Arduino.h>
#include <Adafruit_BMP280.h>

class cBMP280 {
    private:
        Adafruit_BMP280 bmp;

    public:
        cBMP280() {
            bmp = Adafruit_BMP280();
            bmp.begin();
            /*if(!bmp.begin()) {
                Serial.println("FEHLER!");
            }*/
        }

        float getPressure() {
            return bmp.readPressure();
        }

        float getTemperature() {
            return bmp.readTemperature();
        }

        float getAltitude() {
            return bmp.readAltitude();
        }
};