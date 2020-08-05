#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>

#define DHTPIN 0
#define DHTTYPE    DHT11
#define soilPin A1
#define pumpPin 1
#define soilPower 4

DHT_Unified dht(DHTPIN, DHTTYPE);
RTC_DS1307 rtc;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int delayMS;
int moistVAL;
void printDateTime(DateTime dateTime);
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
  rtc.begin();

  if (! rtc.begin()) {
    lcd.println("Couldn't find RTC");
    lcd.flush();
    abort();
  }

  if (! rtc.isrunning()) {
    lcd.println("RTC is NOT running, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  }

  pinMode(soilPower, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(soilPower, LOW);
  digitalWrite(pumpPin, LOW);

}

void loop() {
  delay(delayMS);
  displayLCDTime();
  printSoilStatus();
  getTempHum();

 
}

int readSoil()
{
  digitalWrite(soilPower, HIGH);
  delay(10);
  moistVAL = analogRead(soilPin);
  digitalWrite(soilPower, LOW);
  return moistVAL;
}

void displayLCDTime()
{
  lcd.setCursor(0, 0);
  DateTime now = rtc.now();
  printDigits(now.hour());
  lcd.print(":");
  printDigits(now.minute());
}

void printDigits(byte digits)
{
  // utility function for digital clock display: prints colon and leading 0
  if (digits < 10)
    lcd.print('0');
  lcd.print(digits, DEC);
}

void printSoilStatus()
{
  const int AirValue = 533;
  const int WaterValue = 270;
  int intervals = (AirValue - WaterValue) / 3;
  int soilMoistureValue = 0;
  soilMoistureValue = analogRead(soilPin);
  lcd.setCursor(0, 1);
  if (soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals))
  {
    lcd.print("Very Wet");
    digitalWrite(pumpPin, LOW);
  }
  else if (soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))
  {
    lcd.print("Wet     ");
    digitalWrite(pumpPin, LOW);
  }
  else if (soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))
  {
    lcd.print("Dry     ");
    digitalWrite(pumpPin, HIGH);
  }
  delay(100);
}

void getTempHum()
{
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  lcd.setCursor(9, 0);
  lcd.print("T");
  lcd.print(event.temperature);
  lcd.print("C");
  dht.humidity().getEvent(&event);
  lcd.setCursor(9,1);
  lcd.print("H");
  lcd.print(event.relative_humidity);
  lcd.print("%");
  
}
