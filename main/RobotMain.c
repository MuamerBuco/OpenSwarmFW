#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "UDPsocket.h"
#include "LEDcontrol.h"
#include "BucketControl.h"
#include "BatteryMonitoring.h"
#include "MotorControl.h"

#define STOP_AFTER_MS 35

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
    // runBucketServos(bucketSet);
    LEDRing_programParsing(LEDprogram);
}

uint8_t parse_incoming()
{
    uint8_t recv_msg[12];

    // check if theres anything on the queue, if not wait 10 ticks
    if( xQueueReceive( xQueue, &( recv_msg ), ( TickType_t ) 5 ) )
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
                
                if(LEDRing_ready && LEDprogram != 0){
                    // xTaskCreate(LEDRing_programParsing, "LED_Ring_Effect", 4096, (void*)LEDprogram, 5, NULL);
                    LEDRing_programParsing(LEDprogram);
                }
                //LEDRing_programParsing(LEDprogram);
                printf("Received STANDARD_MODE message");
                return 1;

            case CUSTOM_MOVE :
                // printf("Got to custom move\n");
                memcpy(&speeds_and_directions[0], &recv_msg[1], 8);
                RunMotors(speeds_and_directions);
                printf("Received CUSTOM_MOVE message");
                return 1;
            
            case CUSTOM_LED : ;
                // control the LEDs manually
                uint8_t LEDstate[6];
                memcpy(&LEDstate[0], &recv_msg[1], sizeof LEDstate);
                
                if(LEDRing_ready){
                    // xTaskCreate(parseCustomLED, "LED_Ring_Effect", 4096, (void*)LEDstate, 5, NULL);
                    parseCustomLED(LEDstate);
                }
                printf("Received CUSTOM_LED message");
                return 1;

            case CUSTOM_BUCKET : ;
                uint8_t bucketState[2];
                bucketState[0] = recv_msg[1];
                bucketState[1] = recv_msg[2];
                // set custom bucket state with speed
                runBucketServos(bucketState);
                printf("Received CUSTOM_BUCKET message");
                return 1;
        }

        return 0;
    }
    else {
        return 0;
    }
}

int increment_pwm(int target, int current, float increment_size_multiplier)
{
    if(target > 255 || target < 0) return 0;

    int diff = target - current;
    int increment = diff * increment_size_multiplier;

    if(increment > 1) return current + increment;
    else return current + 1;  

}

void parabolic_drive_ramp(int start, int end, int direction, uint16_t delay_ms)
{
    int current = start;
    uint8_t speeds_dir[8];

    while(current != end)
    {
        current = increment_pwm(end, current, 0.4);

        for(int i = 0; i < 9; i+=2)
        {
            speeds_dir[i] = current;
            speeds_dir[i+1] = direction;
        }
        RunMotors(speeds_dir);
        msDelay(delay_ms);
    }
}

void run_hardware_test_suit()
{
    // test motors, speeds up, slows down, runs each wheel one by one
    for(int i = 0; i < 2; i++)
    {
        parabolic_drive_ramp(190, 255, 1, 700);
        msDelay(1000);

        parabolic_drive_ramp(190, 255, 0, 700);
        msDelay(1000);

        for(int i = 0; i < 4; i++)
        {
            uint8_t speeds_dir[8] = {0, 0, 0, 0, 0, 0, 0, 0};
            speeds_dir[i] = 200;
            speeds_dir[i+1] = 1;
            RunMotors(speeds_dir);
            msDelay(1500);    
        }
    }
    
    uint8_t bucketState[2];
    // move bucket by the min/max angle
    for(int i = 0; i < 10; i++)
    {
        bucketState[0] = MAX_TILT;
        bucketState[1] = MAX_EXTENSION;
        runBucketServos(bucketState);

        msDelay(1000);

        bucketState[0] = MIN_TILT;
        bucketState[1] = MIN_EXTENSION;
        runBucketServos(bucketState);

        msDelay(1000);
    }
    
    // try all preset lighting patterns in enum
    for(int i = 0; i < 	Number_of_LEDRing_programs; i++)
    {
        LEDRing_programParsing(i);
        msDelay(1000);
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // // This helper function configures Wi-Fi or Ethernet, as selected in menuconfig
    ESP_ERROR_CHECK(example_connect());

    xTaskCreate(udp_server_task, "udp_server", 4096, (void*)AF_INET, 5, NULL);

    ESP_ERROR_CHECK(BucketControl_initialize());
    ESP_ERROR_CHECK(MotorControl_initialize());
    ESP_ERROR_CHECK(LEDRing_initialize(0, 32, 24));

    LEDRing_blink(200, 0, 0, 1); //blink green

    uint32_t alarmCounter = 0;
    uint8_t run_delay = 3;

    while (1) 
    {
        if( parse_incoming() )
        {
            //printf("Parsed incoming\n");
            alarmCounter = 0;// reset watchdog timer for shutdown behavior
        }
        else {
            alarmCounter += run_delay;// update -time passed- counter
        }

        //printf("The alarm counter: %d", alarmCounter);

        // if no msg is recived for STOP_AFTER_MS ms, stop everything
        if(alarmCounter >= STOP_AFTER_MS)
        {
            stopRobot();
            //printf("Stopped robot\n");
            alarmCounter = STOP_AFTER_MS;
        }

        // run_hardware_test_suit();
    }
}