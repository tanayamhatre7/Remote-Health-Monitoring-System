# Remote Health Monitoring — ESP32 + MAX30100 + DHT11 + DS18B20

## What this repo contains
- `arduino/` — ESP32 Arduino sketch to read sensors and publish JSON to MQTT.
- `server/` — Node.js server that subscribes to MQTT, stores readings in MongoDB, exposes REST endpoints and emits real-time updates via Socket.io.
- `web-client/` — minimal React client to view live readings.

## Hardware
- ESP32 dev board
- MAX30100 pulse oximeter sensor
- DHT11 (air temp & humidity)
- DS18B20 (skin/temp probe)
- Jumper wires, breadboard, 3.3V logic where needed

## Wiring (short)
- MAX30100: SDA -> ESP32 SDA, SCL -> ESP32 SCL, Vcc 3.3V, GND
- DHT11: data -> GPIO4, Vcc 5V (or 3.3V), GND
- DS18B20: data -> GPIO15, pull-up 4.7k to Vcc, Vcc 3.3V, GND

## Quick start
1. Flash `arduino/esp32_remote_health.ino`. Update WiFi & MQTT details.
2. `cd server` → `npm install` → create `.env` from `.env.example` → `npm start`.
3. `cd web-client` → `npm install` → `npm start` (or build and host).
4. Watch data in web client or use `GET /api/latest/:device`.




