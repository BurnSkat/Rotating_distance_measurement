<<<<<<< codex/replace-liquidcrystal-with-liquidcrystal_i2c-and-add-sd-modu-dvygfz
# Rotating Distance Measurement

<img width="3745" height="3024" alt="Aufbau" src="https://github.com/user-attachments/assets/8973ef26-85bf-4b99-a203-b4113d0161e9" />

## Projektüberblick
Dieses Projekt misst Distanzen in einem Raum mit einem **HC-SR04 Ultraschallsensor**, der auf einem **Steppermotor** montiert ist. Die Bedienung erfolgt über eine **IR-Fernbedienung**. Messwerte werden auf einem **16x2 I2C-LCD** angezeigt und zusätzlich auf eine **SD-Karte** geschrieben.

Ziel ist eine drehbare 360°-Messplattform mit manuellem und automatischem Modus.

## Hauptfunktionen
- **Manueller Modus**
  - Sensor wird per IR-Befehlen gedreht.
  - Einzelmessungen werden ausgelöst und angezeigt.
- **Automatik-Modus**
  - Vordefinierte Anzahl von Messpunkten über 360°.
- **LCD-Ausgabe (I2C)**
  - Menüführung und Messwerte auf 16x2 Display.
- **SD-Logging**
  - Jeder Messwert wird in `messung.csv` abgelegt.
  - Format pro Zeile: `millis;distance`

## Verwendete Bibliotheken
`Ultraschall.ino` nutzt:
- `Arduino.h`
- `Stepper.h`
- `IRremote.h`
- `LiquidCrystal_I2C.h`
- `SPI.h`
- `SD.h`
- `avr/pgmspace.h`
- `EEPROM.h`

## Hardware (aktueller Stand)
- Arduino (UNO-kompatibel)
- HC-SR04 Ultraschallsensor
- Steppermotor + Treiber
- IR-Empfänger + Fernbedienung
- 16x2 I2C LCD
- SD-Kartenmodul

## Pinbelegung laut `Ultraschall.ino`
### Steppermotor
- `D9`, `D7`, `D8`, `D6` (Stepper-Spulen)

### IR
- `A3` → `IR_RECEIVE_PIN`

### Ultraschallsensor
- `D0` → `trigPin`
- `D1` → `echoPin`

### SD-Karte
- `D10` → `SD_CS_PIN` (Chip Select)
- SPI-Pins sind boardabhängig (bei UNO: `D11/D12/D13`)

### Sonstige Eingänge
- `A0` → `SWRD`
- `A1` → `SWBU`
- `A2` → `SWWH`

## Ablauf beim Start
1. LCD wird via I2C initialisiert (`lcd.init()`, `lcd.backlight()`).
2. Startscreen „Raumvermesser V1.0!“ wird angezeigt.
3. IR-Empfänger wird gestartet.
4. SD-Karte wird initialisiert:
   - Bei Erfolg: `SD-Karte bereit` über Serial
   - Bei Fehler: `SD-Karte nicht gefunden` über Serial

## SD-Logging
In `measurement()` wird bei verfügbarer SD-Karte pro Messung in `messung.csv` geschrieben:
- Zeitstempel in Millisekunden (`millis()`)
- Trennzeichen `;`
- Distanzwert

Beispiel:
```text
12345;67.89
12850;68.10
```

## Projektdateien
- `Ultraschall.ino` – komplette Firmware (Menü, Messlogik, Motorsteuerung, SD-Logging)
- `README.md` – Projektbeschreibung und Setup-Informationen

## Hinweise
- Die I2C-LCD-Adresse ist im Code aktuell auf `0x27` gesetzt.
- Bei abweichender LCD-Adresse muss der Konstruktor angepasst werden.
- Für sauberes Logging sollte eine funktionierende SD-Karte (FAT formatiert) verwendet werden.
 =======
<img width="3745" height="3024" alt="20260327_211243~2" src="https://github.com/user-attachments/assets/8973ef26-85bf-4b99-a203-b4113d0161e9" />
# Rotating_distance_measurement



Es soll mit einer IR-Fernbedienung ein Steppermotor bewegt werden, auf dessen Achse ein Ultraschallsensor montiert ist. Dieser soll mit Tasten auf der Fernbedienung über die horizontale Achse stufenlos um 360° in beide Richtungen gedreht werden können, sodass der Abstand in [cm] auf einem LCD-Display per Knopfdruck in Echtzeit dargestellt werden kann.
Für eine korrekte IR-Signalerkennung von der Fernbedienung wird eine rote LED installiert, die aufleuchtet, wenn ein IR-Signal erkannt wird.

Zusatz: Taste auf der Fernbedienung für automatische Messungen für einen kompletten 360° Umlauf. Speicherung der Messergebnisse im .csv Format auf einem SD-Kartenmodul, um die Auswertung zu erleichtern.


Wir wollen ein Gerät zur Abmessung eines Raumes entwerfen. Dabei soll ein Ultraschallsensor verwendet werden, der auf der Drehachse eines Steppermotors befestigt ist. Dieser kann an einer Stelle des Raumes abgestellt werden und die horizontale Messung soll dann per IR-Fernbedienung eingeleitet werden. 
 
Das Gerät soll per An/Aus-Schalter ein, bzw. ausgeschaltet werden. Eine LED soll dabei angeben ob das Gerät an oder aus ist.
Ziel ist es, am Ende 2 Betriebsmodi zu haben. Einmal einen automatischen Ablauf, wo der Sensor in festgelegten Abständen regelmäßige Messungen im horizontalen 360° Kreis vollführt und die Ergebnisse pro Messung im Display ausgibt, und dazu einen manuellen Modus, wo Einzelmessungen getätigt werden können, wobei Winkel und Anzahl der Messungen per IR-Fernbedienung eingestellt werden.
 
Eventuell soll sogar ausgewählt werden in wie vielen Messungen die 360° im Automatikmodus abgefahren werden sollen, quasi eine wählbare Auflösung.
 
Im Display soll die Menüanzeige angezeigt werden, also ob Automatik oder Manueller Modus gewählt wurde. Zudem sollen die Messergebnisse dort wiedergegeben werden und man soll auch per Fernbedienung durch die Liste der Messungen navigieren können.
Der Motor soll im Idealfall auch über einen Encoder verfügen damit eine Kalibrierung des Sensors möglich ist. Oder die Kalibrierung wird über einen Endschalter realisiert.
 
 
 
Einteilung nach EVA:
 
Eingabe:
Ultraschallsensor, IR Fernbedienung, An/Aus-Schalter
 
Verarbeitung: Arduino, (ggf. Encoder)

Ausgabe: LCD-Display, SD-Karte
>>>>>>> main
