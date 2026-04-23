#include <Arduino.h>
#include <Stepper.h>
#include <IRremote.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>
//----------LCD-Parameter initalisieren----------------------------------
const uint8_t LCD_I2C_ADDRESS = 0x27; // Haeufige Standardadresse fuer LCD-I2C-Backpacks
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, 16, 2);

/*
byte Logo[8] = { //definieren Sonderzeichen
B00000,
B00000,
B00000,
B00000,
B00000,
B00000,
B00000,
B00000
};
*/
//----------LCD initalisieren Ende ----------------------------------

//----------Menü relevante Parameter, Integer, Floats & Bools definieren Start----------------------------------
int modestate = 0;
//int moduswahl = 0;
int menuPage = 0;
bool mainmenufirsttime=true;
int MessungenManuell[17]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}; //Array zur Bestimmung der Anzahl an zu machenden Messungen in manuell
int GemessenManuell[17]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}; //Array zur Orientierung der umgesetzten manuellen Messungen
int MessungenAutomatik[33]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32}; //Array zur Bestimmung der Anzahl an zu machenden Messungen in Automatik
int GemessenAutomatik[33]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32}; //Array zur Orientierung der umgesetzten automatischen Messungen

int durchzaehlenG = 0;
int durchzaehlenM = 0;

int WinkelM;
int WinkelG;



int WinkelGewaehlt[]={}; //Wegen Programmspeicher begrenzt, eigentlich sollen 360° möglich sein
//int WinkelMoeglich[360];
int AnzahlWinkel;
//const int maxPages = 2;

int AnzahlM;    //Platzhalter zum Bestimmen der anzahl der zu machenden Messungen manuell
int MessungenM; //übernommene, bestimmte Anzahl der zu machenden Messungen manuell
int GemessenM;  //vergleichsvariable für umgesetzte Messungen manuell
int AnzahlA=0;    //Platzhalter zum Bestimmen der anzahl der zu machenden Messungen automatik
int MessungenA; //übernommene, bestimmte Anzahl der zu machenden Messungen automatik
int GemessenA=0;  //vergleichsvariable für umgesetzte Messungen automatik
float TabelleM[16];//Array für die temoporäre Speicherung der Messergebnisse im manuellen Modus
float TabelleA[32];//Array für die temoporäre Speicherung der Messergebnisse im automatik Modus
bool initialisieren = false; // Bool zur Unterscheidung zwischen ersttmaligem Durchlaufen nach Einschalten und erneuter Messung nach einem Durchlauf

//
float results[16];  // Zwischenspeicherarray für manuelle Ergebnisse
float Aresults[32]; // Zwischenspeicherarray für automatische Ergebnisse


int CursorshiftRow=0;

int index; //unbenutzt

//----------Menü definieren Ende ----------------------------------


//----------Steppermotor initalisieren Start----------------------------------

const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 9, 7, 8, 6);
int stepCount = 0;

int StepsForAutomation;
const int StepsTotal=2000;

//int SWBU = A1; //Tastersteuerung Eingangspins
int SWWH = A2; 

int shift = 0; //Variablen für Dreharten
int status = 0;
bool turn = false;

//----------Steppermotor initalisieren Ende----------------------------------


//----------IR Pin initalisieren Start----------------------------------
const int IR_RECEIVE_PIN=A3;
//----------IR Pin initalisieren Ende----------------------------------


//----------Variablen für HC-SR04 initalisieren Start----------------------------------
const int trigPin = A1;
const int echoPin = A0;
bool measure=false;
float duration, distance;
//----------Variablen für HC-SR04 initalisieren Ende----------------------------------

const uint8_t SD_CS_PIN = 10;
const char SD_FILE_NAME[] = "messungen.csv";
bool sdAvailable = false;

void initSDCard();
void logMeasurementToSD(const char* modus, int messungNummer, float distanzCm);



void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2,0);
  lcd.print("Raumvermesser");
  lcd.setCursor(5,1);
  lcd.print("V1.0!");
  delay(3000);
  lcd.clear();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  initSDCard();
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    Serial.println("Ready to receive IR signals");

for (int WinkelGewaehlt[360]; WinkelGewaehlt[durchzaehlenG]<360; durchzaehlenG++){
  WinkelGewaehlt[durchzaehlenG]=durchzaehlenG;
}

for (int WinkelMoeglich[360]; WinkelMoeglich[durchzaehlenM]<360; durchzaehlenM++){
  WinkelMoeglich[durchzaehlenM]=durchzaehlenM;
}

}

//-----------------------------Custom Voids Start----------------------------------

void mainmenu(){ //Modestate==0
if (mainmenufirsttime==true){
lcd.setCursor(0,0);
lcd.print("Waehle Modus:");
delay(500);
lcd.clear();
mainmenufirsttime=false;
}

lcd.setCursor(0,0);
lcd.print("1.Manuell");
lcd.setCursor(0,1);
lcd.print("2.Automatik");

}

void subMenu() { //Anzahl Messungen je nach automatik oder manuellem Modus

      if (AnzahlM<0){
        AnzahlM=0;
      }
      if (AnzahlM>16){
        AnzahlM=16;
      }
      if (AnzahlA<0){
        AnzahlA=0;
      }
      if (AnzahlA>32){
        AnzahlA=32;
      }

  switch (menuPage) {
    case 0:
      lcd.clear();
      lcd.print("Anzahl Messungen");
      lcd.setCursor(0,1);
      switch (AnzahlM){
        /*case 0:
        lcd.setCursor(0,1);
        lcd.print(MessungenManuell[0]);
        break;*/

        case 1:
        lcd.setCursor(0,1);
        lcd.print(MessungenManuell[1]);
        break;

        case 2:
        lcd.setCursor(0,1);
        lcd.print(MessungenManuell[2]);
        break;

        case 3:
        lcd.print(MessungenManuell[3]);
        break;

        case 4:        
        lcd.print(MessungenManuell[4]);
        break;

        case 5:        
        lcd.print(MessungenManuell[5]);
        break;

        case 6:        
        lcd.print(MessungenManuell[6]);
        break;

        case 7:        
        lcd.print(MessungenManuell[7]);
        break;

        case 8:        
        lcd.print(MessungenManuell[8]);
        break;

        case 9:        
        lcd.print(MessungenManuell[9]);
        break;

        case 10:       
        lcd.print(MessungenManuell[10]);
        break;

        case 11:       
        lcd.print(MessungenManuell[11]);
        break;

        case 12:
        lcd.print(MessungenManuell[12]);
        break;

        case 13:
        lcd.print(MessungenManuell[13]);
        break;

        case 14:
        lcd.print(MessungenManuell[14]);
        break;

        case 15:
        lcd.print(MessungenManuell[15]);
        break;

        case 16:
        lcd.print(MessungenManuell[16]);
        break;
      }

      break;
    case 1:
      lcd.clear();
      lcd.print("Anzahl Messungen");
      lcd.setCursor(0,1);
        switch (AnzahlA){
        
        case 1:
        lcd.setCursor(0,1);
        lcd.print(MessungenAutomatik[1]);
        break;

        case 2:
        lcd.setCursor(0,1);
        lcd.print(MessungenAutomatik[2]);
        break;

        case 3:
        lcd.setCursor(0,1);
        lcd.print(MessungenAutomatik[3]);
        break;

        case 4:
        lcd.print(MessungenAutomatik[4]);
        break;

        case 5:        
        lcd.print(MessungenAutomatik[5]);
        break;

        case 6:        
        lcd.print(MessungenAutomatik[6]);
        break;

        case 7:        
        lcd.print(MessungenAutomatik[7]);
        break;

        case 8:        
        lcd.print(MessungenAutomatik[8]);
        break;

        case 9:        
        lcd.print(MessungenAutomatik[9]);
        break;

        case 10:        
        lcd.print(MessungenAutomatik[10]);
        break;

        case 11:       
        lcd.print(MessungenAutomatik[11]);
        break;

        case 12:       
        lcd.print(MessungenAutomatik[12]);
        break;

        case 13:
        lcd.print(MessungenAutomatik[13]);
        break;

        case 14:
        lcd.print(MessungenAutomatik[14]);
        break;

        case 15:
        lcd.print(MessungenAutomatik[15]);
        break;

        case 16:
        lcd.print(MessungenAutomatik[16]);
        break;

        case 17:
        lcd.setCursor(0,1);
        lcd.print(MessungenAutomatik[17]);
        break;

        case 18:
        lcd.setCursor(0,1);
        lcd.print(MessungenAutomatik[18]);
        break;

        case 19:
        lcd.setCursor(0,1);
        lcd.print(MessungenAutomatik[19]);
        break;

        case 20:
        lcd.print(MessungenAutomatik[20]);
        break;

        case 21:        
        lcd.print(MessungenAutomatik[21]);
        break;

        case 22:        
        lcd.print(MessungenAutomatik[22]);
        break;

        case 23:        
        lcd.print(MessungenAutomatik[23]);
        break;

        case 24:        
        lcd.print(MessungenAutomatik[24]);
        break;

        case 25:        
        lcd.print(MessungenAutomatik[25]);
        break;

        case 26:        
        lcd.print(MessungenAutomatik[26]);
        break;

        case 27:       
        lcd.print(MessungenAutomatik[27]);
        break;

        case 28:       
        lcd.print(MessungenAutomatik[28]);
        break;

        case 29:
        lcd.print(MessungenAutomatik[29]);
        break;

        case 30:
        lcd.print(MessungenAutomatik[30]);
        break;

        case 31:
        lcd.print(MessungenAutomatik[31]);
        break;

        case 32:
        lcd.print(MessungenAutomatik[32]);
        break;

      }
      break;
  }

}

//-----------------------------------------MANUELLES MENÜ CUSTOM VOIDS--------------------------------------------------------------

void manualAngleInit(){ //Winkelinitialisierung vor der ersten manuellen Messung - Motor fährt linksrum bis weisser Taster betätigt wird
  Serial.print(analogRead(SWWH));
 while(analogRead(SWWH)<500){
  myStepper.step(-1);
  Serial.print("steps:");
  Serial.println(stepCount);
  stepCount--;
  delay(10);
}
if(analogRead(SWWH)>500){
  initialisieren = true;
}

  if(initialisieren==true){
  modestate=3;
  }
}

void turnrightinf(){ //Void zum manuellen Drehen des Steppers im Uhrzeigersinn bis man stoppt
 myStepper.step(1);
  Serial.print("steps:");
  Serial.println(stepCount);
  stepCount++;
  turn=true;
  delay(10);

if(IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x40&&turn==true){
  status=1;
  delay(20);
}
if(turn==true&&status==1){
  delay(100);
  status=0;
  turn=false;
  IrReceiver.decodedIRData.command=0x0;
  IrReceiver.resume();
  delay(20);
}
}

void turnleftinf(){ //Void zum manuellen Drehen des Steppers gegen den Uhrzeigersinn bis man stoppt
myStepper.step(-1);
  Serial.print("steps:");
  Serial.println(stepCount);
  stepCount--;
  turn=true;
  delay(10);

if(IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x44&&turn==true){
  status=1;
  delay(20);
}
if(turn==true&&status==1){
  delay(100);
  status=0;
  turn=false;
  IrReceiver.decodedIRData.command=0x0;
  IrReceiver.resume();
  delay(20);
}
}

void turnrightstep(){ //Void zum manuellen Drehen des Steppers im Uhrzeigersinn für 200 Schritte
 myStepper.step(1);
  Serial.print("steps:");
  Serial.println(stepCount);
  stepCount++;
  shift++;
  turn=true;
  delay(10);

if(shift==200&&turn==true){
  shift=0;
  status=1;
}
if(status==1&&turn==true){
  IrReceiver.decodedIRData.command=0x0;
  IrReceiver.resume();
  turn=false;
  status=0;
}

}

void turnleftstep(){ //Void zum manuellen Drehen des Steppers gegen den Uhrzeigersinn für 200 Schritte
  myStepper.step(-1);
  Serial.print("steps:");
  Serial.println(stepCount);
  stepCount--;
  shift--;
  turn=true;
  delay(10);

if(shift==-200&&turn==true){
  shift=0;
  status=1;
}

if(status==1&&turn==true){
  IrReceiver.decodedIRData.command=0x0;
  IrReceiver.resume();
  turn=false;
  status=0;
}

}

void manualAngleMenu(){ //Manuelle Winkeleinstellung durch obere Voids, Anzahl der durchgeführten Messungen wird mitgezählt und angezeigt
      lcd.clear();
      lcd.print("Messung-Nr:");
      lcd.setCursor(0,1);
      lcd.print(GemessenManuell[GemessenM]);
      lcd.print("/");
      lcd.print(MessungenManuell[MessungenM]);
      modestate=5;
}

void showresultsM(){//Präsentiert die Ergebnisse indem es diese aus dem Array 'results' abruft, auf welches diese zuvor beim Measurement Void gelegt wurden

  lcd.clear();
  lcd.setCursor(0,0);
  GemessenM=0;
  lcd.print (results[GemessenM]);
  GemessenM++;
  lcd.setCursor(0,1);
  lcd.print (results[GemessenM]);

modestate=9;

}

//-----------------------------------------MANUELLES MENÜ CUSTOM VOIDS ENDE--------------------------------------------------------------


//-----------------------------------------MESSUNGS VOIDS FÜR BEIDE MODI --------------------------------------------------------------

void measurement(){ //Void zum Messen der Entfernung, Verzweigung am Ende je nach gewähltem Modus (Abhängig vom Vorhandenen Modestate Während Aufruf von Measurement Void)
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
  delay(500);

  if (modestate==5){ //Weiterleitung der Ergebnisse an die zuständige Tabelle für manuelle Messungen (bei Modestate 5)
    EEPROM.put(TabelleM[GemessenM], distance);
    results[GemessenM]=distance;
    Serial.print(results[GemessenM]);
    logMeasurementToSD("manuell", GemessenM + 1, distance);
    GemessenM++;
    manualAngleMenu();
    if (GemessenM==MessungenM){
      modestate=7;
    }
  }

  if (modestate==10){ //Weiterleitung der Ergebnisse an die zuständige Tabelle für automatische Messungen (bei Modestate 10)
    EEPROM.put(TabelleA[GemessenA], distance);
    Aresults[GemessenA]=distance;
    Serial.print(Aresults[GemessenA]);
    logMeasurementToSD("automatik", GemessenA + 1, distance);
    GemessenA++;
    if (GemessenA==MessungenA){
      modestate=12;
    }
  }

}

//-----------------------------------------MESSUNGS VOIDS FÜR BEIDE MODI ENDE--------------------------------------------------------------

//-----------------------------------------AUTOMATISCHES MENÜ CUSTOM VOIDS--------------------------------------------------------------

void autoAngleInit(){ //Winkelinitialisierung vor der ersten automatischen Messung - Motor fährt bis weisser Taster betätigt wird
  Serial.print(analogRead(SWWH));
 while(analogRead(SWWH)<500){
  myStepper.step(-1);
  Serial.print("steps:");
  Serial.println(stepCount);
  stepCount--;
  delay(10);
}
if(analogRead(SWWH)>500){
  initialisieren = true;
}

  if(initialisieren==true){
  modestate=4;
  }

}

void autoAngleMenu(){ //Auswahl der Größe des Kreises zwischen 0° und 360°
      WinkelM=360;
      lcd.clear();
      lcd.print("Winkelgroesse:");
      lcd.setCursor(0,1);
      lcd.print(WinkelG);
      lcd.print("/");
      lcd.print(WinkelM);
  
  modestate=6;
}

void RotationDirection(){
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Drehrichtung:");
lcd.setCursor(0,1);
lcd.print("1. L / 2. R");
modestate=10;

}

void turnleftAuto(){ //Void zum automatischen Drehen des Steppers gegen den Uhrzeigersinn und automatischer Ausführung der Messungen - im Anschluss Weiterleitung zum Präsentieren der Ergebnisse
  while(GemessenA<MessungenA){
  while(StepsForAutomation>=((-1*(WinkelG/MessungenA)*(StepsTotal/WinkelM)))){
  myStepper.step(-1);
  Serial.print("steps:");
  Serial.println(stepCount);
  stepCount--;
  StepsForAutomation--;
  delay(10);
  }
  measurement();
  StepsForAutomation=0;
  //GemessenA++;
  }
 showresultsA();
}

void turnrightAuto(){ //Void zum automatischen Drehen des Steppers im Uhrzeigersinn und automatischer Ausführung der Messungen - im Anschluss Weiterleitung zum Präsentieren der Ergebnisse
 while(GemessenA<MessungenA){
  while(StepsForAutomation<((WinkelG/MessungenA)*(StepsTotal/WinkelM))){
  myStepper.step(1);
  Serial.print("steps:");
  Serial.println(stepCount);
  stepCount++;
  StepsForAutomation++;
  delay(10);
  }
  measurement();
  StepsForAutomation=0;
  //GemessenA++;
  }
  showresultsA();
}

void showresultsA(){//Präsentiert die Ergebnisse indem es diese aus dem Array 'Aresults' abruft, auf welches diese zuvor beim Measurement Void gelegt wurden

  lcd.clear();
  lcd.setCursor(0,0);
  GemessenA=0;
  lcd.print (Aresults[GemessenA]);
  GemessenA++;
  lcd.setCursor(0,1);
  lcd.print (Aresults[GemessenA]);

modestate=14;

}

//-----------------------------------------AUTOMATISCHES MENÜ CUSTOM VOIDS ENDE--------------------------------------------------------------


void initSDCard(){
  if (!SD.begin(SD_CS_PIN)) {
    sdAvailable = false;
    Serial.println("SD card init failed");
    return;
  }

  sdAvailable = true;
  if (!SD.exists(SD_FILE_NAME)) {
    File csvFile = SD.open(SD_FILE_NAME, FILE_WRITE);
    if (csvFile) {
      csvFile.println("modus,messung,distanz_cm,steps,millis");
      csvFile.close();
    }
  }
}

void logMeasurementToSD(const char* modus, int messungNummer, float distanzCm){
  if (!sdAvailable) {
    return;
  }

  File csvFile = SD.open(SD_FILE_NAME, FILE_WRITE);
  if (!csvFile) {
    sdAvailable = false;
    Serial.println("SD write failed");
    return;
  }

  csvFile.print(modus);
  csvFile.print(',');
  csvFile.print(messungNummer);
  csvFile.print(',');
  csvFile.print(distanzCm, 2);
  csvFile.print(',');
  csvFile.print(stepCount);
  csvFile.print(',');
  csvFile.println(millis());
  csvFile.close();
}

//-----------------------------Custom Voids Ende----------------------------------

void loop() {
if(modestate==0){
mainmenu();
} 

if(modestate==0&&IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0xC){
    IrReceiver.decodedIRData.command=0x0;
    IrReceiver.resume();
  menuPage=0;
  modestate=1;
  lcd.clear();
  subMenu();
}

if(modestate==0&&IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x18){
    IrReceiver.decodedIRData.command=0x0;
    IrReceiver.resume();
  menuPage=1;
  modestate=2;
  lcd.clear();
  subMenu();
}

if(modestate==1){
      if (IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x47){
        IrReceiver.decodedIRData.command=0x0;
        IrReceiver.resume();
      AnzahlM++;
      lcd.setCursor(0,1);
      subMenu();
      }
      if (IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x45){
        IrReceiver.decodedIRData.command=0x0;
        IrReceiver.resume();
      AnzahlM--;
      lcd.setCursor(0,1);
      subMenu();
      }
}

if(IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x46&&modestate==1){
    IrReceiver.decodedIRData.command=0x0;
    IrReceiver.resume();
  MessungenM=MessungenManuell[AnzahlM];
 // Serial.print(MessungenM);
  manualAngleInit();
  
  }

if(modestate==2){
      if (IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x47){
        IrReceiver.decodedIRData.command=0x0;
        IrReceiver.resume();
      AnzahlA++;
      lcd.setCursor(0,1);
      subMenu();
      }
      if (IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x45){
        IrReceiver.decodedIRData.command=0x0;
        IrReceiver.resume();
      AnzahlA--;
      lcd.setCursor(0,1);
      subMenu();
      }
}

if(IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x46&&menuPage==1&&modestate==2){
MessungenA=MessungenAutomatik[AnzahlA];
 IrReceiver.decodedIRData.command==0x0;
 IrReceiver.resume();
  autoAngleInit();
//autoAngleMenu();
}

/*if(IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x46&&menuPage==0&&modestate==2){
  MessungenA=MessungenManuell[AnzahlA];
 // Serial.print(MessungenM);
 IrReceiver.decodedIRData.command==0x00;
  autoAngleInit();
  }*/

if(modestate==3){
manualAngleMenu();
}

if(modestate==4){
autoAngleMenu();
}

if (IrReceiver.decodedIRData.command==0x43&&modestate==5){ 
  IrReceiver.decodedIRData.command=0x0;
  IrReceiver.resume();
measurement();
 }

if(IrReceiver.decodedIRData.command==0x40&&status==0&&modestate==5){
  IrReceiver.decodedIRData.command=0x0;
  IrReceiver.resume();
turnrightinf();
 }


if(IrReceiver.decodedIRData.command==0x44&&status==0&&modestate==5){
  IrReceiver.decodedIRData.command=0x0;
  IrReceiver.resume();
turnleftinf();
}

if(IrReceiver.decodedIRData.command==0x15&&status==0&&modestate==5){
  IrReceiver.decodedIRData.command=0x0;
  IrReceiver.resume();
turnrightstep();
 }

if(IrReceiver.decodedIRData.command==0x7&&status==0&&modestate==5){
  IrReceiver.decodedIRData.command=0x0;
  IrReceiver.resume();
turnleftstep();

}

if(modestate==6){
      if (IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x47){
        IrReceiver.decodedIRData.command=0x0;
        IrReceiver.resume();
      WinkelG++;
      lcd.setCursor(0,1);
      autoAngleMenu();
      }
      if (IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x45){
        IrReceiver.decodedIRData.command=0x0;
        IrReceiver.resume();
      WinkelG--;
      lcd.setCursor(0,1);
      autoAngleMenu();
      }

      if (IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x40){
        IrReceiver.decodedIRData.command=0x0;
        IrReceiver.resume();
      WinkelG=WinkelG+90;
      lcd.setCursor(0,1);
      autoAngleMenu();
      }
      if (IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x44){
        IrReceiver.decodedIRData.command=0x0;
        IrReceiver.resume();
      WinkelG=WinkelG-90;
      lcd.setCursor(0,1);
      autoAngleMenu();
      }

      if (WinkelG>359){
    WinkelG=360;
  }
  if (WinkelG<0){
    WinkelG=0;
  }
}

if(modestate==6&&IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x9){
  IrReceiver.decodedIRData.command=0x0;
  IrReceiver.resume();
WinkelGewaehlt[AnzahlWinkel]=WinkelG;
modestate=8;
}

if(modestate==7){
  showresultsM();
}

if(modestate==8){
RotationDirection();

}

  if (modestate==9&&IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x40){
  IrReceiver.decodedIRData.command=0x0;
  IrReceiver.resume();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(results[GemessenM]);
  GemessenM++;
  lcd.setCursor(0,1);
  lcd.print(results[GemessenM]);
  //GemessenM++;
  }

  if (modestate==9&&IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x44){
  IrReceiver.decodedIRData.command=0x0;
  IrReceiver.resume();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(results[GemessenM-1]);
  lcd.setCursor(0,1);
  lcd.print(results[GemessenM]);
  GemessenM--;
  }

  if (GemessenM>MessungenM){
    GemessenM=MessungenM;
  }
  if (GemessenM<0){
    GemessenM=0;
  }

if (modestate==10&&IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0xC){
  IrReceiver.decodedIRData.command=0x0;
  IrReceiver.resume();

turnleftAuto();
//modestate=12;
//GemessenA=0;
StepsForAutomation=0;
}
if (modestate==10&&IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x18){
  IrReceiver.decodedIRData.command=0x0;
  IrReceiver.resume();

turnrightAuto();
//modestate=12;
//GemessenA=0;
StepsForAutomation=0;
}

  if (modestate==14&&IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x40){
  IrReceiver.decodedIRData.command=0x0;
  IrReceiver.resume();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(Aresults[GemessenA]);
  GemessenA++;
  lcd.setCursor(0,1);
  lcd.print(Aresults[GemessenA]);
  //GemessenM++;
  }

  if (modestate==14&&IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x44){
  IrReceiver.decodedIRData.command=0x0;
  IrReceiver.resume();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(Aresults[GemessenA-1]);
  lcd.setCursor(0,1);
  lcd.print(Aresults[GemessenA]);
  GemessenA--;
  }

  if (GemessenA>MessungenA){
    GemessenA=MessungenA;
  }
  if (GemessenA<0){
    GemessenA=0;
  }

if((modestate==9||modestate==14)&&IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x16){ //führt nach vollständigem Menüdurchlauf auf das Startmenü zurück bei IR-Eingabe Taste NULL
  IrReceiver.decodedIRData.command=0x0;
  IrReceiver.resume();
results[16]=0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;
Aresults[32]=0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;
TabelleM[16]=0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;
TabelleA[32]=0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;
mainmenufirsttime=true;
initialisieren=false;
modestate=0;
}

if (IrReceiver.decode()) { //Befehle zur Ausgabe im seriellen Monitor von relevanten Daten zum Überwachen des Codes während der Programmierung
Serial.print("Protocol: ");
Serial.println(IrReceiver.decodedIRData.protocol);
Serial.print("Command: 0x");
Serial.println(IrReceiver.decodedIRData.command, HEX);
IrReceiver.resume();// Enable receiving of the next value
Serial.println(modestate);
Serial.println(((WinkelG/MessungenA)*(StepsTotal/WinkelM)));
  } 

}
