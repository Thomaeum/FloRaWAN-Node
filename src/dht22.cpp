// DHT11 und DHT22 Auslesen
// https://iotspace.dev/arduino-dht22-auslesen-temperatur-und-luftfeuchtesensor
// Es werden die Software-Bibliotheken "DHT sensor library by Adafruit" und "Adafruit Unified Sensor" benötigt

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"
#define DHTTYPE DHT22
#define DHT22_Pin 2

//BMP280
  #include <Wire.h>
  #include <SPI.h>
  #include <Adafruit_BMP280.h>

  Adafruit_BMP280 bmp; // use I2C interface
  Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
  Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();

//Photoresistor
  const int Hell = 0;
  const int Dunkel = 1024;
  int Lichtintensitaet = 0;
  int Lichtstaerke = 0;

//Bodenfeuchte v1.2 kapazitiv
  const int WaterValue = 50;
  const int AirValue = 420;
  int soilMoistureValue = 0;
  int soilmoisturepercent=0;

DHT dht(DHT22_Pin, DHTTYPE);
float humidity, temperature;

void setup()
{
    Serial.begin(9600);
    dht.begin();
    delay(500);
    
    //BMP280
      while ( !Serial ) delay(100);   // wait for native usb
      Serial.println(F("BMP280 Sensor event test"));

      unsigned status;
      //status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
      status = bmp.begin();
      if (!status) {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                          "try a different address!"));
        Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
      }
      /* Default settings from datasheet. */
      bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                      Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                      Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                      Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                      Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

      bmp_temp->printSensorDetails();
    }

void loop()
{
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    Serial.print("Luftfeuchte: ");
    Serial.print(humidity);
    Serial.println("%");
    Serial.print("Temperatur: ");
    Serial.print(temperature);
    Serial.println("°C");

    //Photoresistor
      Lichtintensitaet = analogRead(A0);
      Lichtstaerke = map(Lichtintensitaet, Dunkel, Hell, 0, 100);
      //Serial.println(Lichtintensitaet);
      Serial.print("Lichtstärke: ");
      Serial.print(Lichtstaerke);
      Serial.println("%");

    //Bodenfeuchte v1.2 kapazitiv
      soilMoistureValue = analogRead(A1);  //der Sensor wird an PIN A0 angeschlossen
      //Serial.println(soilMoistureValue);
      soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
      Serial.print("Bodenfeuchte: ");
      Serial.print(soilmoisturepercent);
      Serial.println("%");

    //BMP280
      sensors_event_t temp_event, pressure_event;
      bmp_temp->getEvent(&temp_event);
      bmp_pressure->getEvent(&pressure_event);
      
      Serial.print(F("Temperature: "));
      Serial.print(temp_event.temperature);
      Serial.println("°C");

      Serial.print(F("Pressure: "));
      Serial.print(pressure_event.pressure);
      Serial.println("hPa");

    Serial.println();
    delay(2000);
}