/* LEDC (LED Controller) fade example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "UDPsocket.h"

/*
 *
 */
#ifdef CONFIG_IDF_TARGET_ESP32
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
#endif

ledc_channel_config_t ledc_channel[LEDC_CH_NUM] = {
#ifdef CONFIG_IDF_TARGET_ESP32
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
#endif

uint8_t speeds_and_directions[9] = {0};

uint8_t speeds_and_directions_1[4][2] = { {255, 1}, {255, 1}, {255, 1}, {255, 1} }; // Move Forward
uint8_t speeds_and_directions_2[4][2] = { {255, 0}, {255, 0}, {255, 0}, {255, 0} }; // Move Backward

uint8_t speeds_and_directions_3[4][2] = { {255, 1}, {255, 0}, {255, 1}, {255, 0} }; //Rotate Right
uint8_t speeds_and_directions_4[4][2] = { {255, 0}, {255, 1}, {255, 0}, {255, 1} }; // Rotate Left

uint8_t speeds_and_directions_5[4][2] = { {255, 1}, {255, 0}, {255, 0}, {255, 1} }; //Move Right
uint8_t speeds_and_directions_6[4][2] = { {255, 0}, {254, 1}, {255, 1}, {255, 0} }; // Move Left

void msDelay(uint16_t my_milliseconds) // Delay by n milliseconds
{
    vTaskDelay(my_milliseconds / portTICK_PERIOD_MS);
}

void RunSingleMotor(uint8_t motor_id, uint8_t motor_speed, uint8_t motor_direction)
{
    uint8_t id_to_channel = motor_id * 2;

    if(motor_direction == 0){
        ledc_set_duty(ledc_channel[id_to_channel].speed_mode, ledc_channel[id_to_channel].channel, 0);
        ledc_set_duty(ledc_channel[id_to_channel + 1].speed_mode, ledc_channel[id_to_channel + 1].channel, motor_speed);

        ledc_update_duty(ledc_channel[id_to_channel].speed_mode, ledc_channel[id_to_channel].channel);
        ledc_update_duty(ledc_channel[id_to_channel + 1].speed_mode, ledc_channel[id_to_channel + 1].channel);
    }
    else if(motor_direction == 1){
        ledc_set_duty(ledc_channel[id_to_channel + 1].speed_mode, ledc_channel[id_to_channel + 1].channel, 0);
        ledc_set_duty(ledc_channel[id_to_channel].speed_mode, ledc_channel[id_to_channel].channel, motor_speed);

        ledc_update_duty(ledc_channel[id_to_channel + 1].speed_mode, ledc_channel[id_to_channel + 1].channel);
        ledc_update_duty(ledc_channel[id_to_channel].speed_mode, ledc_channel[id_to_channel].channel);
    }
    else{
        printf("Error in -- RunSingleMotor --: Wrong motor direction value\n");
    }
}

void RunMotors(uint8_t motor_params[9])
{
    RunSingleMotor(0, motor_params[1], motor_params[2]);
    RunSingleMotor(1, motor_params[3], motor_params[4]);
    RunSingleMotor(2, motor_params[5], motor_params[6]);
    RunSingleMotor(3, motor_params[7], motor_params[8]);
}

void PrintBuffer(uint8_t *rx_buffer)
{
    printf("Buffer contents: \n");
    for(uint8_t i = 0; i < 9; i++){
        printf("%d ", rx_buffer[i]);
    }
    printf("\n");
}

void app_main(void)
{
    int ch;

    /*
     * Prepare and set configuration of timers
     * that will be used by LED Controller
     */
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_8_BIT, // resolution of PWM duty
        .freq_hz = 5000,                      // frequency of PWM signal
        .speed_mode = LEDC_HS_MODE,           // timer mode
        .timer_num = LEDC_HS_TIMER,            // timer index
        .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
    };
    
    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&ledc_timer);

    // Set LED Controller with previously prepared configuration
    for (ch = 0; ch < LEDC_CH_NUM; ch++) {
        ledc_channel_config(&ledc_channel[ch]);
    }

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    xTaskCreate(udp_server_task, "udp_server", 4096, (void*)AF_INET, 5, NULL);

    while (1) {

        if(rx_buffer[0] == 1){
            //printf("Writing to buffer...\n");

            memcpy(&speeds_and_directions[0], &rx_buffer[0], sizeof rx_buffer); //* sizeof( rx_buffer[1] ));
            //speeds_and_directions = rx_buffer;
        }
        else if(rx_buffer[0] == 0){
            memset(&speeds_and_directions[0], 0, sizeof speeds_and_directions);
        }
        else{
            printf("Unknown rx Parse Byte");
        }
        
        msDelay(5);
        //PrintBuffer(&speeds_and_directions[0]);

        RunMotors(speeds_and_directions);
        
    }

}