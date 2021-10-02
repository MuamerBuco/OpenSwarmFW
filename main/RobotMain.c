#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "UDPsocket.h"
#include "LEDcontrol.h"
#include "BucketControl.h"
#include "BatteryMonitoring.h"
#include "MotorControl.h"

#define STOP_AFTER_MS 3000

typedef enum command_modes {
    STANDARD_MODE = 1,
    CUSTOM_MOVE,
    CUSTOM_LED,
    CUSTOM_BUCKET

} command_modes;

uint8_t speeds_and_directions[8] = {0};
uint8_t LEDprogram;
uint8_t bucketSet[2] = {0};

void msDelay(uint16_t my_milliseconds) // Delay by n milliseconds
{
    vTaskDelay(my_milliseconds / portTICK_PERIOD_MS);
}

void PrintBuffer(uint8_t *rx_buffer)
{
    printf("Buffer contents: \n");
    for(uint8_t i = 0; i < 12; i++){
        printf("%d ", rx_buffer[i]);
    }
    printf("\n");
}

void stopRobot()
{
    LEDprogram = ROBOT_SHUTDOWN;
    bucketSet[0] = 0;
    bucketSet[1] = 0;

    memset(speeds_and_directions, 0, sizeof speeds_and_directions);

    RunMotors(speeds_and_directions);
    runBucketServos(bucketSet);
    LEDRing_programParsing(LEDprogram);
}

uint8_t parse_incoming()
{
    uint8_t recv_msg[12];

    // check if theres anything on the queue, if not wait 10 ticks
    if( xQueueReceive( xQueue, &( recv_msg ), ( TickType_t ) 10 ) )
    {
        switch( recv_msg[0] )
        {
            case STANDARD_MODE :
                // apply standard program
                memcpy(&speeds_and_directions[0], &recv_msg[1], 8);
                LEDprogram = recv_msg[9];
                bucketSet[0] = recv_msg[10];
                bucketSet[1] = recv_msg[11];

                RunMotors(speeds_and_directions);
                runBucketServos(bucketSet);
                LEDRing_programParsing(LEDprogram);
                return 1;

            case CUSTOM_MOVE :
                memcpy(&speeds_and_directions[0], &recv_msg[1], 8);
                RunMotors(speeds_and_directions);
                return 1;
            
            case CUSTOM_LED : ;
                // control the LEDs manually
                uint8_t LEDstate[6];
                memcpy(&LEDstate[0], &recv_msg[1], sizeof LEDstate);
                parseCustomLED(LEDstate);
                return 1;

            case CUSTOM_BUCKET : ;
                uint8_t bucketState[2];
                bucketState[0] = recv_msg[1];
                bucketState[1] = recv_msg[2];
                // set custom bucket state with speed
                runBucketServos(bucketState);
                return 1;
        }

        return 0;
    }
    else {
        return 0;
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // This helper function configures Wi-Fi or Ethernet, as selected in menuconfig
    ESP_ERROR_CHECK(example_connect());

    xTaskCreate(udp_server_task, "udp_server", 4096, (void*)AF_INET, 5, NULL);

    // TODO set esp_err_t returns and check with ESP ERROR
    ESP_ERROR_CHECK(BucketControl_initialize());
    ESP_ERROR_CHECK(MotorControl_initialize());
    ESP_ERROR_CHECK(LEDRing_initialize(0, 17, 24));

    uint32_t alarmCounter = 0;
    uint8_t run_delay_ms = 3;

    while (1) 
    {
        if( parse_incoming() )
        {
            alarmCounter = 0;// reset watchdog timer for shutdown behavior
        }
        else {
            alarmCounter += run_delay_ms;// update -time passed- counter
        }

        // if no msg is recived for STOP_AFTER_MS ms, stop everything
        if(alarmCounter >= STOP_AFTER_MS)
        {
            stopRobot();
        }

        msDelay(run_delay_ms);
    }
}