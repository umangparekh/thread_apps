/*
 * File: adc.c
 * Description: EM3XX-specific ADC HAL functions
 *
 *
 * Copyright 2008 by Ember Corporation. All rights reserved.                *80*
 */
#include PLATFORM_HEADER
#include "stack/include/error.h"
#include "hal/micro/micro-common.h"
#include "hal/micro/cortexm3/micro-common.h"
#include "micro/adc.h"

void halAdcSetClock(bool slow)
{
}

bool halAdcGetClock(void)
{
  return 0;
}

void halAdcIsr(void)
{
}

void halInternalInitAdc(void)
{
}

EmberStatus halStartAdcConversion(ADCUser id,
                                  ADCReferenceType reference,
                                  ADCChannelType channel,
                                  ADCRateType rate)
{
  return 0;
}

EmberStatus halRequestAdcData(ADCUser id, uint16_t *value)
{
  return 0;
}

EmberStatus halReadAdcBlocking(ADCUser id, uint16_t *value)
{
  return 0;
}

EmberStatus halAdcCalibrate(ADCUser id)
{
  return 0;
}

int16_t halConvertValueToVolts(uint16_t value)
{
  return 0;
}
