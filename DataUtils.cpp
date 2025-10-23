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

#include "DataUtils.h"

PRIVATE float stabilizedValue = DEFAULT_TEMP;

float ExponentialSmooth(float rawValue) {

  /* Formula: Stabilized = (ALPHA * New Raw) + ((1.0 - ALPHA) * Previous Stabilized) */
  stabilizedValue = (ALPHA * rawValue) + ((1.0 - ALPHA) * stabilizedValue);
  
  return stabilizedValue;
}