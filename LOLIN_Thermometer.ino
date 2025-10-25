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

/* 
Humidity/Temp Sensor And Display

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
*/

#include <Wire.h> /* TWI/I2C library for Arduino & Wiring */
#include "Adafruit_SHT31.h" /* SHT31 - Adafruit SHT31 Library */
#include <Adafruit_GFX.h> /* Basic graphic lib */
#include <Adafruit_SSD1306.h> /* SSD1306 Display driver */

#include "DataUtils.h" /* Measured data processing */

/* Serial connection CFG: */
#define SERIAL_BAUD_RATE 115200 /* serial connection speed [bits/sec] */
#define SERIAL_INIT_DELAY 100 /* init conection delay [ms] */

/* SHT31 Sensor CFG: */
#define SHT30_ADDRESS 0x45 /* Therm/Hydro meter I2C address */

/* Display SSD1306 CFG: */
#define SCREEN_WIDTH 128 /* OLED width in pixels */
#define SCREEN_HEIGHT 64 /* OLED height in pixels */
#define OLED_RESET -1 /* Reset pin (set on '-1', as is not connected) */
#define SCREEN_ADDRESS 0x3C /* display I2C address */
#define INIT_SCREEN_TIME 3000 /* init info displayed in sec */

/* Main loop CFG */
#define LOOP_TIME 6000 /* loop time 1sec, measurement per 1sec */
#define BLINK_TIME 100 /* blink for time 0.1 sec */ 

Adafruit_SHT31 sht30 = Adafruit_SHT31(); /* Instance for senzor */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); /* Instance for display */

void setup() {

  /* On board LED setup: */
  pinMode(LED_BUILTIN, OUTPUT);
  
  /* Init serial comunication */
  Serial.begin(SERIAL_BAUD_RATE);
  delay(SERIAL_INIT_DELAY); /* Delay to stabilize the connection */
  Serial.println("\n");
  Serial.println("Serail comunication initialized.");

  /* Init sensor SHT30 check */
  if (!sht30.begin(SHT30_ADDRESS)) {  
    Serial.println("ERROR: SHT30 senzor not found!");
    for(;;); // Hard program termination
  }
  Serial.println("OK: SHT30 sensor conected and initialized.");
  float t_init = sht30.readTemperature();
  Init_ExponentialSmooth(t_init);

  /* Display initialization */
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("ERROR: OLED displej not found!");
    while (1) delay(1); // Soft program termination (background processes ongoing)
    /* TODO (Optionaly: program termination is not necessary, measurement can still runs) */
  }

  /* Display preparation (buffer clearing & init text)*/
  display.clearDisplay();
  display.setTextSize(1); 
  display.setTextColor(SSD1306_WHITE);
  
  /* Init mesage */
  display.setCursor(0, 0); /* left upper corner */
  display.println("Lolin D1 Mini Pro");
  display.println();
  display.println("OLED: OK");
  display.println();
  display.println("I2C sensor: OK");
  display.println();
  display.setTextSize(2);
  display.print("iT: ");
  display.println(t_init);
  display.display(); /* Buffer on the display */ 
  
  delay(INIT_SCREEN_TIME);
}

void loop() {

  float t_filt = 0;
  
  digitalWrite(LED_BUILTIN, LOW);
  delay(BLINK_TIME);
  digitalWrite(LED_BUILTIN, HIGH);
    
  float t = sht30.readTemperature();
  float h = sht30.readHumidity();

  /* Check for data validity (e.g. that it is not NaN) */
  if (isnan(t) || isnan(h)) {
    Serial.println("ERROR: Error reading from sensor SHT30!");
    return; /* Skip other stuf of the current loop */
  }

  t_filt = Run_ExponentialSmooth(t);

  /* Print measred signals */  
  PrintToSerial(t, t_filt, h);
  PrintToDisplay(t_filt,h);
  
  delay(LOOP_TIME - BLINK_TIME); /* reduce the main-delay (loop) time to the blink time */
}

void PrintToDisplay(float t, float h){
  /* Screen preparation */  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(4);
  /* Temperature info */
  display.print(t);
  /* Humidity info */
  display.setCursor(0, 35);
  display.print(h);

  display.display();
}

void PrintToSerial(float t, float t_filt, float h){
  /* Temperature info */
  Serial.print("T: ");
  Serial.print(t);
  Serial.print(", T_filt: ");
  Serial.print(t_filt);
  Serial.print(" *C");

  /* Humidity info */
  Serial.print(" | H: ");
  Serial.print(h);
  Serial.println(" %");
}
