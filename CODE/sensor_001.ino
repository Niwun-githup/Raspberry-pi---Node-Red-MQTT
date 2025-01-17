#include <WiFi.h>
#include <PubSubClient.h>
#include  "sensor_header.h"

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

// UART instance for the sensor
HardwareSerial uart(1); // Use UART1 (TX=GPIO17, RX=GPIO16)

// R60ABD1Sensor instance
R60ABD1Sensor sensor(&uart);

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
  uart.begin(9600, SERIAL_8N1, 16, 17);  // Configure UART1 for the sensor
  sensor.setup();  // Set up the sensor

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Call the sensor loop to process data
  sensor.loop();

  // Prepare payload
  char payload[512];
  snprintf(payload, sizeof(payload),
           "{ \"bed_status\": \"%s\", \"motion_status\": \"%s\", \"heartbeat_rate\": \"%s\", \"respiratory_rate\": \"%s\", \"sleep_quality\": \"%s\" }",
           sensor.bed_status.c_str(),
           sensor.motion_status.c_str(),
           sensor.heartbeat_rate.c_str(),
           sensor.respiratory_rate.c_str(),
           sensor.sleep_quality.c_str());

  // Publish payload
  if (client.publish(topic, payload)) {
    Serial.println("Message sent successfully:");
    Serial.println(payload);
  } else {
    Serial.println("Message failed to send");
  }

  delay(5000);  // Publish every 5 seconds
}
