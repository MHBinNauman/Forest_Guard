#include <Arduino.h>
#include <DHT.h>
#include <TinyGPSPlus.h>

// ====== PINS ======
const int PIN_MQ2_AO     = 34;  // ADC
const int PIN_DHT_DATA   = 27;
const int PIN_FIRE_LED   = 2;   // active LOW

const int PIN_ULTRA_TRIG = 18;
const int PIN_ULTRA_ECHO = 19;
const int PIN_FLOOD_LED  = 4;   // active LOW

const int PIN_QUAKE_DOUT = 25;  // SW-420 DOUT

const int PIN_GPS_TX_ESP = 17;  // ESP32 TX2 -> GPS RX
const int PIN_GPS_RX_ESP = 16;  // ESP32 RX2 <- GPS TX
const int PIN_GPS_LED    = 5;   // active LOW

// ====== DHT ======
#define DHTTYPE DHT11
DHT dht(PIN_DHT_DATA, DHTTYPE);

// ====== GPS ======
HardwareSerial SerialGPS(2);
TinyGPSPlus gps;

// ====== THRESHOLDS (tune) ======
const int   MQ2_ADC_FIRE_THRESHOLD = 2000; // 0-4095
const float TEMP_FIRE_THRESHOLD    = 60.0; // C
const float HUM_FIRE_THRESHOLD     = 30.0; // %

const float FLOOD_DISTANCE_CM      = 5.0;

// ====== QUAKE LOGIC ======
const uint32_t QUAKE_DEBOUNCE_MS = 30;
const uint32_t QUAKE_CLEAR_MS    = 2000;

bool quake_active = false;
uint32_t last_quake_event_ms = 0;
bool last_raw = false;
uint32_t raw_change_ms = 0;

void updateQuakeState() {
  bool raw = (digitalRead(PIN_QUAKE_DOUT) == HIGH);

  if (raw != last_raw) {
    last_raw = raw;
    raw_change_ms = millis();
  }

  if (raw && (millis() - raw_change_ms >= QUAKE_DEBOUNCE_MS)) {
    quake_active = true;
    last_quake_event_ms = millis();
  }

  if (quake_active && (millis() - last_quake_event_ms > QUAKE_CLEAR_MS)) {
    quake_active = false;
  }
}

// ====== GPS ACTIVITY ======
uint32_t lastGpsCharMillis = 0;
const uint32_t GPS_ACTIVE_TIMEOUT = 1200;

float measureDistanceCm() {
  digitalWrite(PIN_ULTRA_TRIG, LOW);
  delayMicroseconds(3);
  digitalWrite(PIN_ULTRA_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_ULTRA_TRIG, LOW);

  unsigned long duration = pulseIn(PIN_ULTRA_ECHO, HIGH, 30000);
  if (duration == 0) return -1.0;
  return duration / 58.0;
}

void setFireLed(bool on)  { digitalWrite(PIN_FIRE_LED,  on ? LOW : HIGH); }
void setFloodLed(bool on) { digitalWrite(PIN_FLOOD_LED, on ? LOW : HIGH); }
void setGpsLed(bool on)   { digitalWrite(PIN_GPS_LED,   on ? LOW : HIGH); }

void setup() {
  Serial.begin(115200);
  delay(700);

  pinMode(PIN_FIRE_LED, OUTPUT);
  pinMode(PIN_FLOOD_LED, OUTPUT);
  pinMode(PIN_GPS_LED, OUTPUT);
  digitalWrite(PIN_FIRE_LED, HIGH);
  digitalWrite(PIN_FLOOD_LED, HIGH);
  digitalWrite(PIN_GPS_LED, HIGH);

  pinMode(PIN_ULTRA_TRIG, OUTPUT);
  pinMode(PIN_ULTRA_ECHO, INPUT);

  pinMode(PIN_QUAKE_DOUT, INPUT);

  dht.begin();

  SerialGPS.begin(9600, SERIAL_8N1, PIN_GPS_RX_ESP, PIN_GPS_TX_ESP);

  Serial.println("{\"status\":\"esp32_online\"}");
}

void loop() {
  // ---- GPS parsing ----
  while (SerialGPS.available()) {
    char c = (char)SerialGPS.read();
    gps.encode(c);
    lastGpsCharMillis = millis();
  }

  bool gps_active = (millis() - lastGpsCharMillis) < GPS_ACTIVE_TIMEOUT;
  setGpsLed(gps_active);

  bool gps_fix = gps.location.isValid() && gps.location.age() < 3000; // recent coords

  // ---- FIRE ----
  int mq2Value = analogRead(PIN_MQ2_AO);
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  bool fireDetected = false;
  if (!isnan(temp) && !isnan(hum)) {
    if (temp >= TEMP_FIRE_THRESHOLD && hum <= HUM_FIRE_THRESHOLD) fireDetected = true;
  }
  if (mq2Value >= MQ2_ADC_FIRE_THRESHOLD) fireDetected = true;
  setFireLed(fireDetected);

  // ---- FLOOD ----
  float distance = measureDistanceCm();
  bool floodDetected = (distance > 0 && distance <= FLOOD_DISTANCE_CM);
  setFloodLed(floodDetected);

  // ---- QUAKE ----
  updateQuakeState();

  // ---- SEND JSON (1 line / sec) ----
  static uint32_t lastSend = 0;
  if (millis() - lastSend >= 1000) {
    lastSend = millis();

    Serial.print("{\"mq2_adc\":"); Serial.print(mq2Value);

    Serial.print(",\"temp\":");
    if (isnan(temp)) Serial.print("null"); else Serial.print(temp, 1);

    Serial.print(",\"hum\":");
    if (isnan(hum)) Serial.print("null"); else Serial.print(hum, 1);

    Serial.print(",\"dist_cm\":");
    if (distance < 0) Serial.print("null"); else Serial.print(distance, 1);

    Serial.print(",\"fire\":");  Serial.print(fireDetected ? 1 : 0);
    Serial.print(",\"flood\":"); Serial.print(floodDetected ? 1 : 0);
    Serial.print(",\"quake\":"); Serial.print(quake_active ? 1 : 0);

    Serial.print(",\"gps_active\":"); Serial.print(gps_active ? 1 : 0);
    Serial.print(",\"gps_fix\":");    Serial.print(gps_fix ? 1 : 0);

    Serial.print(",\"lat\":");
    if (gps_fix) Serial.print(gps.location.lat(), 6);
    else Serial.print("null");

    Serial.print(",\"lon\":");
    if (gps_fix) Serial.print(gps.location.lng(), 6);
    else Serial.print("null");

    Serial.println("}");
  }

  delay(20);
}
