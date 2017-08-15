/*
 * This is a sample configuration file for the "mqtt_esp8266_sensors" sensor.
 *
 * Change the settings below and save the file as "config.h"
 * You can then upload the code using the Arduino IDE.
 */

// WiFi
#define CONFIG_WIFI_SSID "Xom nha la"             // SSID of the wi-fi hostpot
#define CONFIG_WIFI_PASS "AgriConnect"            // Passwork of the wi-fi hostpot
//#define CONFIG_WIFI_SSID "FPT-TanPhuc"
//#define CONFIG_WIFI_PASS "01666863453"

// MQTT
#define CONFIG_MQTT_HOST "mqtt.agriconnect.vn"    // MQTT broker
#define CONFIG_MQTT_USER "node"                   // User - connect to MQTT broker
#define CONFIG_MQTT_PASS "654321"                 // Passwork - connect to MQTT broker
#define CONFIG_MQTT_CLIENT_ID "longthanhnode01"          // Must be unique on the MQTT network

// MQTT Topics
#define CONFIG_MQTT_TOPIC "longthanh/r1/n/01"          // MQTT-Msg-Topic - publish the envi-data to MQTT broker
#define CONFIG_PUMP_STATUS_TOPIC "longthanh/r1/ds/01"  // MQTT-Msg-Topic - publish the pump-status to MQTT broker

#define CONFIG_SUBSCRIBE_TOPIC "longthanh/r1/d/#"      // MQTT-Msg-Topic - subscribe the device-status from MQTT broker
#define CONFIG_PUMP_TOPIC "longthanh/r1/d/01"          // MQTT-Msg-Topic - subscribe the pump-status from MQTT broker

// DELAY PERIOD
#define CONFIG_SAMPLE_DELAY 10000                 // Milliseconds step - 10 seconds

// SHT1x sensor
#define CONFIG_SHT_DATA_PIN  D4
#define CONFIG_SHT_CLOCK_PIN D3

// GPIOs
#define CONFIG_IO_PUMP D13                         // GPIO14 of ESP8266 which controls the PUMP
