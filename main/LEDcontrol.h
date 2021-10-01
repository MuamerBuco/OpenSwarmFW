#ifndef MAIN_WS2812_H_
#define MAIN_WS2812_H_

#include <stdint.h>
#include <driver/rmt.h>

typedef struct 
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;

} pixel_t;

typedef enum LEDRing_programs
{
	BATTERY_EMPTY,
	ROBOT_SHUTDOWN,
	ROBOT_READY,
	REACHED_ENDPOINT,
	CONNECTED_SUCCESSFULY
	
} LEDRing_programs;

// Initialize LED control
esp_err_t LEDRing_initialize(rmt_channel_t channel, gpio_num_t gpioNum, uint16_t pixelCount);

// Control
uint8_t LEDRing_programParsing(LEDRing_programs program);
uint8_t parseCustomLED(uint8_t buffer[6]);
void LEDRing_setPixels(uint16_t index, uint8_t red, uint8_t green, uint8_t blue);
void LEDRing_fullShow(uint8_t red, uint8_t green, uint8_t blue);
void LEDRing_showPixels();
void LEDRing_clear();

// Effects
void rainbowCycle(int);
void theaterChaseRainbow(int);
void FadeInOut(uint8_t red, uint8_t green, uint8_t blue);


#endif /* MAIN_WS2812_H_ */