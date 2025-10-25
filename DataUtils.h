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

/* Helper macro to define local-scope (private) variables/functions */
#define PRIVATE static

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
#define ALPHA 0.05

// --------------------------------------------------------------------------
// PUBLIC FUNCTION PROTOTYPE
// --------------------------------------------------------------------------
void Init_ExponentialSmooth(float t_init);
float Run_ExponentialSmooth(float rawValue);

#endif // DATA_UTILS_H