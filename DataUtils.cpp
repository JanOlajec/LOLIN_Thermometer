/**
 * @file DataUtils.cpp
 * @brief Implementation file for data processing utility functions.
 * This file provides the definitions (logic) for the functions 
 * declared in DataUtils.h, including the Exponential Smoothing filter.
 * @author Jan Olajec
 * @date 23.10.2025
 * @copyright Copyright (c) 2025 Jan OLAJEC, All rights reserved.
 * @license This project is licensed under the MIT License
 */

// --------------------------------------------------------------------------
// INCLUDES
// --------------------------------------------------------------------------

#include "DataUtils.h"

// --------------------------------------------------------------------------
// PRIVATE DATA (Static Variables)
// --------------------------------------------------------------------------

/**
 * @brief Internal state variable for the Exponential Smoothing filter.
 * * This variable is maintained between calls to ExponentialSmooth().
 */
PRIVATE float DU_Filtered = DEFAULT_TEMP;

// --------------------------------------------------------------------------
// PUBLIC FUNCTIONS IMPLEMENTATION
// --------------------------------------------------------------------------

/**
 * @brief Initializes the internal filtered value for the temperature filter.
 * This should be called once, during setup(), after the sensor returns valid data.
 * @param t_init The initial (starting) temperature to set as the first stable value for the filter calculation.
 */
void Init_ExponentialSmooth(float t_init){
  DU_Filtered = t_init;
}

/**
 * @brief Performs Exponential Smoothing on raw sensor data.
 * This function filters noise from raw measurements using a global 
 * static variable 'DU_Filtered' (which is private to DataUtils.cpp) 
 * and the smoothing constant ALPHA.
 * @param rawValue The current raw measured temperature value.
 * @return float The new, smoothed (filtered) value.
 */
float Run_ExponentialSmooth(float rawValue) {
  /* Formula: filtered = (ALPHA * New Raw) + ((1.0 - ALPHA) * Previous filtered) */
  DU_Filtered = (ALPHA * rawValue) + ((1.0 - ALPHA) * DU_Filtered);
  return DU_Filtered;
}