/*
 * ESP8266-MQTT-SENSORS for Weather Station (Environmental Monitoring & Automatic Control).
 * 
 * D3(SCL/GPIO5)  : Clock - SHT1x
 * D4(SDA/GPIO4)  : Data - SHT1x
 * GPIO14         : Output controls the pumps.
 * 
 */
// Depends on the following Arduino libraries:
// - https://github.com/practicalarduino/SHT1x
// - http://pubsubclient.knolleary.net/

// Set configuration options for WiFi, MQTT, DHT and GPIOs in the following file:
#include "config.h"
#include <String.h>

// WiFi
#include <ESP8266WiFi.h>
WiFiClient espClient;

// MQTT
#include <PubSubClient.h>
PubSubClient client(espClient);

// SHT1x sensor
#include <SHT1x.h>
SHT1x sht1x(CONFIG_SHT_DATA_PIN, CONFIG_SHT_CLOCK_PIN);
float temp_c = 0.0;
float temp_f = 0.0;
float humidity = 0.0;

// Millis
unsigned long lastSampleTime = 0;
char sampleIdx = 0;

// GPIO's status
boolean pumpStatus = false;

// ----------------------------------- GET-PAYLOAD-FROM-MQTT VROKER() ---------------------//
//void receivedMsg(char* topic, unsigned char* payload, unsigned int payloadLength)
void receivedMsg(char* topic, byte* payload, unsigned int payloadLength)
{
    // Connvert payload in char[] to String
    String payloadStr = String((char*)payload);
    // Payload without whitespace
    payloadStr.trim();
    // Get boolean status of the devices
    boolean deviceStatus = payloadStr.toInt();
    // Control the devices: (device_status = 1 --> ON) else OFF
    if (strcmp(topic, CONFIG_PUMP_TOPIC) == 0)
    {
      if (deviceStatus != pumpStatus)
      {
        pumpStatus = deviceStatus;
        // Set pump_GPIO HIGH-logic or LOW-logic
        digitalWrite(CONFIG_IO_PUMP, pumpStatus);
      }
      // Publish pumpStatus to MQTT broker
      client.publish(CONFIG_PUMP_STATUS_TOPIC, String(deviceStatus).c_str());
      Serial.println("--------------------");
      Serial.print("PUMP rec_status: "); Serial.println(deviceStatus);
    }
}
// ----------------------------------- SETUP() - RUN ONCE -----------------------------------//
void setup() {
  // GPIOs as OUTPUT
  pinMode(CONFIG_IO_PUMP, OUTPUT);

  // Initilize baud-rate of Serial interface
  Serial.begin(115200);
  
  // Wifi
  setupWifi();
  
  // MQTT
  client.setServer(CONFIG_MQTT_HOST, 1883);
  client.setCallback(receivedMsg);
}
// ----------------------------------- SETUP-WIFI() -------------------------------------------//
void setupWifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(CONFIG_WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
// ----------------------------------- CONNECT-TO-MQTT BROKER() -------------------------------//
void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(CONFIG_MQTT_CLIENT_ID, CONFIG_MQTT_USER, CONFIG_MQTT_PASS))
    {
      Serial.println("connected");
      client.subscribe(CONFIG_SUBSCRIBE_TOPIC);
    } else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
// ----------------------------------- LOOP() - LOOP FOREVER -----------------------------------//
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();
  int k;

  if (currentMillis >= (lastSampleTime + CONFIG_SAMPLE_DELAY))
  {
    lastSampleTime = currentMillis;

    switch(++sampleIdx)
    {
      case 3:
      {
        sampleIdx = 0;
      }
      case 1:
      {
        // Publish deviceStatus to MQTT broker
        client.publish(CONFIG_PUMP_STATUS_TOPIC, String(pumpStatus).c_str());
        Serial.println("--------------------");
        Serial.print("PUMP send_status: "); Serial.println(pumpStatus);
        break;
      }
      case 2:
      {
        for (k = 0; k < 5; k++)
        {
          temp_c = sht1x.readTemperatureC();
          temp_f = sht1x.readTemperatureF();
          if (temp_c < 0)
          {
            //Serial.println("Error reading temperature_C!");
            temp_c = -1;
          }
          if (temp_f < 0)
          {
            //Serial.println("Error reading temperature_F!");
            temp_f  = -1;
          }
          delay(1000);
        }

        for (k = 0; k < 5; k++)
        {
          humidity = sht1x.readHumidity();
          if (humidity < 0 || humidity > 100)
          {
            //Serial.println("Error reading humidity!");
            humidity = -1;
          }
          delay(1000);
        }
       
        // Build JSON string
        String json = buildJson();
        client.publish(CONFIG_MQTT_TOPIC, json.c_str());
   
        Serial.print("Temperature: ");
        Serial.print(temp_c, DEC);
        Serial.print("C / ");
        Serial.print(temp_f, DEC);
        Serial.print("F. Humidity: ");
        Serial.print(humidity);
        Serial.println("%");
        break;
      } // End case
    }   // End switch
  }     // End if-millis
}       // End loop
// -------------------------------- BUILD JSON -------------------------------------------------//
String buildJson()
{
  String data = "{";
  data += "\"air_temperature\":";
  data += String(temp_c).c_str();
  data += ",";
  data += "\"air_humidity\":";
  data += String(humidity).c_str();
  data += "}";
  return data;
}
// ------------------------------- THE END -----------------------------------------------------//

