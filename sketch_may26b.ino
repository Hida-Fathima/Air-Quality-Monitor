#include <Wire.h>
#include <LiquidCrystal.h>
#include <Ethernet.h>
#include <ThingSpeak.h>

#define mq_sensor A0
#define buzz 6

int mq_gas, mq_co2lvl;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

byte smiley[8] = {
  0b00000,
  0b01010,
  0b01010,
  0b00000,
  0b00000,
  0b10001,
  0b01110,
  0b00000
};

byte sad[8] = {
  0b00000,
  0b01010,
  0b01010,
  0b00000,
  0b00000,
  0b01110,
  0b10001,
  0b00000
};

const char* ssid = "Ravi_5G";
const char* password = "9742259508";
const char* thingSpeakAddress = "api.thingspeak.com";
const String thingSpeakAPIKey = "LTHIOTK9KDB7E9QP";
const unsigned long channelID = 2163938;

EthernetClient client;


void setup() {
  pinMode(buzz, OUTPUT);
  pinMode(mq_sensor, INPUT);
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("  CO2 Meter   ");
  lcd.setCursor(0, 0);
  delay(1000);
  lcd.clear();
  lcd.print("Warming coil");

  for (int i = 0; i <= 100; i++) {
    lcd.setCursor(12, 0);
    if (i < 100) lcd.print(" ");
    if (i < 10) lcd.print(" ");
    lcd.print(i);
    lcd.print("%");
    delay(700);
  }
  lcd.createChar(0, smiley);
  lcd.createChar(1, sad);

 

  ThingSpeak.begin(client);
}

void loop() {
 mq_gas = analogRead(mq_sensor);
  mq_co2lvl = map(mq_gas - 120, 0, 1024, 400, 5000);

  

  Serial.println(mq_gas);
  lcd.setCursor(0, 0);
  lcd.print("AQI: ");
  lcd.print(mq_co2lvl);
  lcd.print(" ppm");

 
  if ((mq_co2lvl >= 350) && (mq_co2lvl <= 1000))  {
    lcd.print("Good ");
    lcd.write(byte(0));
    digitalWrite(buzz, LOW);
  }
  else if ((mq_co2lvl >= 1000) && (mq_co2lvl <= 2000)) {
    digitalWrite(buzz, HIGH);
    lcd.print("  Bad ");
    lcd.write(byte(1));
  }
  else {
    lcd.print(" Danger!");
    digitalWrite(buzz, HIGH);

  }

  ThingSpeak.setField(1, mq_co2lvl);
  int httpCode = ThingSpeak.writeFields(channelID, thingSpeakAPIKey.c_str());
  if (httpCode == 200) {
    Serial.println("Channel update successful");
  } else {
    Serial.println("Error updating channel. HTTP code: " + String(httpCode));
  }

  delay(300);
lcd.clear();
}
