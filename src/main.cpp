#include <Arduino.h>
#include <soil_moisture.cpp>
#include <photo_resistor.cpp>
#include <dht22.cpp>

cSoilMoisture mySoilSensor = cSoilMoisture(A1);
cPhotoResistor myPhotoSensor = cPhotoResistor(A0);
cDHTSensor myDHT = cDHTSensor(2);

void setup() {
    Serial.begin(115200);
    Serial.println("Wir sind wieder da.");
}

void loop() {

    Serial.println("");
    Serial.println(mySoilSensor.getSensorData());
    Serial.println(myPhotoSensor.getSensorData());
    Serial.println(myDHT.getTemperature());
    Serial.println(myDHT.getHumidity());
    delay(1000);
}