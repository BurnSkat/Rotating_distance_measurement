<<<<<<< codex/replace-liquidcrystal-with-liquidcrystal_i2c-and-add-sd-modu-dvygfz
# Rotating Distance Measurement

<img width="3745" height="3024" alt="Aufbau" src="https://github.com/user-attachments/assets/8973ef26-85bf-4b99-a203-b4113d0161e9" />

[SPI_Reader_Micro_Speicherkartenmodul_Arduino_Schematics_b636d48d-56a5-4e52-bbe0-19c7ae77d037.pdf](https://github.com/user-attachments/files/26814594/SPI_Reader_Micro_Speicherkartenmodul_Arduino_Schematics_b636d48d-56a5-4e52-bbe0-19c7ae77d037.pdf)
[HD44780_1602_Blaues_LCD_Display_mit_Serielle_Schnittstelle_I2C_Bundle_Datenblatt_AZ-Delivery_Vertriebs_GmbH.pdf](https://github.com/user-attachments/files/26814592/HD44780_1602_Blaues_LCD_Display_mit_Serielle_Schnittstelle_I2C_Bundle_Datenblatt_AZ-Delivery_Vertriebs_GmbH.pdf)


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

Einteilung nach EVA:
 
Eingabe:
Ultraschallsensor, IR Fernbedienung, An/Aus-Schalter
 
Verarbeitung: Arduino, (ggf. Encoder)

Ausgabe: LCD-Display, SD-Karte
>>>>>>> main
