import re  # Importeer de regex-module voor patroonherkenning in strings.
from typing import NamedTuple  # Importeer NamedTuple voor het definiëren van tupelstructuren.
import paho.mqtt.client as mqtt  # Importeer de MQTT-clientbibliotheek voor het communiceren met een MQTT-broker.
from influxdb import InfluxDBClient  # Importeer de InfluxDB-clientbibliotheek voor het communiceren met een InfluxDB-server.

INFLUXDB_ADDRESS = '192.168.1.254'  # Het IP-adres van de InfluxDB-server.
INFLUXDB_USER = 'Ewout10'  # Gebruikersnaam voor authenticatie bij de InfluxDB-server.
INFLUXDB_PASSWORD = 'YOURPASSWORD'  # Wachtwoord voor authenticatie bij de InfluxDB-server.
INFLUXDB_DATABASE = 'plant10'  # Naam van de te gebruiken InfluxDB-database.
MQTT_ADDRESS = '192.168.1.254'  # Het IP-adres van de MQTT-broker.
MQTT_USER = 'EwoutDoms'  # Gebruikersnaam voor authenticatie bij de MQTT-broker.
MQTT_PASSWORD = 'YOURPASSWORD'  # Wachtwoord voor authenticatie bij de MQTT-broker.
MQTT_TOPIC = 'plant/+/+'  # MQTT-onderwerp (topic) waarop wordt geabonneerd, met jokertekens.
MQTT_REGEX = 'plant/([^/]+)/([^/]+)'  # Regex-patroon voor het extraheren van gegevens uit het onderwerp.
MQTT_CLIENT_ID = 'MQTTInfluxDBBridge'  # Client-ID voor de MQTT-client.

influxdb_client = InfluxDBClient(INFLUXDB_ADDRESS, 8086, INFLUXDB_USER, INFLUXDB_PASSWORD, None)  # Maak een nieuwe InfluxDB-client aan.

class SensorData(NamedTuple):
    location: str
    measurement: str
    value: float

def on_connect(client, userdata, flags, rc):
    """Callback voor wanneer de client een CONNACK-respons van de server ontvangt."""
    print('Connected with result code ' + str(rc))  # Druk het verbindingsresultaat af.
    client.subscribe(MQTT_TOPIC)  # Abonneer op het MQTT-onderwerp.

def _parse_mqtt_message(topic, payload):
    match = re.match(MQTT_REGEX, topic)  # Zoek naar overeenkomsten met het regex-patroon in het onderwerp.
    if match:
        location = match.group(1)  # Haal de locatie op uit het onderwerp.
        measurement = match.group(2)  # Haal de meetwaarde op uit het onderwerp.
        if measurement == 'status':
            return None
        return SensorData(location, measurement, float(payload))  # Maak een SensorData-tupel aan.
    else:
        return None

def _send_sensor_data_to_influxdb(sensor_data):
    json_body = [
        {
            'measurement': sensor_data.measurement,
            'tags': {
                'location': sensor_data.location
            },
            'fields': {
                'value': sensor_data.value
            }
        }
    ]
    influxdb_client.write_points(json_body)  # Stuur de sensorgegevens naar InfluxDB.

def on_message(client, userdata, msg):
    """Callback voor wanneer een PUBLISH-bericht van de server wordt ontvangen."""
    print(msg.topic + ' ' + str(msg.payload))  # Druk het onderwerp en de payload af.
    sensor_data = _parse_mqtt_message(msg.topic, msg.payload.decode('utf-8'))  # Parseer het MQTT-bericht.
    if sensor_data is not None:
        _send_sensor_data_to_influxdb(sensor_data)  # Stuur de geparseerde gegevens naar InfluxDB.

def _init_influxdb_database():
    databases = influxdb_client.get_list_database()  # Haal de lijst van databases op.
    if len(list(filter(lambda x: x['name'] == INFLUXDB_DATABASE, databases))) == 0:
        influxdb_client.create_database(INFLUXDB_DATABASE)  # Maak de database aan als deze nog niet bestaat.
    influxdb_client.switch_database(INFLUXDB_DATABASE)  # Wissel naar de opgegeven database.

def main():
    _init_influxdb_database()  # Initialiseer de InfluxDB-database.

    mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1, MQTT_CLIENT_ID)  # Maak een nieuwe MQTT-client aan.
    mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)  # Stel de gebruikersnaam en het wachtwoord in.
    mqtt_client.on_connect = on_connect  # Stel de callback-functie voor verbinding in.
    mqtt_client.on_message = on_message  # Stel de callback-functie voor berichten in.
    mqtt_client.connect(MQTT_ADDRESS, 1883)  # Verbind met de MQTT-broker op poort 1883.
    mqtt_client.loop_forever()  # Houd de client actief en blijf luisteren naar inkomende berichten.

if __name__ == '__main__':
    print('MQTT to InfluxDB bridge')  # Druk een startbericht af.
    main()  # Voer de main-functie uit.
