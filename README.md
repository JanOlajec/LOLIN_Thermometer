# LOLIN Thermometer (SHT31 / SSD1306)

This project is an embedded application for the LOLIN D1 Mini Pro (ESP8266) that continuously measures temperature and humidity using an SHT31 sensor. It utilizes **Exponential Smoothing** for data filtering and displays the stabilized values, raw data, and temperature trend on an I2C SSD1306 OLED display.

## Key Features

* **SHT31 Sensor Integration**: Reads current temperature and relative humidity.
* **Exponential Smoothing Filter**: Stabilizes temperature readings using a low $\alpha$ (alpha) factor (`0.03`) for high noise reduction.
* **Temperature Trend Analysis**: Implements a circular buffer (`TREND_COUNT = 10`) to determine if the temperature is rising (↑), falling (↓), or stable (-) over the last set of measurements.
* **Display Output**: Shows filtered temperature (large font), raw temperature, rounded humidity, and the current trend.
* **Modular Design**: Code is split into `DataUtils.h/.cpp` for data processing and `Global.h` for core type definitions (`UB`, `SB`, `PRIVATE`).

---

## Hardware and Configuration

| Component | Model/Chip | Connection/Address | Notes |
| :--- | :--- | :--- | :--- |
| **Microcontroller** | LOLIN(WEMOS) D1 mini Pro (ESP8266) | - | Arduino Core for ESP8266 required. |
| **Temp/Humidity Sensor** | SHT31 Digital Humidity & Temp Sensor | I2C Address: `0x45` | Uses `Adafruit SHT31` library. |
| **Display** | SSD1306 OLED Display 128x64 | I2C Address: `0x3C` | Uses `Adafruit GFX` and `Adafruit SSD1306` libraries. |

---

## Project Structure

| File | Description |
| :--- | :--- |
| `LOLIN_Thermometer.ino` | Main sketch file containing `setup()` and `loop()`, sensor initialization, and display logic. |
| `DataUtils.h` | Header file defining smoothing constants (`ALPHA`, `TREND_COUNT`) and public prototypes for data processing functions (filter, trend buffer). |
| `DataUtils.cpp` | Implementation file for the Exponential Smoothing filter and the circular buffer logic. |
| `Global.h` | Global definitions, including type aliases (`UB`, `SB`) and the `PRIVATE` macro. |

---

## License

This project is licensed under the **MIT License**.