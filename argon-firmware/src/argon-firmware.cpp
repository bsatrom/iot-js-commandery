/*
 * Project argon-firmware
 * Description: Firmware for my talk "Bending the IoT To Your Will With JavaScript"
 * Author: Brandon Satrom <brandon@particle.io>
 * Date: 09/23/20191
 */
#include "Particle.h"
#include "Air_Quality_Sensor.h"
#include "Adafruit_BME280.h"
#include "SeeedOLED.h"
#include "Grove_ChainableLED.h"
#include "JsonParserGeneratorRK.h"
#include "DiagnosticsHelperRK.h"

#define AQS_PIN A2
#define SENSOR_READING_INTERVAL 30000

AirQualitySensor aqSensor(AQS_PIN);
Adafruit_BME280 bme;
ChainableLED leds(A4, A5, 1);

SYSTEM_THREAD(ENABLED);

unsigned long lastInterval;
int temp, pressure, humidity;
String airQuality;

// Private battery and power service UUID
const BleUuid serviceUuid("5c1b9a0d-b5be-4a40-8f7a-66b36d0a5176");

BleCharacteristic uptimeCharacteristic("uptime", BleCharacteristicProperty::NOTIFY, BleUuid("fdcf4a3f-3fed-4ed2-84e6-04bbb9ae04d4"), serviceUuid);
BleCharacteristic signalStrengthCharacteristic("strength", BleCharacteristicProperty::NOTIFY, BleUuid("cc97c20c-5822-4800-ade5-1f661d2133ee"), serviceUuid);
BleCharacteristic freeMemoryCharacteristic("freeMemory", BleCharacteristicProperty::NOTIFY, BleUuid("d2b26bf3-9792-42fc-9e8a-41f6107df04c"), serviceUuid);
BleCharacteristic tempCharacteristic("temp", BleCharacteristicProperty::NOTIFY, BleUuid("8692609a-27ce-4b35-bfc6-b6e3cd8fc5b8"), serviceUuid);
BleCharacteristic pressureCharacteristic("pressure", BleCharacteristicProperty::NOTIFY, BleUuid("13b7bf7e-6382-4c93-a2e9-922e04d83bc4"), serviceUuid);
BleCharacteristic humidityCharacteristic("humidity", BleCharacteristicProperty::NOTIFY, BleUuid("16f99aec-1fd3-4aa9-86f6-836028324711"), serviceUuid);
BleCharacteristic airQualityCharacteristic("air-quality", BleCharacteristicProperty::NOTIFY, BleUuid("5bfd8a03-2a5b-443d-b873-8ca6fc558134"), serviceUuid);

int toggleLed(String args);
int setText(String args);
int getBMEValues();
String getAirQuality();
void createEventPayload();
void updateDisplay();
void configureBLE();
void updateBLEValues();

void setup()
{
  Serial.begin(9600);

  if (aqSensor.init())
  {
    Serial.println("Air Quality Sensor ready.");
  }
  else
  {
    Serial.println("Air Quality Sensor ERROR!");
  }

  if (bme.begin())
  {
    Serial.println("BME280 Sensor ready.");
  }
  else
  {
    Serial.println("BME280 Sensor ERROR!");
  }

  leds.init();
  leds.setColorHSB(0, 0.0, 0.0, 0.0);

  Wire.begin();
  SeeedOled.init();

  SeeedOled.clearDisplay();
  SeeedOled.setNormalDisplay();
  SeeedOled.setPageMode();

  SeeedOled.setTextXY(2, 0);
  SeeedOled.putString("Particle");
  SeeedOled.setTextXY(3, 0);
  SeeedOled.putString("Dock Demo");
  SeeedOled.setTextXY(4, 0);
  SeeedOled.putString("v1.0");

  Particle.function("toggleLed", toggleLed);
  Particle.function("setText", setText);

  Particle.variable("temp", temp);
  Particle.variable("pressure", pressure);
  Particle.variable("humidity", humidity);
  Particle.variable("airQuality", airQuality);

  configureBLE();

  lastInterval = millis();
}

void loop()
{
  if ((millis() - lastInterval) > SENSOR_READING_INTERVAL)
  {
    airQuality = getAirQuality();
    Serial.printlnf("Air Quality: %s", airQuality.c_str());

    getBMEValues();
    Serial.printlnf("Temp: %d", temp);
    Serial.printlnf("Pressure: %d", pressure);
    Serial.printlnf("Humidity: %d", humidity);

    updateDisplay();
    createEventPayload();

    lastInterval = millis();
  }

  if (BLE.connected())
  {
    updateBLEValues();
  }
}

String getAirQuality()
{
  int quality = aqSensor.slope();
  String qual = "None";

  if (quality == AirQualitySensor::FORCE_SIGNAL)
  {
    qual = "Danger";
  }
  else if (quality == AirQualitySensor::HIGH_POLLUTION)
  {
    qual = "High Pollution";
  }
  else if (quality == AirQualitySensor::LOW_POLLUTION)
  {
    qual = "Low Pollution";
  }
  else if (quality == AirQualitySensor::FRESH_AIR)
  {
    qual = "Fresh Air";
  }

  return qual;
}

int getBMEValues()
{
  temp = (int)bme.readTemperature();
  pressure = (int)(bme.readPressure() / 100.0F);
  humidity = (int)bme.readHumidity();

  return 1;
}

void createEventPayload()
{
  JsonWriterStatic<256> jw;
  {
    JsonWriterAutoObject obj(&jw);

    jw.insertKeyValue("temp", temp);
    jw.insertKeyValue("humidity", humidity);
    jw.insertKeyValue("pressure", pressure);
    jw.insertKeyValue("air-quality", airQuality);
  }

  Particle.publish("sensor-vals", jw.getBuffer(), PRIVATE);
}

void updateDisplay()
{
  SeeedOled.clearDisplay();

  SeeedOled.setTextXY(0, 3);
  SeeedOled.putString(airQuality);

  SeeedOled.setTextXY(2, 0);
  SeeedOled.putString("Temp: ");
  SeeedOled.putNumber(temp);
  SeeedOled.putString("C");

  SeeedOled.setTextXY(3, 0);
  SeeedOled.putString("Humidity: ");
  SeeedOled.putNumber(humidity);
  SeeedOled.putString("%");

  SeeedOled.setTextXY(4, 0);
  SeeedOled.putString("Press: ");
  SeeedOled.putNumber(pressure);
  SeeedOled.putString(" hPa");
}

int setText(String args)
{
  SeeedOled.clearDisplay();

  Serial.printlnf("Text: %s", args.c_str());

  SeeedOled.setTextXY(3, 2);
  SeeedOled.putString(args.c_str());

  return 1;
}

int toggleLed(String args)
{
  float hue = constrain(args.toFloat(), 0.0, 360.0);
  hue = map(hue, 0.0, 360.0, 0.0, 1.0);

  Serial.printlnf("Hue: %f", hue);

  leds.setColorHSB(0, hue, 1.0, 0.5);

  delay(3000);

  leds.setColorHSB(0, 0.0, 0.0, 0.0);

  return 1;
}

void configureBLE()
{
  BLE.addCharacteristic(uptimeCharacteristic);
  BLE.addCharacteristic(signalStrengthCharacteristic);
  BLE.addCharacteristic(freeMemoryCharacteristic);
  BLE.addCharacteristic(tempCharacteristic);
  BLE.addCharacteristic(pressureCharacteristic);
  BLE.addCharacteristic(humidityCharacteristic);
  BLE.addCharacteristic(airQualityCharacteristic);

  BleAdvertisingData advData;

  advData.appendServiceUUID(serviceUuid);

  BLE.advertise(&advData);
}

void updateBLEValues()
{
  uint8_t uptime = (uint8_t)DiagnosticsHelper::getValue(DIAG_ID_SYSTEM_UPTIME);
  uptimeCharacteristic.setValue(uptime);

  uint8_t signalStrength = (uint8_t)(DiagnosticsHelper::getValue(DIAG_ID_NETWORK_SIGNAL_STRENGTH) >> 8);
  signalStrengthCharacteristic.setValue(signalStrength);

  int32_t usedRAM = DiagnosticsHelper::getValue(DIAG_ID_SYSTEM_USED_RAM);
  int32_t totalRAM = DiagnosticsHelper::getValue(DIAG_ID_SYSTEM_TOTAL_RAM);
  int32_t freeMem = (totalRAM - usedRAM);
  freeMemoryCharacteristic.setValue(freeMem);

  tempCharacteristic.setValue(temp);
  pressureCharacteristic.setValue(pressure);
  humidityCharacteristic.setValue(humidity);
  airQualityCharacteristic.setValue(airQuality);
}