#include <stdio.h>
#include "esp_err.h"


#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

esp_err_t BatteryMonitoring_initialize();
uint32_t readBatteryVoltage();