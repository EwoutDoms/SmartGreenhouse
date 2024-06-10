# Smart Greenhouse

## Project Overzicht
Dit project heeft als doel een slim systeem te creëren dat automatisch de omgeving aanpast voor optimale plantengroei. Het systeem monitort bodemvochtigheid, lichtniveaus en temperatuur en activeert waterpompen, LED-verlichting en ventilatoren indien nodig.

## Functies
- Automatisch water geven op basis van bodemvochtigheidsniveaus
- LED-verlichting controle op basis van omgevingslicht
- Temperatuurregeling met behulp van ventilatoren en verwarmingsmatten
- Gegevenslogboek en visualisatie via InfluxDB en Grafana

## Installatie

### Hardware Vereisten
- ESP32
- Bodemvochtsensor
- Lichtafhankelijke weerstand (LDR)
- DS18B20 temperatuursensor
- Waterpomp
- LED-strip
- RFID-lezer
- Ultrasone sensor
- LCD-scherm
- Raspberry Pi

### Software Vereisten
- Arduino IDE
- InfluxDB
- Grafana

### Installeer benodigde bibliotheken voor ESP32:
   - WiFi.h
   - PubSubClient.h
   - SPI.h
   - MFRC522.h
   - DHT.h
   - OneWire.h
   - DallasTemperature.h
   - NTPClient.h
   - Wire.h
   - LiquidCrystal_I2C.h

3. Volg de bedrading schema's in `docs/wiring_diagram.pdf`.

4. Upload de code van `code/esp32_code.ino` naar je ESP32 met behulp van Arduino IDE.

5. Stel InfluxDB en Grafana in zoals beschreven in `docs/setup_influxdb_grafana.md`.

## Gebruik
1. Zet de ESP32 en Raspberry Pi aan.
2. Toegang tot Grafana op `http://<raspberry-pi-ip>:3000` om de gegevens te visualiseren.
3. Monitor het LCD-scherm voor real-time sensor gegevens.

## Directory Structuur
SmartGreenhouse/
├── code/
│   └── esp32_code.ino
├── docs/
│   ├── wiring_diagram.pdf
│   └── setup_influxdb_grafana.md
├── pcb/
│   └── pcb_layouts.brd
└── README.md