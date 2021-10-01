#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"

#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE

#define LEDC_HS_CH0_GPIO       (16)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_HS_CH1_GPIO       (4)
#define LEDC_HS_CH1_CHANNEL    LEDC_CHANNEL_1

#define LEDC_HS_CH2_GPIO       (15)
#define LEDC_HS_CH2_CHANNEL    LEDC_CHANNEL_2
#define LEDC_HS_CH3_GPIO       (2)
#define LEDC_HS_CH3_CHANNEL    LEDC_CHANNEL_3

#define LEDC_HS_CH4_GPIO       (13)
#define LEDC_HS_CH4_CHANNEL    LEDC_CHANNEL_4
#define LEDC_HS_CH5_GPIO       (12)
#define LEDC_HS_CH5_CHANNEL    LEDC_CHANNEL_5

#define LEDC_HS_CH6_GPIO       (32)
#define LEDC_HS_CH6_CHANNEL    LEDC_CHANNEL_6
#define LEDC_HS_CH7_GPIO       (18)
#define LEDC_HS_CH7_CHANNEL    LEDC_CHANNEL_7

#define LEDC_CH_NUM 8

esp_err_t MotorControl_initialize();
void RunMotors(uint8_t motor_params[8]);

// TODO add defines that make sense -- per motor and direction and then put in config pin bindings