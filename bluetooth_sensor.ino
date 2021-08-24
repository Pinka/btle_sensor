#include <SPI.h>
#include <RF24.h>
#include <BTLE.h>

#include <DHT.h>              // Include DHT library code
#include <LowPower.h>

#define DHTPIN  4             // DHT22 data pin is connected to Arduino pin 2
#define DHTTYPE DHT22         // DHT22 sensor is used

DHT dht(DHTPIN, DHTTYPE);     // Initialize DHT library
RF24 radio(7, 8);
BTLE btle(&radio);

struct sensor_data {
  uint16_t service_uuid;
  uint16_t value;
};

void setup() {

  Serial.begin(9600);
  while (!Serial) { }

  Serial.println("BTLE sensor");

  btle.begin("th");
  dht.begin();
}

void loop() {

  sendSensorData();
  LowPower.idle(SLEEP_500MS, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
}

void sendSensorData() {

  float value;
  nrf_service_data buf;

  btle.preparePacket();

  value = dht.readTemperature();
  sensor_data temp;
  temp.service_uuid = NRF_TEMPERATURE_SERVICE_UUID; //0x1809
  temp.value = value * 100;

  if (!btle.addChunk(0x16, sizeof(temp), &temp)) {
    Serial.println("Temperature level does not fit");
  }

  value = dht.readHumidity();
  sensor_data humidity;
  humidity.service_uuid = 0x2A6F; // Humidity
  humidity.value = value * 100;

  if (!btle.addChunk(0x16, sizeof(humidity), &humidity)) {
    Serial.println("Humidity does not fit");
  }

  btle.transmitPacket();
  btle.hopChannel();

  Serial.println(temp.value);
  Serial.println(humidity.value);
  Serial.println("----------");
}
