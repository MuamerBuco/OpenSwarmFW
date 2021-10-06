#include "MotorControl.h"

static ledc_channel_config_t ledc_channel_motors[LEDC_CH_NUM] = 
{
    {
        .channel    = LEDC_HS_CH0_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_HS_CH0_GPIO,
        .speed_mode = LEDC_HS_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_HS_TIMER
    },

    {
        .channel    = LEDC_HS_CH1_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_HS_CH1_GPIO,
        .speed_mode = LEDC_HS_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_HS_TIMER
    },

    {
        .channel    = LEDC_HS_CH2_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_HS_CH2_GPIO,
        .speed_mode = LEDC_HS_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_HS_TIMER
    },

    {
        .channel    = LEDC_HS_CH3_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_HS_CH3_GPIO,
        .speed_mode = LEDC_HS_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_HS_TIMER
    },

    {
        .channel    = LEDC_HS_CH4_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_HS_CH4_GPIO,
        .speed_mode = LEDC_HS_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_HS_TIMER
    },

    {
        .channel    = LEDC_HS_CH5_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_HS_CH5_GPIO,
        .speed_mode = LEDC_HS_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_HS_TIMER
    },

    {
        .channel    = LEDC_HS_CH6_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_HS_CH6_GPIO,
        .speed_mode = LEDC_HS_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_HS_TIMER
    },

    {
        .channel    = LEDC_HS_CH7_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_HS_CH7_GPIO,
        .speed_mode = LEDC_HS_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_HS_TIMER
    },
};

esp_err_t MotorControl_initialize()
{
    esp_err_t err_state;

    // Prepare and set configuration of timers
    ledc_timer_config_t ledc_timer_motors = {
        .duty_resolution = LEDC_TIMER_8_BIT, // resolution of PWM duty
        .freq_hz = 5000,                      // frequency of PWM signal
        .speed_mode = LEDC_HS_MODE,           // timer mode
        .timer_num = LEDC_HS_TIMER,            // timer index
        .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
    };
    
    // Set configuration of timer0 for high speed channels
    err_state = ledc_timer_config(&ledc_timer_motors);

    // Set LED Controller with previously prepared configuration
    for (int ch = 0; ch < LEDC_CH_NUM; ch++) {
        err_state = ledc_channel_config(&ledc_channel_motors[ch]);
    }

    return err_state;
}

void RunSingleMotor(uint8_t motor_id, uint8_t motor_speed, uint8_t motor_direction)
{
    uint8_t id_to_channel = motor_id * 2;

    if(motor_direction == 0){
        ledc_set_duty(ledc_channel_motors[id_to_channel].speed_mode, ledc_channel_motors[id_to_channel].channel, 0);
        ledc_set_duty(ledc_channel_motors[id_to_channel + 1].speed_mode, ledc_channel_motors[id_to_channel + 1].channel, motor_speed);

        ledc_update_duty(ledc_channel_motors[id_to_channel].speed_mode, ledc_channel_motors[id_to_channel].channel);
        ledc_update_duty(ledc_channel_motors[id_to_channel + 1].speed_mode, ledc_channel_motors[id_to_channel + 1].channel);
    }
    else if(motor_direction == 1){
        ledc_set_duty(ledc_channel_motors[id_to_channel + 1].speed_mode, ledc_channel_motors[id_to_channel + 1].channel, 0);
        ledc_set_duty(ledc_channel_motors[id_to_channel].speed_mode, ledc_channel_motors[id_to_channel].channel, motor_speed);

        ledc_update_duty(ledc_channel_motors[id_to_channel + 1].speed_mode, ledc_channel_motors[id_to_channel + 1].channel);
        ledc_update_duty(ledc_channel_motors[id_to_channel].speed_mode, ledc_channel_motors[id_to_channel].channel);
    }
    else{
        printf("Error in -- RunSingleMotor --: Wrong motor direction value\n");
    }
}

void PrintBuffer2(uint8_t *rx_buffer)
{
    printf("Buffer contents: \n");
    for(uint8_t i = 0; i < 8; i++){
        printf("%d ", rx_buffer[i]);
    }
    printf("\n");
}

void RunMotors(uint8_t motor_params[8])
{
    PrintBuffer2(motor_params);
    RunSingleMotor(0, motor_params[0], motor_params[1]);
    RunSingleMotor(1, motor_params[2], motor_params[3]);
    RunSingleMotor(2, motor_params[4], motor_params[5]);
    RunSingleMotor(3, motor_params[6], motor_params[7]);
}
