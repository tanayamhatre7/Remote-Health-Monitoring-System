// server.js
require('dotenv').config();
const mqtt = require('mqtt');
const express = require('express');
const mongoose = require('mongoose');
const http = require('http');
const socketio = require('socket.io');
const Reading = require('./models/Reading');

const app = express();
const server = http.createServer(app);
const io = socketio(server);

app.use(express.json());

const MONGO = process.env.MONGO_URI || 'mongodb://localhost:27017/remote_health';
mongoose.connect(MONGO, {useNewUrlParser:true, useUnifiedTopology:true});

const mqttUrl = process.env.MQTT_BROKER || 'mqtt://broker.hivemq.com';
const client = mqtt.connect(mqttUrl);

const TOPIC = 'remote-health/#';
client.on('connect', () => {
  console.log('MQTT connected');
  client.subscribe(TOPIC);
});

client.on('message', async (topic, message) => {
  try {
    const json = JSON.parse(message.toString());
    const doc = new Reading({
      device_id: json.device_id,
      timestamp: new Date(),
      heart_rate: json.heart_rate,
      spo2: json.spo2,
      air_temp_c: json.air_temp_c,
      air_humidity: json.air_humidity,
      skin_temp_c: json.skin_temp_c
    });
    await doc.save();
    io.emit('new-reading', doc);
    console.log('Saved reading', doc._id);
  } catch (e) {
    console.error('Bad message', e);
  }
});

// REST endpoints
app.get('/api/latest/:device', async (req, res) => {
  const d = await Reading.findOne({device_id: req.params.device}).sort({timestamp:-1});
  res.json(d);
});

app.get('/api/history/:device', async (req, res) => {
  const items = await Reading.find({device_id: req.params.device}).sort({timestamp:-1}).limit(200);
  res.json(items);
});

const PORT = process.env.PORT || 4000;
server.listen(PORT, ()=>console.log('Server listening', PORT));
