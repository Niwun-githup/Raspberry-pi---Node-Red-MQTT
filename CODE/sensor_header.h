#ifndef R60ABD1_SENSOR_H
#define R60ABD1_SENSOR_H

#include <Arduino.h>

class R60ABD1Sensor {
 public:
  // Sensor states
  String presence_status = "";
  String motion_status = "";
  String heartbeat_rate = "";
  String respiratory_rate = "";
  String sleep_quality = "";
  String bed_status = "";

  // Variables for tracking updates
  unsigned long last_update = 0;
  const unsigned long update_interval = 5000;

  // Constructor
  R60ABD1Sensor(HardwareSerial* serial) : serial_(serial) {}

  void setup() {
    serial_->begin(9600);
  }

  void loop() {
    static uint8_t buffer[128];
    static int index = 0;

    while (serial_->available()) {
      buffer[index++] = serial_->read();
      if (index >= 7 && buffer[index - 2] == 0x54 && buffer[index - 1] == 0x43) {
        processPacket(buffer, index);
        index = 0;
      }
    }
  }

 private:
  HardwareSerial* serial_;

  void processPacket(const uint8_t* bytes, int length) {
    if (length < 7) return;

    // Bed Status
    if ((bytes[0] == 0x53) && (bytes[1] == 0x59) && (bytes[2] == 0x84) && (bytes[3] == 0x01)) {
      if (millis() - last_update > update_interval) {
        int bed_status_code = bytes[6];
        bed_status = (bed_status_code == 0) ? "Out Bed" : (bed_status_code == 1) ? "In Bed" : "Unknown";
        Serial.printf("Bed status: %d (%s)\n", bed_status_code, bed_status.c_str());
        last_update = millis();
      }
    }

    // Presence Detection
    if ((bytes[0] == 0x53) && (bytes[1] == 0x59) && (bytes[2] == 0x80) && (bytes[3] == 0x01)) {
      int presence = bytes[6];
      presence_status = (presence == 0) ? "Clear" : (presence == 1) ? "Detected" : "Unknown";
      Serial.printf("Presence status: %d (%s)\n", presence, presence_status.c_str());
    }

    // Motion Detection
    if ((bytes[0] == 0x53) && (bytes[1] == 0x59) && (bytes[2] == 0x80) && (bytes[3] == 0x02)) {
      int motion = bytes[6];
      motion_status = (motion == 0) ? "Clear" : (motion == 1) ? "Detected" : (motion == 2) ? "Strong" : "Unknown";
      Serial.printf("Motion status: %d (%s)\n", motion, motion_status.c_str());
    }

    // Heartbeat Rate
    if ((bytes[0] == 0x53) && (bytes[1] == 0x59) && (bytes[2] == 0x85) && (bytes[3] == 0x02)) {
      int heartbeat = bytes[6];
      heartbeat_rate = String(heartbeat) + " bpm";
      Serial.printf("Heartbeat rate: %d bpm\n", heartbeat);
    }

    // Respiratory Rate
    if ((bytes[0] == 0x53) && (bytes[1] == 0x59) && (bytes[2] == 0x81) && (bytes[3] == 0x02)) {
      int respiratory = bytes[6];
      respiratory_rate = String(respiratory) + " b/m";
      Serial.printf("Respiratory rate: %d b/m\n", respiratory);
    }

    // Sleep Quality
    if ((bytes[0] == 0x53) && (bytes[1] == 0x59) && (bytes[2] == 0x84) && (bytes[3] == 0x06)) {
      int sleep_quality_code = bytes[6];
      sleep_quality = (sleep_quality_code == 0) ? "Poor" : (sleep_quality_code == 1) ? "Fair" : (sleep_quality_code == 2) ? "Good" : "Unknown";
      Serial.printf("Sleep quality: %s\n", sleep_quality.c_str());
    }
  }
};

#endif
