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
- 2 led's
- DHT11
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

3. Volg de bedrading en pcb schema's in:
`bedrading_schema.pdf`
`Foto_PCB_Layout.pdf`
`pcb_layouts`
5. Upload de code van `esp32_code.ino` naar je ESP32 met behulp van Arduino IDE.
6. Upload de code van `Verbind_MQTT-client_MQTT-broker.md` en `Brug_MQTT-broker_InfluxDB-database.md` naar je Raspberry Pi.

7. Stel InfluxDB en Grafana in zoals beschreven in `Setup_Influxdb_Grafana.pdf`.

## Gebruik
1. Zet de ESP32 en Raspberry Pi aan.
2. Toegang tot Grafana op `http://<raspberry-pi-ip>:3000` om de gegevens te visualiseren.
3. Monitor het LCD-scherm voor real-time sensor gegevens.
