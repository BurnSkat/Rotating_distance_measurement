# Rotating Distance Measurement

<img width="3745" height="3024" alt="Aufbau" src="https://github.com/user-attachments/assets/8973ef26-85bf-4b99-a203-b4113d0161e9" />

<img width="4032" height="3024" alt="20260417_104809" src="https://github.com/user-attachments/assets/cd420b83-92fa-4115-9fea-b8b2fd58885a" />

<img width="947" height="701" alt="grafik" src="https://github.com/user-attachments/assets/0bc7966a-bb63-41d5-9c5c-c44a46430df7" />

<img width="869" height="612" alt="grafik" src="https://github.com/user-attachments/assets/9aa28e56-934c-467c-952b-79113d625685" />



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


## Bedienungsanleitung

### IR-Fernbedienung (Protokoll 7)

Die folgende Tabelle basiert direkt auf den in `Ultraschall.ino` ausgewerteten `command`-Werten.

| Taste | IR-HEX (Command) | Verwendung im Sketch |
|---|---|---|
| CH- | `0x45` | Verringert die Anzahl der Messungen im jeweiligen Untermenü (`modestate` 1/2). |
| CH | `0x46` | Bestätigt die Auswahl: startet im manuellen Modus die Initialisierung/den Ablauf; im Automatik-Menü wird die Messanzahl übernommen. |
| CH+ | `0x47` | Erhöht die Anzahl der Messungen im jeweiligen Untermenü (`modestate` 1/2). |
| -<< | `0x44` | Dreht im manuellen Messmodus nach links (Gegenuhrzeigersinn); in der Ergebnisansicht blättert zurück. |
| >> | `0x40` | Dreht im manuellen Messmodus nach rechts (Uhrzeigersinn); in der Ergebnisansicht blättert vor. |
| Play/Pause | `0x43` | Löst im manuellen Messmodus (`modestate` 5) eine Ultraschallmessung aus. |
| Vol- | `0x07` | Dreht den Stepper schrittweise nach links (200 Schritte). |
| Vol+ | `0x15` | Dreht den Stepper schrittweise nach rechts (200 Schritte). |
| 0 | `0x16` | Setzt im Ergebnisbildschirm (`modestate` 9) Mess-/Hilfswerte zurück und springt ins Hauptmenü. |
| 1 | `0x0C` | Wählt „Manuell“ (setzt `menuPage=0`, `modestate=1`). |
| 2 | `0x18` | Wählt „Automatik“ (setzt `menuPage=1`, `modestate=2`). |
| 3 | `0x5E` | Im Sketch derzeit nicht ausgewertet. |
| 4 | `0x08` | Im Sketch derzeit nicht ausgewertet. |
| 5 | `0x1C` | Im Sketch derzeit nicht ausgewertet. |
| 6 | `0x5A` | Im Sketch derzeit nicht ausgewertet. |
| 7 | `0x42` | Im Sketch derzeit nicht ausgewertet. |
| 8 | `0x52` | Im Sketch derzeit nicht ausgewertet. |
| 9 | `0x4A` | Im Sketch derzeit nicht ausgewertet. |
| EQ | `0x09` | Im Sketch derzeit nicht ausgewertet. |
| 100+ | `0x19` | Im Sketch derzeit nicht ausgewertet. |
| 200+ | `0x0D` | Im Sketch derzeit nicht ausgewertet. |

