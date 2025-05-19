#include "ws2812b.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include <stdlib.h>

_ws2812b * init_ws2812b(PIO pio, uint8_t pin){
    _ws2812b * ws = malloc(sizeof(_ws2812b));

    uint offset = pio_add_program(pio, &ws2812_program);
    uint sm = pio_claim_unused_sm(pio, true);

    pio_sm_config c = ws2812_program_get_default_config(offset);

    // set pin to be part of set output group, i.e. set by set instruction
    sm_config_set_set_pins(&c, pin, 1);

    // attach pio to the GPIO
    pio_gpio_init(pio, pin);

    // set pin direction to output at the PIO
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

    // set pio clock to 8MHz, giving 10 cycles per LED binary digit
    float div = clock_get_hz(clk_sys)/8000000.0;
    sm_config_set_clkdiv(&c, div);

    // give all the FIFO space to TX (not using RX)
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    // shift to the left, use autopull, next pull theshould 24 bits
    sm_config_set_out_shift(&c, false, true, 24);

    // set sticky-- continue to drive value from last set/out. other stuff off.
    sm_config_set_out_special(&c, true, false, false);

    // load config, and jump to the start of the program
    pio_sm_init(pio, sm, offset, &c);

    // enable this pio state machine
    pio_sm_set_enabled(pio, sm, true);
    
    ws->pio = pio;
    ws->sm = sm;
    return ws;
}

double ws2812b_validate_number(const double num, const double lim_max, const double lim_min){
    if (num > lim_max)
        return lim_max;
    if (num < lim_min)
        return lim_min;
    return num;
}

uint32_t ws2812b_rgb_color(double r, double g, double b){
    r = ws2812b_validate_number(r, 1.0, 0.0);
    g = ws2812b_validate_number(g, 1.0, 0.0);
    b = ws2812b_validate_number(b, 1.0, 0.0);
    unsigned char R = r * 255;
    unsigned char G = g * 255;
    unsigned char B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}


void ws2812b_plot(const _ws2812b *ws, const Frame *frame){
    uint32_t color;
    for (uint8_t i = 0; i < NUM_LEDS_WS2812B; i++ ){
        if (frame->leds[i])
            color = ws2812b_rgb_color(frame->color[0], frame->color[1], frame->color[2]);
        else
            color = ws2812b_rgb_color(0, 0, 0);
        pio_sm_put_blocking(ws->pio, ws->sm, color);
    }
}

void ws2812b_turn_off(const _ws2812b *ws){
    Frame f = {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0}
    };
    ws2812b_plot(ws, &f);
}