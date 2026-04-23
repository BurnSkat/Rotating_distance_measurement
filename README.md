# Rotating Distance Measurement

Dieses Projekt erfasst Distanzwerte mit einem **HC-SR04 Ultraschallsensor**, der auf einem **Schrittmotor** montiert ist und den Raum über 360° abtastet. 
Die Bedienung erfolgt über eine **IR-Fernbedienung**, die Ausgabe über ein **16x2-I2C-LCD** und optional per **CSV-Logging auf SD-Karte**.

## Bilder

<img width="3745" height="3024" alt="Aufbau" src="https://github.com/user-attachments/assets/8973ef26-85bf-4b99-a203-b4113d0161e9" />

<img width="4032" height="3024" alt="20260417_104809" src="https://github.com/user-attachments/assets/cd420b83-92fa-4115-9fea-b8b2fd58885a" />

<img width="947" height="701" alt="grafik" src="https://github.com/user-attachments/assets/0bc7966a-bb63-41d5-9c5c-c44a46430df7" />

<img width="869" height="612" alt="grafik" src="https://github.com/user-attachments/assets/9aa28e56-934c-467c-952b-79113d625685" />

## Funktionen

- **Manueller Modus**
  - Drehen des Sensors per IR-Tasten (kontinuierlich oder in festen Schritten)
  - Einzelmessungen auf Tastendruck
- **Automatik-Modus**
  - Messung einer definierten Anzahl von Messpunkten über 360°
- **Anzeige am LCD**
  - Menüführung, Modusauswahl und Messwerte
- **SD-Logging**
  - Speicherung in `messung.csv`
  - Zeilenformat: `millis;distance`

## Hardware

- Arduino (UNO-kompatibel)
- HC-SR04 Ultraschallsensor
- Schrittmotor + Treiber
- IR-Empfänger + Fernbedienung
- 16x2 I2C LCD
- SD-Kartenmodul
- MB102 Netzteil-Adapter
- Wondom BCPB2 Spannungsversorgung

## Verwendete Bibliotheken (laut `Ultraschall.ino`)

- `Arduino.h`
- `Stepper.h`
- `IRremote.h`
- `LiquidCrystal_I2C.h`
- `SPI.h`
- `SD.h`
- `avr/pgmspace.h`
- `EEPROM.h`

## Pinbelegung (`Ultraschall.ino`)

### Schrittmotor
- `D9`, `D7`, `D8`, `D6`

### IR
- `A3` → `IR_RECEIVE_PIN`

### Ultraschallsensor
- `D0` → `trigPin`
- `D1` → `echoPin`

### SD-Karte
- `D10` → `SD_CS_PIN`
- SPI bei UNO: `D11`, `D12`, `D13`

### Weitere Eingänge
- `A0` → `SWRD`
- `A1` → `SWBU`
- `A2` → `SWWH`

## Startablauf

1. LCD initialisieren (`lcd.init()`, `lcd.backlight()`).
2. Startscreen anzeigen („Raumvermesser V1.0!“).
3. IR-Empfänger starten.
4. SD-Karte initialisieren.
   - Erfolg: `SD-Karte bereit` (Serial)
   - Fehler: `SD-Karte nicht gefunden` (Serial)

## SD-Logging

Bei jeder Messung wird (falls SD verfügbar) in `messung.csv` geschrieben:

- Zeitstempel in Millisekunden (`millis()`)
- Trennzeichen `;`
- Distanzwert

Beispiel:

```text
12345;67.89
12850;68.10
```

## Projektdateien

- `Ultraschall.ino` – Hauptsketch (Menü, Messlogik, Motorsteuerung, SD-Logging)
- `Winkelanpassung.ino` / `Winkelanpassungv2.ino` – Varianten zur Winkelsteuerung
- `SDandI2CDisplay.ino` – Fokus auf SD + Display
- `BestVersion.ino` – konsolidierter Entwicklungsstand
- `README.md` – Dokumentation

## Bedienung (IR-Fernbedienung)

Zuordnung gemäß den im Repository enthaltenen Sketches (insbesondere `Ultraschall.ino`).

| Taste | IR-HEX (Command) | Funktion |
|---|---|---|
| `1` | `0x0C` | Manueller Modus (`menuPage=0`, `modestate=1`) |
| `2` | `0x18` | Automatikmodus (`menuPage=1`, `modestate=2`) |
| `CH+` | `0x47` | Erhöht geplante Messungen |
| `CH-` | `0x45` | Verringert geplante Messungen |
| `CH` | `0x46` | Bestätigt Auswahl / Start nächste Aktion |
| `>>` | `0x40` | Kontinuierlich nach rechts / vorblättern |
| `<<` | `0x44` | Kontinuierlich nach links / zurückblättern |
| `Vol+` | `0x15` | Schrittweise nach rechts (200 Steps) |
| `Vol-` | `0x07` | Schrittweise nach links (200 Steps) |
| `Play/Pause` | `0x43` | Messung auslösen (manueller Messmodus) |
| `0` | `0x16` | Werte zurücksetzen und ins Menü zurück |

### Derzeit ohne Funktion

| Taste | IR-HEX |
|---|---|
| `3` | `0x5E` |
| `4` | `0x08` |
| `5` | `0x1C` |
| `6` | `0x5A` |
| `7` | `0x42` |
| `8` | `0x52` |
| `9` | `0x4A` |
| `EQ` | `0x09` |
| `100+` | `0x19` |
| `200+` | `0x0D` |

## EVA-Zuordnung

- **Eingabe:** Ultraschallsensor, IR-Fernbedienung, Schalter
- **Verarbeitung:** Arduino
- **Ausgabe:** LCD-Display, SD-Karte

## Hinweise

- Die LCD-I2C-Adresse ist aktuell auf `0x27` gesetzt.
- Bei anderer Adresse den LCD-Konstruktor im Sketch anpassen.
- Für stabiles Logging SD-Karte in FAT/FAT32 formatieren.

## Datenblätter

- Arduino UNO: https://docs.arduino.cc/resources/datasheets/A000066-datasheet.pdf
- Wondom BCPB2: https://www.boomaudio.de/media/pdf/eb/21/e6/Wondom-PS-BC12111-V2RI25nFre0q9S0.pdf
- MB102: https://cdn.shopify.com/s/files/1/1509/1638/files/MB102_Netzteil_Adapter_Datenblatt_AZ-Delivery_Vertriebs_GmbH.pdf
- HC-SR04: https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
