import base64
import json
import paho.mqtt.client as mqtt
from influxdb_client import InfluxDBClient, Point
from influxdb_client.client.write_api import SYNCHRONOUS

# --- Configuration InfluxDB ---
INFLUXDB_URL = "http://10.17.8.39:15000"
INFLUXDB_BUCKET = "write"
INFLUXDB_ORG = "env_board_data"

influxdb_client = InfluxDBClient(url=INFLUXDB_URL)
write_api = influxdb_client.write_api(write_options=SYNCHRONOUS)

# --- Champs possibles pour les différents types de trames ---
DATA_FIELDS = ["cmd", "id", "temperature", "humidite", "pression", "part1", "part2", "part3", "vbatt"]
NAME_FIELDS = ["cmd", "id", "name"]

# --- Dictionnaire pour associer ID à nom ---
id_to_name = {}

# --- Fonction : décodage du payload base64 ---
def decode_payload(encoded_payload):
    try:
        decoded_bytes = base64.b64decode(encoded_payload)
        decoded_str = decoded_bytes.decode('utf-8')
        values = decoded_str.split(';')

        if not values or len(values) < 2:
            print("Payload vide ou incomplet.")
            return None

        cmd = int(values[0]) if values[0].isdigit() else None
        field_list = DATA_FIELDS if cmd == 1 else NAME_FIELDS
        parsed_data = {}

        for i, field in enumerate(field_list):
            if i >= len(values):
                parsed_data[field] = None
                continue

            try:
                value = float(values[i]) if '.' in values[i] else int(values[i])
            except ValueError:
                value = values[i]  # Garder le texte brut si ce n'est ni un float ni un int

            parsed_data[field] = value

        print(f"[Décodage] Payload : {decoded_str}")
        print(f"[Décodage] Données extraites : {parsed_data}")
        return parsed_data

    except Exception as e:
        print(f"[Erreur] Impossible de décoder le payload : {e}")
        return None

# --- Callback : traitement d'un message MQTT reçu ---
def on_message(client, userdata, msg):
    try:
        message_json = json.loads(msg.payload.decode())
        encoded_payload = message_json.get("uplink_message", {}).get("frm_payload", "")

        decoded_data = decode_payload(encoded_payload)
        if not decoded_data:
            print("[Ignoré] Payload non valide.")
            return

        cmd = decoded_data.get("cmd")
        sensor_id = decoded_data.get("id")
        sensor_name = decoded_data.get("name")

        if sensor_id is None:
            print("[Ignoré] ID manquant.")
            return

        # cmd == 2 : on enregistre le nom associé à un ID
        if cmd == 2 and sensor_name is not None:
            id_to_name[sensor_id] = sensor_name

        # cmd == 1 : on veut logguer des données
        if cmd == 1:
            # Chercher le nom associé si on l’a déjà reçu, sinon garder l’ID
            sensor_name = id_to_name.get(sensor_id)
            if not sensor_name:  # Si sensor_name est None, "" ou autre valeur falsy
                sensor_name = str(sensor_id)


            # Création du point InfluxDB
            point = Point("mqtt_data").tag("topic", sensor_name)

            for field, value in decoded_data.items():
                if field not in ("id", "name") and value is not None:
                    point = point.field(field, value)

            write_api.write(bucket=INFLUXDB_BUCKET, org=INFLUXDB_ORG, record=point)
            print(f"[Succès] Données écrites pour {sensor_name}.")

    except json.JSONDecodeError as e:
        print(f"[Erreur JSON] {e}")

# --- Callback : connexion au broker MQTT ---
def on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0:
        print("[MQTT] Connecté avec succès.")
        client.subscribe("#")  # S'abonner à tous les topics
    else:
        print(f"[MQTT] Échec de connexion. Code : {rc}")

# --- Configuration et lancement du client MQTT ---
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set(
    username="app-env-board",
    password="NNSXS.PIVTLG7O76LOVDW6E2RWG7YWSSPT4GUJOHMBW6A.4KON6N4IFYXYDFHYDGDYS2A3JYOYUDPYI7Y6I6LMZIHJNBO6UPZA"
)

client.connect("134.158.16.170", 1883, 60)
client.loop_forever()
