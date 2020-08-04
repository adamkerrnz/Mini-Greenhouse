#include <Wire.h>
#include <DHT.h>
#include <DHT_U.h>
#include <RTClib.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define soilPin A0
#define pumpPin 5
#define soilPower 4

DHT_Unified dht1(DHTPIN, DHTTYPE);
RTC_DS1307 rtc;

int delayMS;
int moistVAL;
void printDateTime(DateTime dateTime);
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup() {
  Serial.begin(9600);
  pinMode(soilPower, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(soilPower, LOW);
  dht1.begin();
  rtc.begin();
  sensor_t sensor;
  dht1.temperature().getSensor(&sensor);
  dht1.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
}

void loop() {
  delay(delayMS);
  //Displays Temp & Humidity values from DHT11
  sensors_event_t event;
  dht1.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.print(F("No Temperature Sensor!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.print(F("°C"));
  }
  dht1.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }

  //Prints soil moisture Values
  Serial.print("Soil Moisture = ");
  Serial.println(moistVAL);

  // Sets the Relay on or off

  if (readSoil() < 100)
    digitalWrite(pumpPin, HIGH);
  else
    digitalWrite(pumpPin, LOW);

  //Prints the RTC Values
  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

}
int readSoil()
{
  digitalWrite(soilPower, HIGH);
  delay(10);
  moistVAL = analogRead(soilPin);
  digitalWrite(soilPower, LOW);
  return moistVAL;
}
