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
#include "Global.h"
#include "DataUtils.h"

// --------------------------------------------------------------------------
// PRIVATE DATA (Static Variables)
// --------------------------------------------------------------------------

/**
 * @brief Internal state variable for the Exponential Smoothing filter.
 * * This variable is maintained between calls to Run_ExponentialSmooth().
 */
PRIVATE float DU_Filtered = DEFAULT_TEMP;

/**
 * @brief Circular buffer for storing temperature history for trend calculation.
 */
PRIVATE float DU_TmprTrendBuffer[TREND_COUNT];

/**
 * @brief Index for writing into the circular buffer (points to the oldest value).
 */
PRIVATE UB DU_TmprTrendBufferIdx = 0;

// --------------------------------------------------------------------------
// PUBLIC FUNCTIONS IMPLEMENTATION
// --------------------------------------------------------------------------

/**
 * @brief Initializes the internal filtered value for the temperature filter.
 * This should be called once, during setup(), after the sensor returns valid data.
 * @param t_init The initial (starting) temperature to set as the first stable value for the filter calculation.
 */
void Init_ExponentialSmooth(float t_init) {
  DU_Filtered = t_init;
}

/**
 * @brief Initializes the circular trend buffer with an initial temperature value.
 * @param t_init The initial temperature to fill the buffer with.
 */
void Init_TmprTrendBuffer(float t_init) {
  // Initialize the entire buffer with the starting temperature
  for (UB i = 0; i < TREND_COUNT; i++) {
    DU_TmprTrendBuffer[i] = t_init;
  }
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

/**
 * @brief Adds a new temperature value to the circular buffer and advances the index.
 * @param newTmpr The new temperature value to store.
 */
void AddTmprToTrendBuffer(float newTmpr) {
    // Store the new value at the current index (which is the oldest value)
    DU_TmprTrendBuffer[DU_TmprTrendBufferIdx] = newTmpr;
    // Advance index and wrap around (circular buffer logic)
    DU_TmprTrendBufferIdx = (DU_TmprTrendBufferIdx + 1) % TREND_COUNT;
}

/**
 * @brief Calculates the temperature trend by comparing the newest and oldest values in the buffer.
 * @return SB Returns 1 (rising), -1 (falling), or 0 (stable).
 */
SB GetTemperatureTrend() {
  UB newestIdx;
  // Index of the oldest value (where the next write will occur)
  UB oldestIdx = DU_TmprTrendBufferIdx; 

  // The newest index is the one before the oldest index in the circular buffer
  if(0 == DU_TmprTrendBufferIdx) {
    newestIdx = TREND_COUNT - 1;
  } else {
    newestIdx = DU_TmprTrendBufferIdx - 1;
  }
  
  float oldestTemp = DU_TmprTrendBuffer[oldestIdx];
  float newestTemp = DU_TmprTrendBuffer[newestIdx];

  if (newestTemp > oldestTemp + TREND_THRESHOLD) {
    return 1; // Rising trend
  } else if (newestTemp < oldestTemp - TREND_THRESHOLD) {
    return -1; // Falling trend
  } else {
    return 0; // Stable
  }
}

/**
 * @brief Gets a constant pointer to the internal temperature trend buffer.
 * @return const float* A read-only pointer to the circular buffer array.
 */
const float* GetTmprTrendBuffer() {
  return DU_TmprTrendBuffer;
}