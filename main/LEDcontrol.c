#include "LEDcontrol.h"
#include <esp_log.h>
#include <driver/gpio.h>
#include <stdint.h>
#include <driver/rmt.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

static char tag[] = "WS2812 LED Control";

// Private global variables
uint8_t PixelCount = 24; //global pixel count
rmt_channel_t Channel = 0; //remote channel

rmt_item32_t * Items;
pixel_t * Pixels;

typedef enum CustomLEDprograms {
	SET_SINGLE_PIXEL = 1,
	SET_ALL_PIXELS = 2,
	RAINBOW = 3,
	THEATER_CHASE = 4,
	FADE_IN_OUT = 5,
	BLINK_ONCE = 6,
	BLINK_N_TIMES = 7
} CustomLEDprograms;

/**
 * A NeoPixel is defined by 3 bytes ... red, green and blue.
 * Each byte is composed of 8 bits ... therefore a NeoPixel is 24 bits of data.
 * At the underlying level, 1 bit of NeoPixel data is one item (two levels)
 */


/**
 * Set two levels of RMT output to the Neopixel value for a "1".
 */

static void setItem1(rmt_item32_t *pItem) {
	pItem->level0 = 1;
	pItem->duration0 = 7;
	pItem->level1 = 0;
	pItem->duration1 = 6;
} // setItem1


/**
 * Set two levels of RMT output to the Neopixel value for a "0".
 */
static void setItem0(rmt_item32_t *pItem) 
{
	pItem->level0 = 1;
	pItem->duration0 = 4;
	pItem->level1 = 0;
	pItem->duration1 = 8;
} // setItem0


esp_err_t LEDRing_initialize(rmt_channel_t channel, gpio_num_t gpioNum, uint16_t pixelCount) 
{
	esp_err_t err_state;

	PixelCount = pixelCount;
	Channel = channel;
	Items = (rmt_item32_t *)calloc(sizeof(rmt_item32_t), pixelCount * 24);
	Pixels = (pixel_t *)calloc(sizeof(pixel_t),pixelCount);

	rmt_config_t config;
	config.rmt_mode = RMT_MODE_TX;
	config.channel = channel;
	config.gpio_num = gpioNum;
	config.mem_block_num = 1;
	config.tx_config.loop_en = 0;
	config.tx_config.carrier_en = 0;
	config.tx_config.idle_output_en = 1;
	config.tx_config.idle_level = (rmt_idle_level_t)0;
	config.tx_config.carrier_duty_percent = 50;
	config.tx_config.carrier_freq_hz = 10000;
	config.tx_config.carrier_level = (rmt_carrier_level_t)1;
	config.clk_div = 8;

	err_state = rmt_config(&config);
	err_state = rmt_driver_install(Channel, 0, 0);

	return err_state;
}


void LEDRing_setPixels(uint16_t index, uint8_t red, uint8_t green, uint8_t blue)
{
	if (index >= PixelCount)
	{
		ESP_LOGE(tag, "setPixel: index out of range: %d", index);
		return;
	}
	Pixels[index].red = red;
	Pixels[index].green = green;
	Pixels[index].blue = blue;
} // setPixel

void LEDRing_showPixels()
{
	uint32_t i,j;
	rmt_item32_t *pCurrentItem = Items;
	for (i=0; i<PixelCount; i++) 
	{
		uint32_t currentPixel = Pixels[i].red | (Pixels[i].green << 8) | (Pixels[i].blue << 16);
		
		for (j=0; j<24; j++) 
		{
			if (currentPixel & 1<<j) {
				setItem1(pCurrentItem);
			} else {
				setItem0(pCurrentItem);
			}
			pCurrentItem++;
		}
	}
	// Show the pixels.
	rmt_write_items(Channel, Items, PixelCount*24, 1);
}

void LEDRing_clear() 
{
	uint16_t i;
	
	for (i=0; i<PixelCount; i++) 
	{
		Pixels[i].red = 0;
		Pixels[i].green = 0;
		Pixels[i].blue = 0;
	}
} // clear

// Turns the full strip to given color with no effects
void LEDRing_fullShow(uint8_t red, uint8_t green, uint8_t blue)
{
	for(uint8_t pixNum =0; pixNum < PixelCount; pixNum++)
	{
		LEDRing_setPixels(pixNum, green, blue, red); //the WS2812 strip we have shows colors as GRB
	}
	LEDRing_showPixels();
}

void LEDRing_blink(uint8_t red, uint8_t green, uint8_t blue, uint8_t number_of_blinks);

// set direct custom commands for LEDs
uint8_t parseCustomLED(uint8_t buffer[6])
{
	uint8_t switch_case = buffer[0];
	uint8_t index = buffer[1];
	uint8_t red = buffer[2];
	uint8_t green = buffer[3];
	uint8_t blue = buffer[4];
	uint8_t ms_delay = buffer[5];

	switch( switch_case )
	{
		case SET_SINGLE_PIXEL :
			LEDRing_setPixels(index, red, green, blue);
			return 1;

		case SET_ALL_PIXELS :
			LEDRing_fullShow(red, green, blue);
			return 1;
		
		case RAINBOW :
			rainbowCycle(ms_delay);
			return 1;

		case THEATER_CHASE :
			theaterChaseRainbow(ms_delay);
			return 1;

		case FADE_IN_OUT :
			FadeInOut(red, green, blue);
			return 1;

		case BLINK_ONCE :
			LEDRing_blink(red, green, blue, 1);
			return 1;

		case BLINK_N_TIMES :
			LEDRing_blink(red, green, blue, index);
			return 1;
	}

	return 0;
}

// effects

uint8_t * Wheel(uint8_t WheelPos) 
{
  static uint8_t c[3];
  
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }
  
  return c;
}

void rainbowCycle(int SpeedDelay) 
{
  uint8_t *c;
  uint16_t i, j;
  
  for(j=0; j<256*5; j++) 
  {
    for(i=0; i< PixelCount; i++) 
	{
      c=Wheel(((i * 256 / PixelCount) + j) & 255);
      LEDRing_setPixels(i, *c, *(c+1), *(c+2));
    }

    LEDRing_showPixels();
    vTaskDelay(SpeedDelay / portTICK_PERIOD_MS);
  }
}

void theaterChaseRainbow(int SpeedDelay) 
{
  uint8_t *c;
  
  for (int j=0; j < 256; j++) 
  {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) 
	{
        for (int i=0; i < PixelCount; i=i+3) 
		{
          c = Wheel( (i+j) % 255);
          LEDRing_setPixels(i+q, *c, *(c+1), *(c+2));    //turn every third pixel on
        }
        
		LEDRing_showPixels();
		vTaskDelay(SpeedDelay / portTICK_PERIOD_MS);
        
		for (int i=0; i < PixelCount; i=i+3) 
		{
      			LEDRing_setPixels(i+q, 0,0,0);        //turn every third pixel off
        }
    }
  }
}

void LEDRing_blink(uint8_t red, uint8_t green, uint8_t blue, uint8_t number_of_blinks)
{
	uint8_t r = red, g = green, b = blue;

	for(int lap = 0; lap < number_of_blinks; lap++)
	{
		LEDRing_fullShow(red, green, blue);
		
		for(int i = 0; i < 256; i++)
		{
			if(red > 0) red -= 1;
			if(green > 0) green -= 1;
			if(blue > 0) blue -= 1;

			LEDRing_fullShow(red, green, blue);
			vTaskDelay(20 / portTICK_PERIOD_MS); 
		}

		red = r; green = g; blue = b;
	} 
}

void FadeInOut(uint8_t red, uint8_t green, uint8_t blue)
{
  float r, g, b;
  
  for(int k = 0; k < 256; k=k+1) 
  {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    LEDRing_fullShow(r,g,b);
	vTaskDelay(20 / portTICK_PERIOD_MS);
  }

  vTaskDelay(1000 / portTICK_PERIOD_MS);

  for(int k = 255; k >= 0; k=k-2) 
  {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    LEDRing_fullShow(r,g,b);

	vTaskDelay(20 / portTICK_PERIOD_MS); 
  }
}

//////////// PROGRAM PARSING

uint8_t LEDRing_programParsing(LEDRing_programs program)
{
	switch(program) 
	{
		case ROBOT_READY :
			LEDRing_fullShow(0, 255, 0); // just lights up green
			return 1;

		case CONNECTED_SUCCESSFULY :
			LEDRing_blink(255, 0, 255, 3); // blink purple
			return 1;
		
		case ROBOT_SHUTDOWN :
			LEDRing_clear(); // turn all LEDs off
			return 1;

		case BATTERY_EMPTY :
			LEDRing_fullShow(255, 0, 0);
			return 1;

		case REACHED_ENDPOINT :
			LEDRing_fullShow(200, 0, 200);
			return 1;
	}

	return 0;
}

