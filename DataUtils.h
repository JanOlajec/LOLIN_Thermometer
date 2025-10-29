/**
 * @file DataUtils.h 
 * @brief Header file for data processing utility functions.
 * This file contains function declarations and constants for smoothing 
 * sensor data, primarily using the Exponential Smoothing method.
 * @author Jan Olajec
 * @date 23.10.2025
 * @copyright Copyright (c) 2025 Jan OLAJEC, All rights reserved.
 * @license This project is licensed under the MIT License
 */

#ifndef DATA_UTILS_H
#define DATA_UTILS_H

// --------------------------------------------------------------------------
// INCLUDES
// --------------------------------------------------------------------------
#include "Global.h"

// --------------------------------------------------------------------------
// CONSTANTS
// --------------------------------------------------------------------------

/**
 * @brief Default initial temperature value used before first sensor reading.
 */
#define DEFAULT_TEMP 20.0

/**
 * @brief Smoothing factor (Alpha) for the Exponential Smoothing filter.
 * * A lower value (closer to 0) results in stronger smoothing 
 * (more historical data influence), while a higher value (closer to 1) 
 * results in weaker smoothing (more current raw data influence).
 */
#define ALPHA 0.02

/**
 * @brief Number of measurements stored in the circular trend buffer.
 */
#define TREND_COUNT 10

/**
 * @brief The threshold (in degrees Celsius) used to define a temperature trend change.
 */
#define TREND_THRESHOLD 0.1

// --------------------------------------------------------------------------
// PUBLIC FUNCTION PROTOTYPE
// --------------------------------------------------------------------------

/**
 * @brief Initializes the internal filtered value for the Exponential Smoothing filter.
 * @param t_init The initial (starting) temperature to set as the first stable value.
 */
void Init_ExponentialSmooth(float t_init);

/**
 * @brief Performs Exponential Smoothing on raw sensor data.
 * @param rawValue The current raw measured temperature value.
 * @return float The new, smoothed (filtered) value.
 */
float Run_ExponentialSmooth(float rawValue);

/**
 * @brief Initializes the circular trend buffer with an initial temperature value.
 * @param t_init The initial temperature to fill the buffer with.
 */
void Init_TmprTrendBuffer(float t_init);

/**
 * @brief Adds a new temperature value to the circular buffer and advances the index.
 * @param newTmpr The new temperature value to store.
 */
void AddTmprToTrendBuffer(float newTmpr);

/**
 * @brief Calculates the temperature trend by comparing the newest and oldest values in the buffer.
 * @return SB Returns 1 (rising), -1 (falling), or 0 (stable).
 */
SB GetTemperatureTrend(void);

/**
 * @brief Gets a constant pointer to the internal temperature trend buffer.
 * @return const float* A read-only pointer to the circular buffer array.
 */
const float* GetTmprTrendBuffer(void);

/**
* @brief Rounds a float value to a specific number of decimal places.
* @param value The float value to round.
* @param places The number of decimal places (e.g., 2).
* @return float The rounded value.
*/
float RoundToDecimals(float value, UB places);

#endif // DATA_UTILS_H