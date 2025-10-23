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

#define PRIVATE static

#define DEFAULT_TEMP 22.0
#define ALPHA 0.07

/**
 * @brief Performs Exponential Smoothing on raw sensor data.
 * * This function filters noise from raw measurements using a global 
 * static variable 'stabilizedValue' (which is private to DataUtils.cpp) 
 * and the smoothing constant ALPHA.
 * * @param rawValue The current raw measured value (e.g., temperature).
 * @return float The new, smoothed (stabilized) value.
 */
float ExponentialSmooth(float rawValue);

#endif // DATA_UTILS_H