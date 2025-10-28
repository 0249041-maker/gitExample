#!/usr/bin/env python3
import csv, time, os
from datetime import datetime, UTC
from pathlib import Path
import paho.mqtt.client as mqtt

# ---------- CONFIG ----------
TOPIC_PREFIX = "u0249041"   # <-- cámbialo a tu prefijo (p.ej. u07)
TOPICS = [
    f"{TOPIC_PREFIX}/sensors/temperature",
    f"{TOPIC_PREFIX}/sensors/gps",
    f"{TOPIC_PREFIX}/data/status",
    f"{TOPIC_PREFIX}/data/uptime",
]
BROKER_HOST = "127.0.0.1"
BROKER_PORT = 1883
CSV_PATH = Path("mqtt_capture.csv")

# ---------- (Opcional) Paramiko SSH check ----------
USE_SSH_CHECK = True
def ssh_check():
    if not USE_SSH_CHECK:
        return
    try:
        import paramiko
        host = "127.0.0.1"
        user = os.getenv("USER") or "carmen"
        cmd  = "systemctl is-active mosquitto"
        print("[SSH] Checking Mosquitto via SSH...")
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect(host, username=user, timeout=6)  # usará tu clave local si la tienes
        _, stdout, _ = ssh.exec_command(cmd, timeout=6)
        print(f"[SSH] Service state: {stdout.read().decode().strip()}")
        ssh.close()
    except Exception as e:
        print(f"[SSH] Warning: {e}. Continuing...")

# ---------- MQTT (paho 1.x con MQTTv5) ----------
def ensure_header(path: Path):
    if not path.exists():
        with path.open("w", newline="", encoding="utf-8") as f:
            csv.writer(f).writerow(["timestamp_utc","topic","payload","qos","retain"])

def on_connect(client, userdata, flags, rc, properties=None):
    # En paho 1.x no pasan 'properties' por defecto; ya activamos MQTTv5 en el constructor
    print(f"[MQTT] Connected rc={rc} -> subscribing…")
    for t in TOPICS:
        client.subscribe(t, qos=1)

def on_message(client, userdata, msg):
    row = [datetime.now(UTC).isoformat(),
       msg.topic,
       msg.payload.decode("utf-8", errors="replace"),
       msg.qos,
       int(msg.retain)]
    with CSV_PATH.open("a", newline="", encoding="utf-8") as f:
        csv.writer(f).writerow(row)
    print(f"[MQTT] {row}")

def main():
    ssh_check()
    ensure_header(CSV_PATH)
    # Activamos protocolo MQTTv5 con la API 1.x:
    client = mqtt.Client(protocol=mqtt.MQTTv5)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(BROKER_HOST, BROKER_PORT, keepalive=30)
    try:
        client.loop_forever()
    except KeyboardInterrupt:
        print("\n[MQTT] Stopping...")

if __name__ == "__main__":
    main()
