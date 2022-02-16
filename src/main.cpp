#include <Arduino.h>
#include <soil_moisture.cpp>
#include <photo_resistor.cpp>
#include <dht22.cpp>
#include <bmp280.cpp>

void setup() {
    Serial.begin(115200);
    Serial.println("Wir sind wieder da.");
}

void loop() {

    cSoilMoisture mySoilSensor = cSoilMoisture(32);
    cPhotoResistor myPhotoSensor = cPhotoResistor(25);
    cDHT22 myDHT22 = cDHT22(15);
    cBMT280 myBMP280 = cBMT280();

    Serial.println("");
    Serial.println("Soil Moisture: " + String(mySoilSensor.getSensorData()));
    Serial.println("Photo Resistor: " + String(myPhotoSensor.getSensorData()));
    Serial.println("DHT22 Temperature: " + String(myDHT22.getTemperature()));
    Serial.println("DHT22 Humidity: " + String(myDHT22.getHumidity()));
    Serial.println("BMP280 Temperature: " + String(myBMP280.getTemperature()));
    Serial.println("BMP280 Pressure: " + String(myBMP280.getPressure()));
    
    delay(1000);
}