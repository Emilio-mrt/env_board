import json
import base64
import paho.mqtt.client as mqtt
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS

# Configuration InfluxDB
influxdb_url = "http://10.17.8.39:15000"
influxdb_bucket = "write"

# Init InfluxDB
influxdb_client = InfluxDBClient(url=influxdb_url)
write_api = influxdb_client.write_api(write_options=SYNCHRONOUS)

# Champs trames
FIELD_NAMES = ["id", "temperature","humidite","pression", "part1", "part2", "part3","vbatt","name"]

def decode_payload(encoded_payload):
    try:
        decoded_bytes = base64.b64decode(encoded_payload)
        decoded_string = decoded_bytes.decode('utf-8')

        # S  paration des valeurs
        values = decoded_string.split(';')
        parsed_values = {}

        for i, field in enumerate(FIELD_NAMES):
            try:
                if i < len(values):  #  ^iviter l'IndexError
                    parsed_values[field] = float(values[i]) if '.' in values[i] else int(values[i])
                else:
                    parsed_values[field] = None  # Si manque une valeur, la mettre    None
            except ValueError:
                parsed_values[field] = values[i]  # Garder en string si non convertible

        print(f"Payload d  cod   : {decoded_string}")
        print(f"Valeurs extraites : {parsed_values}")
        return parsed_values

    except (base64.binascii.Error, UnicodeDecodeError) as e:
        print(f"Erreur lors du d  codage du payload: {e}")
        return None  # Retourne None
def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        encoded_payload = payload.get("uplink_message", {}).get("frm_payload", "")

        # D  coder le payload
        decoded_values = decode_payload(encoded_payload)
        if decoded_values is None:
            print("Payload non valide, m  trique ignor  e.")
            return

        # Extraire l'ID pour le tag
        id_value = decoded_values.get("id")
        if id_value is None:
            print("ID manquant dans le payload, m  trique ignor  e.")
            return
        if not name:
                point = Point("mqtt_data").tag("topic", id_value)
        else:
                point = Point("mqtt_data").tag("topic", name)

        # Ajouter les autres valeurs comme champs
        for field, value in decoded_values.items():
            if field != "id" and value is not None:  # Exclure l'ID et ajouter seulement les valeurs valides
                point = point.field(field, value)

        #  ^icriture dans InfluxDB
        write_api.write(bucket=influxdb_bucket, org="env_board_data", record=point)
        print("Donn  es   crites dans InfluxDB avec succ  s.")

    except json.JSONDecodeError as e:
        print(f"Erreur de d  codage JSON: {e}")

def on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0:
        print("Connect   avec succ  s")
        client.subscribe("#")
    else:
        print(f" ^ichec de connexion avec le code {rc}")

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message