#include <stdio.h>
#include "driver/ledc.h"


#define SERVO_MIN_PULSEWIDTH 1000.0 //Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH 4200.0 //Maximum pulse width in microsecond
#define SERVO_MAX_DEGREE 180.0 //Maximum angle in degree upto which servo can rotate
#define SERVO_PERIOD 20000.0
#define MAX_TIMER 32767

#define MAX_TILT 100
#define MAX_EXTENSION 100

#define GPIO_SERVO_0 24
#define GPIO_SERVO_1 25

esp_err_t BucketControl_initialize();
void runBucketServos(uint8_t bucket_parameters[2]);
void test_function();