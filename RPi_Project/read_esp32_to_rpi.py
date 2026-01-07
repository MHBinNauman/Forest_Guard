import serial
import json
import time
import requests

# ---------- Serial ----------
PORT = "/dev/ttyACM0"   # or /dev/ttyUSB0
BAUD = 115200

# ---------- Blynk ----------
BLYNK_TOKEN = "PASTE_YOUR_BLYNK_DEVICE_TOKEN_HERE"
BLYNK_SERVER = "https://blynk.cloud"

# Batch update endpoint (updates multiple virtual pins at once)
BLYNK_BATCH_URL = f"{BLYNK_SERVER}/external/api/batch/update"  
# How often to push to Blynk (seconds)
PUSH_INTERVAL = 1.0

def fmt(v, suffix=""):
    if v is None:
        return "NA"
    return f"{v}{suffix}"

def safe_int(v, default=0):
    try:
        return int(v)
    except:
        return default

def safe_float(v):
    try:
        return float(v)
    except:
        return None

def send_to_blynk(payload: dict):
    """
    payload example:
    { "V0": 25.1, "V1": 36.0, "V3": 1, "V7": "33.6" }
    """
    params = {"token": BLYNK_TOKEN}
    params.update(payload)

    try:
        r = requests.get(BLYNK_BATCH_URL, params=params, timeout=5)
        # Blynk returns 200 on success
        if r.status_code != 200:
            print(f"[BLYNK] HTTP {r.status_code}: {r.text}")
    except Exception as e:
        print(f"[BLYNK] Error: {e}")

def main():
    ser = serial.Serial(PORT, BAUD, timeout=1)
    time.sleep(2)  # ESP32 may reset when serial opens
    print(f"Listening on {PORT} @ {BAUD}...\n")

    last_push = 0.0

    while True:
        line = ser.readline().decode(errors="ignore").strip()
        if not line:
            continue

        try:
            data = json.loads(line)

            mq2   = data.get("mq2_adc")
            temp  = data.get("temp")
            hum   = data.get("hum")
            dist  = data.get("dist_cm")

            fire  = data.get("fire")
            flood = data.get("flood")
            quake = data.get("quake")

            gps_active = data.get("gps_active")
            gps_fix    = data.get("gps_fix")
           
            print(
                f"MQ2={fmt(mq2)}  "
                f"T={fmt(temp,'C')}  H={fmt(hum,'%')}  "
                f"Dist={fmt(dist,'cm')}  "
                f"FIRE={fire} FLOOD={flood} QUAKE={quake}  "
                f"GPS_ACTIVE={gps_active} GPS_FIX={gps_fix}  "
                f"LAT={fmt(lat)} LON={fmt(lon)}"
            )

            # ----- Push to Blynk every PUSH_INTERVAL seconds -----
            now = time.time()
            if now - last_push >= PUSH_INTERVAL:
                last_push = now

                # If dist is null, send nothing / or send -1 (your choice)
                dist_val = safe_float(dist)
                if dist_val is None:
                    dist_val = -1

                # Lat/Lon labels: show NA if no fix
                if lat is None or lon is None:
                    lat_str = "NA"
                    lon_str = "NA"
                else:
                    lat_str = f"{lat}"
                    lon_str = f"{lon}"

                payload = {
                    # Gauges
                    "V0": safe_float(temp) if temp is not None else 0,
                    "V1": safe_float(hum)  if hum  is not None else 0,
                    "V2": dist_val,  # "dam depth" gauge (using ultrasonic distance)

                    # Alarms
                    "V3": safe_int(fire, 0),
                    "V4": safe_int(flood, 0),
                    "V5": safe_int(quake, 0),

                    # GPS
                    "V6": safe_int(gps_active, 0),
                    "V7": lat_str,
                    "V8": lon_str,
                }

                send_to_blynk(payload)

        except json.JSONDecodeError:
            print("RAW:", line)

if __name__ == "__main__":
    main()
