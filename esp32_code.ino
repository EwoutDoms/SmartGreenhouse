// Voor met de wifi te verbinden
#include <WiFi.h>
const char* ssid = "YOURWIFINAME";
const char* password = "YOURWIFIPASSWORD";
//////////////////////////////////////////////////////
// Voor met MQTT te verbinden
#include <PubSubClient.h>
const char* mqttServer = "192.168.0.242";
const int mqttPort = 1883;
const char* mqttUser = "EwoutDoms";
const char* mqttPassword = "YOURPASSWORD";
WiFiClient espClient;
PubSubClient client(espClient);
//////////////////////////////////////////////////////
// Voor de RFID-lezer
#include <SPI.h>
#include <MFRC522.h>
//////////////////////////////////////////////////////
#include <DHT.h>
//////////////////////////////////////////////////////
// Voor de DS18B20 temperatuur sensor
#include <OneWire.h>
#include <DallasTemperature.h>
//////////////////////////////////////////////////////
// Voor de actuele tijd te geven
#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
//////////////////////////////////////////////////////
// Voor het LCD scherm
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// GPIO 21 (SDA)
// GPIO 22 (SCL)
LiquidCrystal_I2C lcd(0x27, 15, 2);

//-------------------------------------------------------------

// Ondersteunt communicatie met RFID
#define SS_PIN 5
#define RST_PIN 33
MFRC522 mfrc522(SS_PIN, RST_PIN);
//////////////////////////////////////////////////////
// Opzetten DHT sensor
#define DHTPin 14
#define DHTTYPE DHT11
DHT dht(DHTPin, DHTTYPE);
//////////////////////////////////////////////////////
#define vochtmeting 35
//////////////////////////////////////////////////////
#define waterpomp 12
//////////////////////////////////////////////////////
#define ldr 34
//////////////////////////////////////////////////////
// Opzetten DS18B20 sensor
#define oneWireBus 15
// Maak een OneWire-object om de communicatie met de DS18B20 te beheren
OneWire oneWire(oneWireBus);
// Maak een DallasTemperature-object om de temperatuur te lezen
DallasTemperature sensors(&oneWire);
//////////////////////////////////////////////////////
#define LEDplant 27
//////////////////////////////////////////////////////
#define ventilatorLED 4
//////////////////////////////////////////////////////
#define warmtematLED 13
//////////////////////////////////////////////////////
#define trigPin 25
#define echoPin 26

//-------------------------------------------------------------

// Variabelen declareren en initialiseren
int y;
unsigned long aantijd = 0;
unsigned long uittijd = 0;
unsigned long aantijd2 = 0;

long duration;
int afstand;
float temperatuur;
float humidity;
float temperatuurC;
int vochtwaarde;
int vochtigheidPercentage;
int ldrwaarde;
int huidigetijd;

float x = 0;
float a = 0;
float b = 0;
float c = 0;
float d = 0;
float e = 0;
float f = 0;
float g = 0;
float h = 0;
float i = 0;
float j = 0;

float aaa = 0;
//-------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  //////////////////////////////////////////////////////
  // Zoeken naar wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Zoeken naar Wifi");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wifi zoeken");
  }
  Serial.println("Geconnecteerd met Wifi");
  //////////////////////////////////////////////////////
  // Verbinden met MQTT
  client.setServer(mqttServer, mqttPort);
  while (!client.connected()) {
    Serial.println("Connecteren met MQTT");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Geconnecteerd met MQTT");
    } else {
      Serial.print("Gefaald met connecteren met MQTT ");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MQTT zoeken");
      Serial.print(client.state());
      delay(1000);
    }
  }

  //////////////////////////////////////////////////////
  // Begin voor rfid-kaart te lezen
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Gereed voor RFID-kaart te lezen");
  Serial.println();
  //////////////////////////////////////////////////////
  // Begin voor dht11
  dht.begin();
  //////////////////////////////////////////////////////
  // Begin voor ds18b20
  sensors.begin();
  //////////////////////////////////////////////////////
  // Begin voor actuele tijd van Belgie
  timeClient.begin();
  timeClient.setTimeOffset(7200);  // Tijdzone van  België (UTC+2)
  //////////////////////////////////////////////////////
  // Begin voor LCD scherm
  lcd.init();
  lcd.backlight();
  //////////////////////////////////////////////////////

  pinMode(vochtmeting, INPUT);
  pinMode(waterpomp, OUTPUT);
  pinMode(ldr, INPUT);
  pinMode(LEDplant, OUTPUT);
  pinMode(ventilatorLED, OUTPUT);
  pinMode(warmtematLED, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

//-------------------------------------------------------------
void loop() {

  // Controleer verbinding en herverbind met MQTT indien nodig
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  Serial.println("---------------------------------------------------------------");

  Tijd();
  RFID();
  vochtigheidgrond();
  DS18B20();
  DHT();
  LDR();
  LCD();
  waterleeg();

  if (y == 1) {
    Serial.println("1");
    led1();
    pompenwater1();
    warmte1();

  } else if (y == 2) {
    Serial.println("2");
    led2();
    pompenwater2();
    warmte2();
  }

  // Ontkoppel MQTT aan het einde van de loop
  disconnectMQTT();

  delay(1000);
}


//-------------------------------------------------------------
void reconnect() {
  // Loop tot we opnieuw zijn verbonden met MQTT
  while (!client.connected()) {
    Serial.print("Verbinding zoeken met MQTT");
    // Proberen te connecteren
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Geconnecteerd");
    } else {
      Serial.print("MQTT connectie gefaald, rc=");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MQTT zoeken");
      Serial.print(client.state());
      Serial.println(" Opnieuw proberen");
      // Wacht 1 seconde voor opnieuw te proberen
      delay(1000);
    }
  }
}

void disconnectMQTT() {
  if (client.connected()) {
    client.disconnect();
    Serial.println("Gedisconnecteerd van MQTT");
  }
}

void Tijd() {
  // Huidige tijd weergeven op Serial Monitor
  timeClient.update();
  Serial.println("Tijd: " + timeClient.getFormattedTime());
}

void RFID() {
  // Zoekt voor nieuwe kaart + leest het
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {

    //////////////////////////////////////////////////////

    // Leest de UID + laat het zien op de serial monitor
    Serial.print("UID tag:");
    String content = "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();

    //////////////////////////////////////////////////////

    // Kijkt na of UID code klopt en geeft toegang of geen toegang
    Serial.print("Message: ");
    content.toUpperCase();

    if (content.substring(1) == "73 53 C8 19") {
      y = 1;
      Serial.println("Kaart 1");

    } else if (content.substring(1) == "C3 F6 6A EE") {
      y = 2;
      Serial.println("Kaart 2");

    } else
      Serial.println("Foute kaart");
  }
}

void LCD() {
  // Als de afstand kleiner is dan de waarde
  if (afstand >= 7) {
    // Geeft het op de LCD weer dat het water bijna leeg is
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("water is bijna");
    lcd.setCursor(0, 1);
    lcd.print("leeg!!");

    // Anders geeft het bepaalde waardes, afhankelijk van de delay
  } else {
    if (aaa > 3) {
      // Toon temperatuur en vochtigheid
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("huis temp " + String(temperatuur) + "C");

      lcd.setCursor(0, 1);
      lcd.print("huis vocht " + String(humidity) + "%");
      // Terugkeren naar nul als aaa gelijk is aan 3
    } else {
      // Toon bodemvocht en vochtigheidPercentage
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bodem temp " + String(temperatuurC) + "C");

      lcd.setCursor(0, 1);
      lcd.print("Bodem vocht " + String(vochtigheidPercentage) + "%");
    }
    aaa++;
    if (aaa > 7) {
      aaa = 0;
    }
  }
}

void DHT() {
  // Weergeven waarde van DHT sensor
  temperatuur = dht.readTemperature() + 1;
  humidity = dht.readHumidity();

  // Waarde van temperatuur en humidity weergeven op Serial monitor
  Serial.print("temperatuur DHT11: ");
  Serial.print(temperatuur);
  Serial.println(" °C");
  Serial.print("humidity DHT11: ");
  Serial.print(humidity);
  Serial.println("%");

  a = temperatuur;           // Omzetten voor naar grafana te sturen
  String aa = String(a, 2);  // zet het kommagetal om naar een string met 2 decimalen
  client.publish("plant/plant1/temperatuur", aa.c_str());

  b = humidity;              // Omzetten voor naar grafana te sturen
  String bb = String(b, 2);  // zet het kommagetal om naar een string met 2 decimalen
  client.publish("plant/plant1/humidity", bb.c_str());
}

void vochtigheidgrond() {
  vochtwaarde = analogRead(vochtmeting);
  /*
  // Lees de analoge waarde van de sensor
  Serial.print("vochtwaarde: ");
  Serial.println(vochtwaarde);
  */
  // Verander de analoge waarde naar een vochtigheidspercentage
  vochtigheidPercentage = map(vochtwaarde, 0, 4095, 100, 0);

  // Print het vochtigheidspercentage naar de seriële monitor
  Serial.print("Vochtigheidspercentage: ");
  Serial.print(vochtigheidPercentage);
  Serial.println("%");

  c = vochtigheidPercentage;  // Omzetten voor naar grafana te sturen
  String cc = String(c, 2);   // zet het kommagetal om naar een string met 2 decimalen
  client.publish("plant/plant1/vochtigheidPercentage", cc.c_str());
}

void LDR() {
  // Weergaven waarde van lichtsensor
  ldrwaarde = analogRead(ldr);
  Serial.print("ldr waarde: ");
  Serial.println(ldrwaarde);

  d = ldrwaarde;             // Omzetten voor naar grafana te sturen
  String dd = String(d, 2);  // zet het kommagetal om naar een string met 2 decimalen
  client.publish("plant/plant1/ldrwaarde", dd.c_str());
}

void DS18B20() {
  // Vraag de temperatuur op van alle aangesloten sensoren
  sensors.requestTemperatures();
  // Haal de temperatuurwaarde op van DS18B20 (sensor 0)
  temperatuurC = sensors.getTempCByIndex(0);

  // Controleer of de temperatuurwaarde geldig is (geen -127,0, wat aangeeft dat de meting mislukt is)
  if (temperatuurC != -127.0) {
    // Print de temperatuurwaarde naar de seriële monitor
    Serial.print("Temperatuur DS18B20: ");
    Serial.print(temperatuurC);
    Serial.println(" °C");
  } else {
    // Als de meting mislukt is, print een foutmelding
    Serial.println("Fout bij meting van DS18B20 temperatuur");
  }

  e = temperatuurC;          // Omzetten voor naar grafana te sturen
  String ee = String(e, 2);  // zet het kommagetal om naar een string met 2 decimalen
  client.publish("plant/plant1/DS18B20", ee.c_str());
}

void waterleeg() {
  // Stuur een ultrasoonpuls om de afstand te meten
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Meet de duur van de echo
  duration = pulseIn(echoPin, HIGH);
  // Bereken de afstand in centimeters
  afstand = duration * 0.034 / 2;

  // Geef de gemeten afstand weer op de seriële monitor
  Serial.print("Afstand: ");
  Serial.print(afstand);
  Serial.println(" cm");

  f = afstand;               // Omzetten voor naar grafana te sturen
  String ff = String(f, 2);  // zet het kommagetal om naar een string met 2 decimalen
  client.publish("plant/plant1/waterAfstand", ff.c_str());
}

//-------------------------------------------------------------

void led1() {
  huidigetijd = timeClient.getHours();
  const long interval = 5000;  // Interval van 5 seconden

  // Als de huidige tijd tussen deze uren zit
  if (huidigetijd > 7 && huidigetijd < 21) {
    // Als de lichtsensor geen licht detecteert
    if (ldrwaarde >= 2000) {
      // Als de timer verstreken is schakel de ledstrip aan
      if (millis() - aantijd >= interval) {
        digitalWrite(LEDplant, HIGH);
        // Bericht naar mqtt te sturen
        client.publish("plant/plant1/ledAAN", String(1).c_str());
        Serial.println("LEDplant: aan");
        uittijd = millis();
      }
    } else {
      // Schakel de ledstrip uit als de timer verstreken is
      if (millis() - uittijd >= interval) {
        digitalWrite(LEDplant, LOW);
        Serial.println("LEDplant: uit");
        aantijd = millis();
      }
    }
  } else {
    digitalWrite(LEDplant, LOW);
  }
}


void pompenwater1() {
  const long interval2 = 3000;  // Interval van 3 seconden

  // Als de watervoorraad bijna leeg is. Zet het de motor NIET aan
  if (afstand < 8) {
    // Als de vochtigheid kleiner is dan een bepaalde waarde
    if (vochtigheidPercentage <= 20) {
      // Schakel de waterpomp in
      digitalWrite(waterpomp, HIGH);
      // Bericht naar mqtt te sturen
      client.publish("plant/plant1/waterpompAAN", String(1).c_str());
      Serial.println("Pomp: ingeschakeld");
      aantijd2 = millis();
      // Geeft een bepaald aantal tijd water aan de plant
    } else if (millis() - aantijd2 > interval2) {
      // Schakel de waterpomp uit
      digitalWrite(waterpomp, LOW);
      Serial.println("Pomp: uitgeschakeld");
    }
  } else {
    // Schakel de waterpomp uit
    digitalWrite(waterpomp, LOW);
  }
}

void warmte1() {
  // Als de temperatuur kleiner is dan een bepaalde waarde
  if (temperatuurC < 26) {
    // Zet het de warmtemat (led) aan
    digitalWrite(warmtematLED, HIGH);
    // Bericht naar mqtt te sturen
    client.publish("plant/plant1/warmtematLED", String(1).c_str());
    Serial.println("Te koud --> warmtematLED: aan");
  } else {
    digitalWrite(warmtematLED, LOW);
  }

  // Als de temperatuur groter is dan een bepaalde waarde
  if (temperatuurC > 27) {
    // Zet het de ventilator (led) aan
    digitalWrite(ventilatorLED, HIGH);
    // Bericht naar mqtt te sturen
    client.publish("plant/plant1/VentilatorLED", String(1).c_str());
    Serial.println("Te warm --> VentilatorLED: aan");
  } else {
    digitalWrite(ventilatorLED, LOW);
  }
}

//-------------------------------------------------------------

void led2() {
  huidigetijd = timeClient.getHours();
  const long interval = 5000;  // Interval van 5 seconden

  // Als de huidige tijd tussen deze uren zit
  if (huidigetijd > 7 && huidigetijd < 22) {
    // Als de lichtsensor geen licht detecteert
    if (ldrwaarde >= 750) {
      // Als de timer verstreken is schakel de ledstrip aan
      if (millis() - aantijd >= interval) {
        digitalWrite(LEDplant, HIGH);
        // Bericht naar mqtt te sturen
        client.publish("plant/plant1/ledAAN", String(1).c_str());
        Serial.println("LEDplant: aan");
        uittijd = millis();
      }
    } else {
      // Schakel de ledstrip uit als de timer verstreken is
      if (millis() - uittijd >= interval) {
        digitalWrite(LEDplant, LOW);
        Serial.println("LEDplant: uit");
        aantijd = millis();
      }
    }
  } else {
    digitalWrite(LEDplant, LOW);
  }
}

void pompenwater2() {
  const long interval2 = 3000;  // Interval van 3 seconden

  // Als de watervoorraad bijna leeg is. Zet het de motor NIET aan
  if (afstand < 8) {
    // Als de vochtigheid kleiner is dan een bepaalde waarde
    if (vochtigheidPercentage <= 55) {
      // Schakel de waterpomp in
      digitalWrite(waterpomp, HIGH);
      // Bericht naar mqtt te sturen
      client.publish("plant/plant1/waterpompAAN", String(1).c_str());
      Serial.println("Pomp: ingeschakeld");
      aantijd2 = millis();
      // Geeft een bepaald aantal tijd water aan de plant
    } else if (millis() - aantijd2 > interval2) {
      // Schakel de waterpomp uit
      digitalWrite(waterpomp, LOW);
      Serial.println("Pomp: uitgeschakeld");
    }
  } else {
    // Schakel de waterpomp uit
    digitalWrite(waterpomp, LOW);
  }
}

void warmte2() {
  // Als de temperatuur kleiner is dan een bepaalde waarde
  if (temperatuurC < 27) {
    // Zet het de warmtemat (led) aan
    digitalWrite(warmtematLED, HIGH);
    // Bericht naar mqtt te sturen
    client.publish("plant/plant1/warmtematLED", String(1).c_str());
    Serial.println("Te koud --> warmtematLED: aan");
  } else {
    digitalWrite(warmtematLED, LOW);
  }

  // Als de temperatuur groter is dan een bepaalde waarde
  if (temperatuurC > 28) {
    // Zet het de ventilator (led) aan
    digitalWrite(ventilatorLED, HIGH);
    // Bericht naar mqtt te sturen
    client.publish("plant/plant1/ventilatorLED", String(1).c_str());
    Serial.println("Te warm --> VentilatorLED: aan");
  } else {
    digitalWrite(ventilatorLED, LOW);
  }
}
