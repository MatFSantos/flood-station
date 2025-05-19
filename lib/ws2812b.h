#ifndef WS2812B_H
#define WS2812B_H

#include <stdint.h>
#include "hardware/pio.h"
#include "ws2812b_designs.h"

#define PIN_WS2812B 7
#define NUM_LEDS_WS2812B 25

/**
 * @file ws2812b.
 * @brief Library file that contains the configuration and definitions of the
 * LED Matrix module WS2812B.ADC_BASE
 * @author Mateus F Santos
 * @date 01.02.2025
 */

/**
 * @brief Struct to the pio and state machine configuration.
 * 
 * @param pio pointer to PIO controler
 * @param sm pointer to the state machine
 */
typedef struct {
    PIO pio;
    uint sm;
} _ws2812b;

/**
 * @brief Inialyze the WS2812B
 * 
 * @param pio pointer to PIO controler
 * @param pin GPIO pin that WS2812B is
 * 
 * @return _ws2812b* Pointer to created struct
 */
_ws2812b * init_ws2812b(PIO pio, uint8_t pin);

/**
 * @brief Plot the form acordding the passed matrix
 * 
 * @param ws Pointer to the _ws2812b with configurations
 * @param frame frame that will be drawed in the WS2812B matrix
 */
void ws2812b_plot(const _ws2812b *ws, const Frame *frame);

/**
 * @brief Turn off all LEDs on the WS2812B
 * 
 * @param ws Pointer to the _ws2812b with configurations
 */
void ws2812b_turn_off(const _ws2812b *ws);


/**
 * @brief Convert RGB color to WS2812B according intensity value
 * 
 * @param r red component of RGB color (value between 0 and 1)
 * @param g green component of RGB color (value between 0 and 1)
 * @param b blue component of RGB color (value between 0 and 1)
 */
uint32_t ws2812b_rgb_color(double r, double g, double b);

/**
 * @brief Validate a number according to the specified limits
 * 
 * @param num number to validate
 * @param lim_max limit max to validate the number
 * @param lim_min limit min to validate the number
 */
double ws2812b_validate_number(const double num, const double lim_max, const double lim_min);

#endif