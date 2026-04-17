#include <Arduino.h>
#include <Stepper.h>
#include <IRremote.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>
//----------LCD-Parameter initalisieren & Menü definieren Start----------------------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int SD_CS_PIN = 10;
bool sdAvailable = false;

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

int modestate = 0;
int moduswahl = 0;
int menuPage = 0;
bool mainmenufirsttime=true;
int MessungenManuell[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}; //Array zur Bestimmung der Anzahl an zu machenden Messungen in manuell
int GemessenManuell[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}; //Array zur Orientierung der umgesetzten manuellen Messungen
int MessungenAutomatik[32]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32}; //Array zur Bestimmung der Anzahl an zu machenden Messungen in Automatik
int GemessenAutomatik[32]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32}; //Array zur Orientierung der umgesetzten automatischen Messungen
//const int maxPages = 2;
int AnzahlM;    //Platzhalter zum Bestimmen der anzahl der zu machenden Messungen manuell
int MessungenM; //übernommene, bestimmte Anzahl der zu machenden Messungen manuell
int GemessenM;  //vergleichsvariable für umgesetzte Messungen manuell
int AnzahlA;    //Platzhalter zum Bestimmen der anzahl der zu machenden Messungen automatik
int MessungenA; //übernommene, bestimmte Anzahl der zu machenden Messungen automatik
int GemessenA;  //vergleichsvariable für umgesetzte Messungen automatik
float TabelleM[16];//Array für die temoporäre Speicherung der Messergebnisse im manuellen Modus
float TabelleA[32];//Array für die temoporäre Speicherung der Messergebnisse im automatik Modus
bool initialisieren = false;
float results[16];  // Zwischenspeicherarray für manuelle Ergebnisse
float Aresults[32]; // Zwischenspeicherarray für automatische Ergebnisse
int CursorshiftRow=0;
int index;


//----------LCD initalisieren & Menü definieren Ende ----------------------------------

//----------Steppermotor initalisieren Start----------------------------------

const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 9, 7, 8, 6);
int stepCount = 0;

int SWBU = A1; //Tastersteuerung Eingangspins
int SWWH = A2; 

int shift = 0; //Variablen für Dreharten
int status = 0;
bool turn = false;

//----------Steppermotor initalisieren Ende----------------------------------


//----------IR Pin initalisieren Start----------------------------------
const int IR_RECEIVE_PIN=A3;
//----------IR Pin initalisieren Ende----------------------------------


//----------Variablen für HC-SR04 initalisieren Start----------------------------------
const int trigPin = A5;
const int echoPin = A4;
int SWRD = A0;
bool measure=false;
float duration, distance;
//----------Variablen für HC-SR04 initalisieren Ende----------------------------------


void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2,0);
  lcd.print("Raumvermesser");
  lcd.setCursor(5,1);
  lcd.print("V1.0!");
  delay(3000);
  lcd.clear();
 /* lcd.home();
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

//-----------------------------Custom Voids Start----------------------------------

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
  delay(500);

  if (modestate==5){
    EEPROM.put(TabelleM[GemessenM], distance);
    results[GemessenM]=distance;
    Serial.print(results[GemessenM]);
    GemessenM++;
    manualAngleMenu();
    if (GemessenM==MessungenM+1){
      modestate=7;
    }
  }

  if (modestate==6){
    EEPROM.put(TabelleA[GemessenA], distance);

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

void mainmenu(){
if (mainmenufirsttime==true){
lcd.setCursor(0,0);
lcd.print("Waehle Modus:");
delay(500);
lcd.clear();
mainmenufirsttime=false;
}

lcd.setCursor(0,0);
lcd.print("1.Manuell");
//lcd.autoscroll();
lcd.setCursor(0,1);
lcd.print("2.Automatik");
//delay(500);
//modestate=1;

/*
if(IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0xC){
  modestate=1;
}

if(IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x18){
  modestate=2;
}
*/
}

void showresultsM(){

  lcd.clear();
  lcd.setCursor(0,0);
  GemessenM=0;
  lcd.print (results[GemessenM]);
  GemessenM++;
  lcd.setCursor(0,1);
  lcd.print (results[GemessenM]);
  //GemessenM++;
/* for (GemessenM=0;GemessenM<=MessungenM;GemessenM++){
    //EEPROM.get(TabelleM[GemessenM],results[GemessenM]);
    lcd.print (results[GemessenM]);
    delay(500);
    CursorshiftRow++;
    lcd.setCursor(0,CursorshiftRow);
  }
*/


modestate=9;



}


void manualAngleInit(){
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

void manualAngleMenu(){
      lcd.clear();
      lcd.print("Messung-Nr:");
      lcd.setCursor(0,1);
      lcd.print(GemessenManuell[GemessenM]);
      lcd.print("/");
      lcd.print(MessungenManuell[MessungenM]);
      modestate=5;
}

void autoAngleMenu(){
  modestate=6; //noch führt nichts hierhin
}

void subMenu() {

      if (AnzahlM<0){
        AnzahlM=0;
      }
      if (AnzahlM>15){
        AnzahlM=15;
      }
      if (AnzahlA<0){
        AnzahlA=0;
      }
      if (AnzahlA>31){
        AnzahlA=31;
      }

  switch (menuPage) {
    case 0:
      lcd.clear();
      lcd.print("Anzahl Messungen");
      lcd.setCursor(0,1);
      switch (AnzahlM){
        case 0:
        lcd.setCursor(0,1);
        lcd.print(MessungenManuell[0]);
        break;

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
      }

      break;
    case 1:
      lcd.clear();
      lcd.print("Anzahl Messungen");
      lcd.setCursor(0,1);
           switch (AnzahlA){
        case 0:
        lcd.setCursor(0,1);
        lcd.print(MessungenAutomatik[0]);
        break;

        case 1:
        lcd.setCursor(0,1);
        lcd.print(MessungenAutomatik[1]);
        break;

        case 2:
        lcd.setCursor(0,1);
        lcd.print(MessungenAutomatik[2]);
        break;

        case 3:
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
        lcd.setCursor(0,1);
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

      }
      break;
  }

}
//-----------------------------Custom Voids Ende----------------------------------

void loop() {
if(modestate==0){
mainmenu();
} 

if(modestate==1){
      if (IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x47){
      AnzahlM++;
      lcd.setCursor(0,1);
      subMenu();
      }
      if (IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x45){
      AnzahlM--;
      lcd.setCursor(0,1);
      subMenu();
      }
}

if(modestate==2){
      if (IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x47){
      AnzahlA++;
      lcd.setCursor(0,1);
      subMenu();
      }
      if (IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x45){
      AnzahlA--;
      lcd.setCursor(0,1);
      subMenu();
      }
}

if(IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x46&&modestate==1){
  MessungenM=MessungenManuell[AnzahlM];
 // Serial.print(MessungenM);
  manualAngleInit();
  
  }

if(modestate==3){
manualAngleMenu();
}

if(modestate==7){
  showresultsM();
}

  if (modestate==9&&IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x40){
  //index=GemessenM;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(results[GemessenM]);
  GemessenM++;
  lcd.setCursor(0,1);
  lcd.print(results[GemessenM]);
  //GemessenM++;
  }

  if (modestate==9&&IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x44){
  //index=GemessenM;
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

if(modestate==9&&IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x16){
results[16]=0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;
Aresults[32]=0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;
TabelleM[16]=0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;
TabelleA[32]=0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;
mainmenufirsttime=true;
initialisieren=false;
modestate=0;
}

if(IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x46&&menuPage==1&&modestate==2){
MessungenA=MessungenAutomatik[AnzahlA];
//autoAngleMenu();
}

if(modestate==4){
autoAngleMenu();
}


if(IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0xC){
  menuPage=0;
  modestate=1;
  lcd.clear();
  subMenu();
}

if(IrReceiver.decode()==true&&IrReceiver.decodedIRData.command==0x18){
  menuPage=1;
  modestate=2;
  lcd.clear();
  subMenu();
}



/*
mainmenu();
if (modestate==1){
}

if(modestate==2){
}
*/

if (IrReceiver.decode()) {
Serial.print("Protocol: ");
Serial.println(IrReceiver.decodedIRData.protocol);
Serial.print("Command: 0x");
Serial.println(IrReceiver.decodedIRData.command, HEX);
IrReceiver.resume();// Enable receiving of the next value
  }

 if (analogRead(SWRD)<500||IrReceiver.decodedIRData.command==0x43&&modestate==5){ 
measurement();
 }

if(analogRead(SWBU)<500||IrReceiver.decodedIRData.command==0x40&&status==0&&modestate==5){
turnrightinf();
 }


if(IrReceiver.decodedIRData.command==0x44&&status==0&&modestate==5){
turnleftinf();
}

if(analogRead(SWBU)<500||IrReceiver.decodedIRData.command==0x15&&status==0){
turnrightstep();
 }

if(IrReceiver.decodedIRData.command==0x7&&status==0){
turnleftstep();

}

}
