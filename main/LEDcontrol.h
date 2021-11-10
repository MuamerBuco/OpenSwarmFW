#ifndef MAIN_WS2812_H_
#define MAIN_WS2812_H_

#include <stdint.h>
#include <driver/rmt.h>

static uint32_t LEDRing_ready = 1;

typedef struct 
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;

} pixel_t;

typedef enum LEDRing_programs
{
	NONE,
	ROBOT_SHUTDOWN,
	BATTERY_EMPTY,
	ROBOT_READY,
	REACHED_ENDPOINT,
	CONNECTED_SUCCESSFULY,
	Number_of_LEDRing_programs
	
} LEDRing_programs;

typedef enum CustomLEDprograms {
	SET_SINGLE_PIXEL = 1,
	SET_ALL_PIXELS = 2,
	RAINBOW = 3,
	THEATER_CHASE = 4,
	FADE_IN_OUT = 5,
	BLINK_ONCE = 6,
	BLINK_N_TIMES = 7
} CustomLEDprograms;

// Initialize LED control
esp_err_t LEDRing_initialize(rmt_channel_t channel, gpio_num_t gpioNum, uint16_t pixelCount);

// Control
uint8_t LEDRing_programParsing(LEDRing_programs program);
uint8_t parseCustomLED(uint8_t buffer[6]);
void LEDRing_setPixels(uint16_t index, uint8_t red, uint8_t green, uint8_t blue);
void LEDRing_fullShow(uint8_t red, uint8_t green, uint8_t blue);
void LEDRing_fullShow_time(uint8_t red, uint8_t green, uint8_t blue, int number_of_runs);
void LEDRing_blink(uint8_t red, uint8_t green, uint8_t blue, uint8_t number_of_blinks);
void LEDRing_showPixels();
void LEDRing_clear();

// Effects
void rainbowCycle(int);
void theaterChaseRainbow(int);
void FadeInOut(uint8_t red, uint8_t green, uint8_t blue);


#endif /* MAIN_WS2812_H_ */