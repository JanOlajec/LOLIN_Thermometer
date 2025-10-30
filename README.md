# LOLIN Thermometer (SHT31 / SSD1306)

This project is an embedded application for the LOLIN D1 Mini Pro (ESP8266) that continuously measures temperature and humidity using an SHT31 sensor. It utilizes **Exponential Smoothing** for data filtering and displays the stabilized values, raw data, and temperature trend on an I2C SSD1306 OLED display.

## Key Features

* **SHT31 Sensor Integration**: Reads current temperature and relative humidity
* **Data Processing**:
  * Exponential Smoothing Filter: Stabilizes temperature readings using a low $\alpha$ (alpha) factor of `0.03` for high noise reduction
  * Round-to-decimal precision control 
  * Temperature trend analysis with circular buffer (10 samples)
* **Display Output**: Shows filtered temperature (large font), raw temperature, humidity, and trend indicators (↑,↓,-)
* **MQTT Integration**:
  * Publishes temperature and humidity data as JSON payloads
  * Topics: `home/thermometer/temperature`, `home/thermometer/humidity`
  * Status reporting on `home/thermometer/status`
* **Node-RED Dashboard**: Includes flow configuration for data visualization

## Hardware Configuration

| Component | Configuration | Details |
|-----------|--------------|----------|
| **MCU** | LOLIN D1 Mini Pro v2 | ESP8266 based board |
| **Temperature Sensor** | SHT31 | I2C Address: `0x45` |
| **Display** | SSD1306 OLED 128x64 | I2C Address: `0x3C` |
| **Network** | WiFi + MQTT | Static IP: 192.168.241.120 |

## Project Structure

| File | Description |
|------|-------------|
| `LOLIN_Thermometer.ino` | Main application with setup/loop and MQTT handling |
| `DataUtils.h/.cpp` | Data processing utilities (smoothing, trend analysis) |
| `Global.h` | Type definitions and global configuration |
| `NodeRed_flow_LOLINTmpr.json` | Node-RED dashboard configuration |

## Data Processing Features

### Exponential Smoothing
The project uses exponential smoothing with the formula:

$filtered = (\alpha \times raw) + ((1 - \alpha) \times previous)$

Where $\alpha = 0.03$ for strong noise reduction.

### Temperature Trend Analysis
* Buffer Size: 10 samples
* Threshold: ±0.1°C change
* Indicators: Rising (↑), Falling (↓), Stable (-)

## Dependencies

* `Wire.h` - I2C communication
* `Adafruit_SHT31` - Temperature sensor driver
* `Adafruit_GFX` - Graphics library
* `Adafruit_SSD1306` - OLED display driver
* `ESP8266WiFi` - WiFi connectivity
* `PubSubClient` - MQTT client
* `ArduinoJson` - JSON serialization

## License

This project is licensed under the MIT License.

Copyright (c) 2025 Jan OLAJEC, All rights reserved.