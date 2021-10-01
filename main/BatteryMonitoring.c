#include "BatteryMonitoring.h"

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

static esp_adc_cal_characteristics_t *adc_chars;
#if CONFIG_IDF_TARGET_ESP32

/*
ADC1 channel 0 is GPIO36
ADC1 channel 1 is GPIO37
ADC1 channel 2 is GPIO38
ADC1 channel 3 is GPIO39
ADC1 channel 4 is GPIO32
ADC1 channel 5 is GPIO33
ADC1 channel 6 is GPIO34
ADC1 channel 7 is GPIO35
*/

static const adc_channel_t channel = ADC_CHANNEL_6;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;

#endif
static const adc_atten_t atten = ADC_ATTEN_DB_11; // Set attenuation to 11dB for max voltage = 2.9V(check if correct)
static const adc_unit_t unit = ADC_UNIT_1; // Cant use ADC2 because WiFi is active


static void check_efuse(void)
{
#if CONFIG_IDF_TARGET_ESP32
    //Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }
    
    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
#endif
}

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}

esp_err_t BatteryMonitoring_initialize()
{
    //Check if Two Point or Vref are burned into eFuse
    check_efuse();

    //Configure ADC
    if (unit == ADC_UNIT_1) {
        adc1_config_width(width);
        adc1_config_channel_atten(channel, atten);
    } 
    else {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }

    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);

    return ESP_OK;
}

// return single reading in mV, averaged after multisampling
uint32_t readBatteryVoltage()
{
    uint32_t adc_reading = 0;
    
    //Multisampling
    for (uint8_t i = 0; i < NO_OF_SAMPLES; i++) 
    {
        if (unit == ADC_UNIT_1) {
            adc_reading += adc1_get_raw((adc1_channel_t)channel);
        } 
        else {
            int raw;
            adc2_get_raw((adc2_channel_t)channel, width, &raw);
            adc_reading += raw;
        }
    }
    
    adc_reading /= NO_OF_SAMPLES;
    
    //Convert adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    return voltage;
}

// used for testing and calibration
void test_battery_monitoring(void)
{
    //Check if Two Point or Vref are burned into eFuse
    check_efuse();

    //Configure ADC
    if (unit == ADC_UNIT_1) {
        adc1_config_width(width);
        adc1_config_channel_atten(channel, atten);
    } 
    else {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }

    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);

    //Continuously sample ADC1
    while (1) 
    {
        uint32_t adc_reading = 0;

        //Multisampling
        for (uint8_t i = 0; i < NO_OF_SAMPLES; i++) 
        {
            if (unit == ADC_UNIT_1) {
                adc_reading += adc1_get_raw((adc1_channel_t)channel);
            } 
            else {
                int raw;
                adc2_get_raw((adc2_channel_t)channel, width, &raw);
                adc_reading += raw;
            }
        }

        adc_reading /= NO_OF_SAMPLES;
        
        //Convert adc_reading to voltage in mV
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}