/**
 * @file LOLIN_Thermometer.ino
 * @brief Main sketch for reading temperature/humidity and displaying data on OLED.
 * This project implements data smoothing (Exponential Smoothing) 
 * for the SHT30 sensor readings and displays the stabilized values on an 
 * I2C SSD1306 OLED display.
 * * ====================================
 * * HARDWARE & CONFIGURATION
 * * ====================================
 * * @board LOLIN(WEMOS) D1 mini Pro (ESP8266)
 * * @sensor SHT31 Digital Humidity & Temp Sensor (I2C address: 0x45)
 * * @display SSD1306 OLED Display 128x64 (I2C address: 0x3C)
 * * @libraries Adafruit SHT31, Adafruit GFX, Adafruit SSD1306, DataUtils
 * @author Jan Olajec
 * @date 23.10.2025
 * @copyright Copyright (c) 2025 Jan OLAJEC, All rights reserved.
 * @license This project is licensed under the MIT License
 */

/* Humidity/Temp Sensor And Display

Board setup:
Board: Lolin D1 Mini Pro v2
- Add support for LOLIN: http://arduino.esp8266.com/stable/package_esp8266com_index.json
- install board "esp8266 by ESP8266 Community" with Board manger
- select board "LOLIN(WEMOS) D1 mini Pro"
- port: based on chip CH340G

SHT30 Shield setup (SHT31 Digital Humidity & Temp Sensor):
- install lib: SHT31 - Adafruit SHT31 Library
- address: 0x8A (7bit: 0x45)

Display setup:
- display SSD1306 / IIC
- graph lib: Adafruit GFX Library
- display driver: Adafruit SSD1306
- addres 0x78 (7bit: 0x3C)

MQTT:
- Add lib: PubSubClient (Author: Nick O'Leary)
- Add lib: ArduinoJson.h (Author: Benoit Blanchon)
*/

// --------------------------------------------------------------------------
// INCLUDES
// --------------------------------------------------------------------------

#include <Wire.h> /* TWI/I2C library for Arduino & Wiring */
#include "Adafruit_SHT31.h" /* SHT31 - Adafruit SHT31 Library */
#include <Adafruit_GFX.h> /* Basic graphic lib */
#include <Adafruit_SSD1306.h> /* SSD1306 Display driver */
#include <ESP8266WiFi.h> /* WiFi lib for ESP8266 */
#include <PubSubClient.h> /* MQTT client */
#include <ArduinoJson.h> /* JSON lib */

#include "Global.h"
#include "DataUtils.h" /* Measured data processing */

// --------------------------------------------------------------------------
// CONSTANTS / CONFIGURATION
// --------------------------------------------------------------------------

/* Serial connection configuration: */
#define SERIAL_BAUD_RATE 115200 /* Serial connection speed [bits/sec] */
#define SERIAL_INIT_DELAY 100 /* Initial connection delay [ms] */

/* SHT31 Sensor configuration: */
#define SHT30_ADDRESS 0x45 /* Therm/Hygrometer I2C address */

/* Display SSD1306 configuration: */
#define SCREEN_WIDTH 128 /* OLED width in pixels */
#define SCREEN_HEIGHT 64 /* OLED height in pixels */
#define OLED_RESET -1 /* Reset pin (set to '-1' as it is not connected) */
#define SCREEN_ADDRESS 0x3C /* Display I2C address */
#define INIT_SCREEN_TIME 2000 /* Initial info displayed duration [ms] */

/* Main loop configuration */
#define LOOP_TIME 6000 /* Loop time in milliseconds */
#define BLINK_TIME 100 /* Blink duration for the on-board LED [ms] */ 

/* MQTT broker (Mosquitto) on Raspberry Pi */
#define MQTT_SERVER "192.168.241.111" /* IP address Raspberry Pi (Mosquitto Broker) */
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "LolinThermometer_01" /* Unique ID for this senzor */
#define MQTT_TOPIC_TEMP "home/thermometer/temperature"
#define MQTT_TOPIC_HUM "home/thermometer/humidity"
#define MQTT_TOPIC_STATUS "home/thermometer/status"

/* WiFi */
#define WIFI_ACTIVE 1 /* Enable/Disable WIFI conection */
/* WiFi disabled (0) - For debug purpose over serial cable connection. 
 * WiFi enabled  (1) - Establish WiFi connection and MQTT connection.
 */
#if (WIFI_ACTIVE == 1)
#define WIFI_SSID "YOUR_WIFI_SSID" 
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#endif

// --------------------------------------------------------------------------
// MAIN DATA
// --------------------------------------------------------------------------

Adafruit_SHT31 sht30 = Adafruit_SHT31(); /* Instance for the SHT31 sensor */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); /* Instance for the SSD1306 display */

#if (WIFI_ACTIVE == 1)
WiFiClient espClient; /* WiFi Client */
PubSubClient client(espClient); /* MQTT client is using WiFI client */
#endif
// --------------------------------------------------------------------------
// FUNCTION PROTOTYPES
// --------------------------------------------------------------------------

void PrintToDisplay(float t_filt, float t_raw, UB h);
void PrintToSerial(float t, float t_filt, UB h);
#if (WIFI_ACTIVE == 1)
void WifiSetup();
void ReconnectMqtt();
void PublishData(float tmpr, UB hum);
#endif

// --------------------------------------------------------------------------
// MAIN FUNCTIONS
// --------------------------------------------------------------------------

void setup() {

  /* On board LED setup: */
  pinMode(LED_BUILTIN, OUTPUT);
  
  /* Initialize serial communication */
  Serial.begin(SERIAL_BAUD_RATE);
  delay(SERIAL_INIT_DELAY); /* Delay to stabilize the connection */
  Serial.println("\n");
  Serial.println("Serial communication initialized.");
  
  /* Initialize SHT31 sensor and check connection */
  if (!sht30.begin(SHT30_ADDRESS)) {  
    Serial.println("ERROR: SHT31 sensor not found!");
    for(;;); // Hard program termination
  }
  Serial.println("OK: SHT31 sensor connected and initialized.");
  
  float t_init = sht30.readTemperature();
  float h_init = sht30.readHumidity();

  /* Check for data validity (e.g., is not NaN) */
  if (isnan(t_init) || isnan(h_init)) {
    Serial.println("ERROR: Error reading from sensor SHT31!");
    for(;;); // Hard program termination
  }
  Serial.println("OK: SHT31 sensor data OK.");

  t_init = RoundToDecimals(t_init, 2);
  Init_TmprSmooth(t_init);
  Init_TmprTrendBuffer(t_init);
    
  h_init = RoundToDecimals(h_init, 2);
  Init_HumiSmooth(h_init);

  /* Initialize the OLED display */
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("ERROR: OLED display not found!");
    while (1) delay(1);
    /* TODO (Optionaly: program termination is not necessary, measurement can still runs) */
  }
  
  /* Display preparation (buffer clearing & init text)*/
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  /* Init mesage */
  display.setCursor(0, 0); /* Set cursor to the top-left corner */
  display.println("Lolin D1 Mini Pro");
  display.println();
  display.println("OLED: OK");
  display.println();
  display.println("I2C sensor: OK");
  display.println();
  display.setTextSize(2);
  display.print("iT: ");
  display.println(t_init);
  display.display(); /* Update the display buffer */ 

  delay(INIT_SCREEN_TIME);
  display.clearDisplay();

#if (WIFI_ACTIVE == 1)
  display.setTextSize(1);
  display.setCursor(0, 0); /* Set cursor to the top-left corner */
  display.println("Setup WIFI...");
  display.display(); /* Update the display buffer */ 

  WifiSetup();
  
  display.println("Setup MQTT...");
  display.display(); /* Update the display buffer */ 

  client.setServer(MQTT_SERVER, MQTT_PORT);
  
  display.println("Setup OK");
  display.display(); /* Update the display buffer */ 
  
  delay(INIT_SCREEN_TIME);
  display.clearDisplay();
#endif /* WIFI_ACTIVE */
}

void loop() {

  float t_filt = 0; // Filtered temperature value
  float h_filt = 0;
  UB h_round = 0; // Rounded humidity value (UB type)

#if (WIFI_ACTIVE == 1)
  /* Check MQTT connection */
  if (!client.connected()) {
    /* Try to reconnect */
    ReconnectMqtt();
  }
  /* MQTT client processing of incoming/outgoing packets */
  client.loop();
#endif /* WIFI_ACTIVE */
  
  digitalWrite(LED_BUILTIN, LOW);
  delay(BLINK_TIME);
  digitalWrite(LED_BUILTIN, HIGH);
    
  float t = sht30.readTemperature();
  float h = sht30.readHumidity();
  
  /* Check for data validity (e.g., is not NaN) */
  if (isnan(t) || isnan(h)) {
    Serial.println("ERROR: Error reading from sensor SHT31!");
    return; /* Skip the rest of the current loop iteration */
  }

  t = RoundToDecimals(t, 2);
  AddTmprToTrendBuffer(t);
  t_filt = Run_TmprSmooth(t);
  t_filt = RoundToDecimals(t_filt, 2);
  
  h = RoundToDecimals(h, 2);
  h_filt = Run_HumiSmooth(h);
  h_filt = RoundToDecimals(h_filt, 2);

  /* Cast humidity to integer (UB type) */
  h_round = (UB)round(h);
  
  /* Clamp humidity to 100% */
  if (h_round > 100) {
    h_round = 100;
  }

  /* Print measured signals */  
  PrintToSerial(t, t_filt, h_filt);
  PrintToDisplay(t_filt, t, h_round);

#if (WIFI_ACTIVE == 1)
  /* Send MQTT data to MQTT broker */
  if (client.connected()) {
    /* Publish only in case of available client connection */
    PublishData(t_filt, h_filt);
  }
#endif /* WIFI_ACTIVE */
  
  delay(LOOP_TIME - BLINK_TIME);
  /* Adjust main loop delay time for blink duration */
}

// --------------------------------------------------------------------------
// MAIN HELPER FUNCTIONS
// --------------------------------------------------------------------------

/**
 * @brief Prints current temperature and humidity values to the OLED display.
 * @param t_filt The filtered temperature value.
 * @param t_raw The raw temperature value.
 * @param h The rounded humidity value (0-100%).
 */
void PrintToDisplay(float t_filt, float t_raw, UB h){
  /* Prepare display for output */  
  display.clearDisplay();
  
  /* Line 1: Filtered Temperature (Large font) */
  display.setCursor(0, 0);
  display.setTextSize(4);
  display.print(t_filt);
  
  /* Line 2: Raw Temperature and Humidity (Smaller font) */
  display.setCursor(0, 35);
  display.setTextSize(2);
  display.print(t_raw);
  display.print("C ");
  display.print(h);
  display.println("%");
  
  /* Line 3: Temperature Trend Indicator */
  SB trend = GetTemperatureTrend();
  if (1 == trend) {
    display.write(24); // Arrow UP symbol (24)
  } else if (-1 == trend) {
    display.write(25); // Arrow DOWN symbol (25)
  } else {
    display.print("-"); /* Stable (no significant change) */
  }

  display.print(" a:");
  display.print(ALPHA);

  display.display();  // Update the OLED display
}

/**
 * @brief Prints current temperature and humidity values to the Serial Monitor.
 * @param t The raw temperature value.
 * @param t_filt The filtered temperature value.
 * @param h The rounded humidity value (0-100%).
 */
void PrintToSerial(float t, float t_filt, float h){
  /* Temperature info */
  Serial.print("T_raw: ");
  Serial.print(t, 2); /* number of decimal places */
  Serial.print(" C, T_filt: ");
  Serial.print(t_filt, 2);
  Serial.print(" C");

  /* Humidity info */
  Serial.print(" | H_filt: ");
  Serial.print(h, 2);
  Serial.print(" %");

  /* DEBUG: Checking Trend-buffer values */
  // Serial.print(" Buffer: ");
  // const float* buffer = GetTmprTrendBuffer();
  // for (UB i = 0; i < TREND_COUNT; i++) {
  //   Serial.print(buffer[i]);
  //   Serial.print(" ");
  // }

  Serial.println();
}

#if (WIFI_ACTIVE == 1)
// --------------------------------------------------------------------------
// WIFI/MQTT FUNCTIONS
// --------------------------------------------------------------------------

/**
 * @brief Handles connection to the local WiFi network.
 */
void WifiSetup() {
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  IPAddress local_IP(192, 168, 241, 120);
  IPAddress gateway(192, 168, 241, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(192, 168, 241, 1);

  // Static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS)) {
    Serial.println("Static IP configuration failed!");
  }

  WiFi.mode(WIFI_STA); /* LOLIN ESP8266 as client */
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    // TODO add retries
    delay(500);
    Serial.print(".");
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("Connected to AP: ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Signal: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.print("Mac: ");
  Serial.println(WiFi.macAddress());
  Serial.println("");
}

/**
 * @brief Attempts to reconnect to the MQTT broker if the connection is lost.
 */
void ReconnectMqtt() {
  /* Loop until we're reconnected */
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    /* Attempt to connect */
    if (client.connect(MQTT_CLIENT_ID, "username_optional", "password_optional")) {
      Serial.println("connected");
      /* Publish status on connection */
      client.publish(MQTT_TOPIC_STATUS, "Device connected and operational.");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/**
 * @brief Publishes the measured data to the MQTT broker as JSON.
 * @param tmpr Temperature value.
 * @param hum Humidity value.
 */
void PublishData(float tmpr, float hum) {
  /* Create JSON doc with dynamic alocation (max. 100 Bytes) */
  StaticJsonDocument<100> doc;
  
  doc["value"] = tmpr;
  doc["unit"] = "C";
  
  /* Serialize JSON na string */
  char jsonBuffer[100];
  serializeJson(doc, jsonBuffer);
  
  /* Send to MQTT */
  client.publish(MQTT_TOPIC_TEMP, jsonBuffer);

  doc.clear();

  doc["value"] = hum;
  doc["unit"] = "%";
  serializeJson(doc, jsonBuffer);
  client.publish(MQTT_TOPIC_HUM, jsonBuffer);
  
  Serial.print("MQTT published. T:"); 
  Serial.print(tmpr);
  Serial.print(", H:"); 
  Serial.println(hum);
}
#endif /* WIFI_ACTIVE */