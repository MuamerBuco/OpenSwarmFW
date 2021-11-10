#include "BucketControl.h"

#include <stdint.h>
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

static ledc_timer_config_t timer_conf;
static ledc_channel_config_t ledc_conf;

uint8_t current_tilt = 0;
uint8_t current_extension = 0;

// TODO fix angle calculations
uint32_t calculate_duty(uint8_t angle)
{
    float duty = SERVO_MIN_PULSEWIDTH + ( (SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH)/SERVO_MAX_DEGREE ) * angle;
        
    uint32_t duty_int = (uint32_t)duty;
    
    return duty_int;
}

esp_err_t BucketControl_initialize()
{
    esp_err_t err_state;

    //configure low speed PWM timer
    timer_conf.duty_resolution = LEDC_TIMER_15_BIT;
    timer_conf.freq_hz = 50;
    timer_conf.speed_mode = LEDC_LOW_SPEED_MODE;
    timer_conf.timer_num = LEDC_TIMER_1;
    err_state = ledc_timer_config(&timer_conf);
    
    //configure low speed PWM channel 0
    ledc_conf.channel = LEDC_CHANNEL_0;
    ledc_conf.duty = 0;
    ledc_conf.gpio_num = GPIO_SERVO_0;
    ledc_conf.intr_type = LEDC_INTR_DISABLE;
    ledc_conf.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_conf.timer_sel = LEDC_TIMER_1;
    err_state = ledc_channel_config(&ledc_conf);

    //configure low speed PWM channel 1
    ledc_conf.channel = LEDC_CHANNEL_1;
    ledc_conf.duty = 0;
    ledc_conf.gpio_num = GPIO_SERVO_1;
    ledc_conf.intr_type = LEDC_INTR_DISABLE;
    ledc_conf.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_conf.timer_sel = LEDC_TIMER_1;
    err_state = ledc_channel_config(&ledc_conf);

    return err_state;
}

// takes in bucket (extension and tilt) servo parameters in degrees and actuates the bucket
int runBucketServos(uint8_t bucket_parameters[2])
{
    uint32_t timer_duty;

    if( bucket_parameters[0] <= MAX_TILT && bucket_parameters[1] <= MAX_EXTENSION ) {
        current_tilt = bucket_parameters[0];
        current_extension = bucket_parameters[1];
    } else {
        printf("Bucket parameters out of scope\n");
        return 0;
    }

    timer_duty = calculate_duty(bucket_parameters[0]);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, timer_duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

    timer_duty = calculate_duty(bucket_parameters[1]);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, timer_duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);

    return 1;
}

///// TESTING

void servo_task(void* params)
{
    int32_t count, timer_duty;
    while(1)
    {
        for (count = 0; count <= SERVO_MAX_DEGREE; count++) 
        {
            printf("Angle of rotation: %d\n", count);
            timer_duty = calculate_duty(count);
            printf("duty: %d \n", timer_duty);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, timer_duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            vTaskDelay(10/portTICK_RATE_MS);     //Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation at 5V
        }

        vTaskDelay(2000/portTICK_RATE_MS);

        for (count = SERVO_MAX_DEGREE; count >= 0; count--) 
        {
            printf("Angle of rotation: %d\n", count);
            timer_duty = calculate_duty(count);
            printf("duty: %d\n", timer_duty);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, timer_duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            vTaskDelay(10/portTICK_RATE_MS);     //Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation at 5V
        }

        vTaskDelay(2000/portTICK_RATE_MS);
    }
}

void test_function()
{
    //configure high speed PWM timer
    timer_conf.duty_resolution = LEDC_TIMER_15_BIT;
    timer_conf.freq_hz = 50;
    timer_conf.speed_mode = LEDC_LOW_SPEED_MODE;
    timer_conf.timer_num = LEDC_TIMER_1;
    ledc_timer_config(&timer_conf);
    
    //configure high speed PWM channel 0
    ledc_conf.channel = LEDC_CHANNEL_0;
    ledc_conf.duty = 0;
    ledc_conf.gpio_num = GPIO_SERVO_1;
    ledc_conf.intr_type = LEDC_INTR_DISABLE;
    ledc_conf.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_conf.timer_sel = LEDC_TIMER_1;
    ledc_channel_config(&ledc_conf);
    
    xTaskCreate(&servo_task, "servo-task", 5000, NULL, 4, NULL);

}