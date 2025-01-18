#include <WiFi.h>
#include <PubSubClient.h>
#include "R60ABD1.h" // Include the custom library

// WiFi credentials
const char* ssid = "Office_solo";
const char* password = "solo1234";

// MQTT server credentials
const char* mqtt_server = "192.168.100.178";
const int mqtt_port = 1883;
const char* mqtt_user = "raspi5nr";
const char* mqtt_password = "12345";
const char* topic = "test";

// WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

// กำหนด UART และขา GPIO
#define TX_PIN 17 // ใช้หมายเลข GPIO โดยตรง
#define RX_PIN 16 // ใช้หมายเลข GPIO โดยตรง

HardwareSerial mySerial(1); // ใช้ Serial1

// R60ABD1 sensor instance
R60ABD1 sensor(mySerial); // ใช้ Serial1 กับเซ็นเซอร์

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN); // กำหนด TX/RX
  
  // Initialize the sensor
  sensor.begin();

  Serial.println("R60ABD1 Sensor Initialized");

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Update the sensor data
  sensor.update(); // Retrieve fresh data from the sensor

  // Retrieve sensor data
  int presence = sensor.getPresenceStatus();
  int motion = sensor.getMotionStatus();
  int heartbeat = sensor.getHeartbeatRate();
  int respiratory = sensor.getRespiratoryRate();
  int sleepQuality = sensor.getSleepQuality();
  int bedStatus = sensor.getBedStatus();

  // Log data
  Serial.println("Presence Status: " + String(presence));
  Serial.println("Motion Status: " + String(motion));
  Serial.println("Heartbeat Rate: " + String(heartbeat));
  Serial.println("Respiratory Rate: " + String(respiratory));
  Serial.println("Sleep Quality: " + String(sleepQuality));
  Serial.println("Bed Status: " + String(bedStatus));
  Serial.println("-------------------------");

  // Prepare payload for MQTT
  char payload[512];
  snprintf(payload, sizeof(payload),
           "{ \"presence_status\": \"%d\", \"motion_status\": \"%d\", \"heartbeat_rate\": %d, \"respiratory_rate\": %d, \"sleep_quality\": %d, \"bed_status\": \"%d\" }",
           presence,
           motion,
           heartbeat,
           respiratory,
           sleepQuality,
           bedStatus);

  // Publish payload
  if (client.publish(topic, payload)) {
    Serial.println("Message sent successfully:");
    Serial.println(payload);
  } else {
    Serial.println("Message failed to send");
  }

  delay(5000);  // Publish every 5 seconds
}
