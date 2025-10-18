import React, {useEffect, useState} from 'react';
import io from 'socket.io-client';

const socket = io('http://localhost:4000');

export default function App(){
  const [latest, setLatest] = useState(null);

  useEffect(()=>{
    socket.on('new-reading', (r) => setLatest(r));
    fetch('/api/latest/tanaya-device-01').then(r=>r.json()).then(setLatest).catch(()=>{});
  },[]);

  if(!latest) return <div>Waiting for data...</div>;
  return (
    <div style={{fontFamily:'sans-serif',padding:20}}>
      <h2>Device: {latest.device_id}</h2>
      <div>Heart rate: {latest.heart_rate}</div>
      <div>SpO2: {latest.spo2}</div>
      <div>Air temp (°C): {latest.air_temp_c}</div>
      <div>Skin temp (°C): {latest.skin_temp_c}</div>
      <div>Humidity: {latest.air_humidity}</div>
      <div>At: {new Date(latest.timestamp).toLocaleString()}</div>
    </div>
  );
}
