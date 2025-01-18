#include <Arduino.h>
#include "R60ABD1.h"

// กำหนด UART และขา GPIO
#define TX_PIN 17 // ใช้หมายเลข GPIO โดยตรง
#define RX_PIN 16 // ใช้หมายเลข GPIO โดยตรง

HardwareSerial mySerial(1); // ใช้ Serial1

R60ABD1 sensor(mySerial); // ใช้ Serial1 กับเซ็นเซอร์

void setup() {
  Serial.begin(115200); // เปิด Serial Monitor
  mySerial.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN); // กำหนด TX/RX

  sensor.begin(); // เริ่มต้นเซ็นเซอร์
  Serial.println("R60ABD1 Sensor Initialized");
}

void loop() {
  Serial.println("Reading sensor data...");
  sensor.update(); // อ่านข้อมูลจากเซ็นเซอร์

  // แสดงข้อมูลจากเซ็นเซอร์
  Serial.println("Presence Status: " + String(sensor.getPresenceStatus()));
  Serial.println("Motion Status: " + String(sensor.getMotionStatus()));
  Serial.println("Heartbeat Rate: " + String(sensor.getHeartbeatRate()));
  Serial.println("Respiratory Rate: " + String(sensor.getRespiratoryRate()));
  Serial.println("Sleep Quality: " + String(sensor.getSleepQuality()));
  Serial.println("Bed Status: " + String(sensor.getBedStatus()));
  Serial.println("-------------------------");

  delay(5000); // รอ 5 วินาทีก่อนอ่านข้อมูลใหม่
}
