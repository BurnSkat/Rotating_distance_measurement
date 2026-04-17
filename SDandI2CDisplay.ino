#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
const int SD_CS_PIN = 10;
bool sdAvailable = false;

void setup() {
//  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2,0);
  lcd.print("Raumvermesser");
  lcd.setCursor(5,1);
  lcd.print("V1.0!");
  delay(3000);
  lcd.clear();
//  lcd.home();
  lcd.print("Waehle Modus:");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);*/
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //pinMode(RECV_PIN,INPUT);
  Serial.begin(9600);
  sdAvailable = SD.begin(SD_CS_PIN);
  if (sdAvailable) {
    Serial.println("SD-Karte bereit");
  } else {
    Serial.println("SD-Karte nicht gefunden");
  }
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    Serial.println("Ready to receive IR signals");
  //updateMenu();
}

void measurement(){ //Void zum manuellen Messen der Entfernung
 IrReceiver.decodedIRData.command=0x0;
 IrReceiver.resume();
 digitalWrite(trigPin, LOW);
 delayMicroseconds(2);
 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10);
 digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration*.0343)/2;
  Serial.print("Distance: ");
  Serial.println(distance);
  if (sdAvailable) {
    File dataFile = SD.open("messung.csv", FILE_WRITE);
    if (dataFile) {
      dataFile.print(millis());
      dataFile.print(';');
      dataFile.println(distance);
      dataFile.close();
    }
  }
