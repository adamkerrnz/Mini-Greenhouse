#include <Wire.h>
#include <DHT.h>
#include <DHT_U.h>
#include <RTClib.h>
#include <LiquidCrystal.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define soilPin A1
#define pumpPin 5
#define soilPower 4

DHT_Unified dht1(DHTPIN, DHTTYPE);
RTC_DS1307 rtc;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int delayMS;
int moistVAL;
void printDateTime(DateTime dateTime);
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);

  if (! rtc.begin()) {
    lcd.println("Couldn't find RTC");
    lcd.flush();
    abort();
  }

  if (! rtc.isrunning()) {
    lcd.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }


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
  lcd.flush();
  delay(delayMS);
  // Sets the Relay on or off
  if (readSoil() < 100)
    digitalWrite(pumpPin, HIGH);
  else
    digitalWrite(pumpPin, LOW);

  displayLCD(); //calls the displayLCD fuction for showing information
  printSoilStatus();

}

int readSoil()
{
  digitalWrite(soilPower, HIGH);
  delay(10);
  moistVAL = analogRead(soilPin);
  digitalWrite(soilPower, LOW);
  return moistVAL;
}

void displayLCD()
{
  lcd.setCursor(0, 0);
  DateTime now = rtc.now();
  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
  lcd.print(" ");
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
  const int AirValue = 520;
  const int WaterValue = 260;
  int intervals = (AirValue - WaterValue) / 3;
  int soilMoistureValue = 0;
  soilMoistureValue = analogRead(soilPin);
  
  if (soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals))
  {
    lcd.setCursor(0, 1);
    lcd.print("Very Wet");
  }
  else if (soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))
  {
    lcd.setCursor(0, 1);
    lcd.print("Wet");
  }
  else if (soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))
  {
    lcd.setCursor(0, 1);
    lcd.print("Dry");
  }
  delay(100);
}
