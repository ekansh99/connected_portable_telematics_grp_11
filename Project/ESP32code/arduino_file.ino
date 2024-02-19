#include <WiFi.h>
#include <PubSubClient.h>  
#include <esp_wifi.h>  
#include <ArduinoJson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <HardwareSerial.h>

DynamicJsonDocument sensor_data_payload(1024);

String arr;
char *token;
int a, b, c;
float m, n, o;

char sensor_data_format_for_mqtt_publish[1024];

const char* ssid =   "OnePlus";       //ssid - service set Identifier (Replace it with your ssid name)

const char* password =  "p123456789";        // replace with ssid paasword

const char* mqttBroker = "mqtt.thingsboard.cloud";       // broker address - replace it with your broker address/cloud broker - test.mosquitto.org192.168.43.75

const int   mqttPort = 1883;       // broker port number

const char* clientID = "projectTelematics";      // client-id --> replace it in case willing to connect with same broker

const char* username = "Project_Connected_Portable_Telematics_Device";
const char* mqtt_topic_for_publish = "v1/devices/me/telemetry"; // topic namescdac/room/data

WiFiClient MQTTclient;

PubSubClient client(MQTTclient);

long lastReconnectAttempt = 0;
boolean reconnect()
{
  if (client.connect(clientID,username,password)){

    Serial.println("Attempting to connect broker");
    
  }
  return client.connected();
}

void setup() {
  Serial.begin(57600);//baud rate bits per sec for console
  Serial2.begin(57600);//baud rate bits per sec for UART
  Serial.println("Attempting to connect...");
  WiFi.mode(WIFI_STA);      
  //esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]); // for wifi spoofing
  WiFi.begin(ssid, password); // Connect to WiFi.
  
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Couldn't connect to WiFi.");
  }
  Serial.print("ESP32 IP ADDRESS : ");
  Serial.println(WiFi.localIP());
  //Add details for MQTT Broker
  client.setServer(mqttBroker, mqttPort); // Connect to broker
  lastReconnectAttempt = 0;
}
void loop() {
  char charArr[1000];
  if (!client.connected()) //if device is not connected to broker
  {
    long now = millis();  // Returns the number of milliseconds passed since the Arduino board began running the current program
    if (now - lastReconnectAttempt > 5000) { // Try to reconnect.
      lastReconnectAttempt = now;
      if (reconnect())
      { 
        lastReconnectAttempt = 0;
      }
    }
  }
  else 
  { 
    Serial.println("Connected to Broker --- !!");
    client.loop();
if (Serial2.available()) {
   arr = Serial2.readStringUntil('\n');
    Serial.println("Received data: " + arr);
    arr.toCharArray(charArr, sizeof(charArr));
  }   
// Extracting substrings using strtok
    token = strtok(charArr, ",");
    a = atoi(token);//X Axis
    token = strtok(NULL, ",");
    b = atoi(token);//Y Axis
    token = strtok(NULL, ",");
    c = atoi(token);//Z Axis
    token = strtok(NULL, ",");
    m = atof(token);//Temperature
    token = strtok(NULL, ",");
    n = atof(token);//NO2
    token = strtok(NULL, ",");
    o = atof(token);//Speed

    // Printing the variables
    Serial.print("X Axis: ");
    Serial.println(a);
    Serial.print("Y Axis: ");
    Serial.println(b);
    Serial.print("Z Axis: ");
    Serial.println(c);
    Serial.print("Temperature: ");
    Serial.println(m);
    Serial.print("NO2: ");
    Serial.println(n);
    Serial.print("Speed: ");
    Serial.println(o);
   
    sensor_data_payload["X axis"]   = a;  
    sensor_data_payload["Y axis"]   = b;  
    sensor_data_payload["Z axis"]   = c;
    sensor_data_payload["Temperature"]   = m;
    sensor_data_payload["NO2"]   = n;
    sensor_data_payload["Speed"]   = o;
    serializeJson(sensor_data_payload, sensor_data_format_for_mqtt_publish);
                  
    delay(500);
    client.publish(mqtt_topic_for_publish,sensor_data_format_for_mqtt_publish);  //(topicname, payload)
    Serial.println("sensor data sent to broker");
    delay(500);
  }
}
