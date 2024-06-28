import paho.mqtt.client as mqtt  # Importeer de MQTT-clientbibliotheek voor het communiceren met een MQTT-broker.

import subprocess  # Voor het uitvoeren van externe programma's of shell-opdrachten.


mqtt_broker = "192.168.1.254"  # Het IP-adres van de MQTT-broker.

mqtt_port = 1883  # De poort waarop de MQTT-broker luistert.

mqtt_user = "EwoutDoms"  # Gebruikersnaam voor authenticatie bij de MQTT-broker.

mqtt_password = "YOURPASSWORD"  # Wachtwoord voor authenticatie bij de MQTT-broker.

mqtt_topic_prefix = "plant/"  # Voorvoegsel voor MQTT-onderwerpen (topics).


def on_connect(client, userdata, flags, rc):
    
    print("Geconnecteerd")  # Bericht om aan te geven dat de verbinding tot stand is gebracht.
    
    client.subscribe([  # Abonneer op meerdere MQTT-onderwerpen met QoS-niveau 0.
        (mqtt_topic_prefix + "plant1/temperatuur", 0),
        (mqtt_topic_prefix + "plant1/humidity", 0),
        (mqtt_topic_prefix + "plant1/vochtigheidPercentage", 0),
        (mqtt_topic_prefix + "plant1/ldrwaarde", 0),
        (mqtt_topic_prefix + "plant1/DS18B20", 0),
        (mqtt_topic_prefix + "plant1/waterAfstand", 0),
        (mqtt_topic_prefix + "plant1/LEDaan", 0),
        (mqtt_topic_prefix + "plant1/waterpompAAN", 0),
        (mqtt_topic_prefix + "plant1/warmtematLED", 0),
        (mqtt_topic_prefix + "plant1/VentilatorLED", 0),
        ])


client = mqtt.Client()  # Maak een nieuwe MQTT-client aan.

client.username_pw_set(mqtt_user, mqtt_password)  # Stel de gebruikersnaam en wachtwoord in voor authenticatie.

client.on_connect = on_connect  # Stel de callback-functie in die wordt aangeroepen wanneer verbinding is gemaakt.

client.connect(mqtt_broker, mqtt_port, 60)  # Verbind met de MQTT-broker op de opgegeven poort met een keepalive van 60 seconden.

client.loop_forever()  # Houd de client actief en blijf luisteren naar inkomende berichten.
