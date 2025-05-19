#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"

#include "lib/ws2812b.h"
#include "lib/pwm.h"
#include "lib/led.h"
#include "lib/push_button.h"
#include "lib/ssd1306.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** pin definitions */
#define BUZZER 21
#define PWM_DIVISER 20
#define PWM_WRAP 2000   /* aprox 3.5kHz freq */
#define WATER_LEVEL_SENSOR 27   /* X axis */
#define RAIN_LEVEL_SENSOR 26    /* Y axis */

/** global vars */
_ws2812b *ws;                               /* 5x5 matrix */
ssd1306_t ssd;                              /* OLED display */

QueueHandle_t xQueueData; /* queue used to store sensor values */
TaskHandle_t xAlertMode = NULL; /* used to store alert mode task */
TaskHandle_t xNormalMode = NULL; /* used to store normal mode task */

typedef struct {
    double water_level;
    double rain_level;
} flood_station_t;

/**
 * @brief Function task to use on RTOS. This task get the sensors values
 * and put it in a Queue.
 * 
 * @param params params that the task can receive 
 */
void vReadSensorsTask(void *params);

/**
 * @brief Function task to use on RTOS. This task verify the sensors values
 * and change the operation mode according it.
 *
 * @param params params that the task can receive
 */
void vControlModeTask(void *params);

/**
 * @brief Function task to use on RTOS. This task execute the normal mode on
 * flood station.
 *
 * @param params params that the task can receive
 */
void vNormalModeTask(void *params);

/**
 * @brief Function task to use on RTOS. This task execute the alert mode on
 * flood station
 *
 * @param params params that the task can receive
 */
void vAlertModeTask(void *params);

/**
 * @brief Initialize the SSD1306 display
 *
 */
void init_display(void);

/**
 * @brief Initialize the all GPIOs that will be used in project
 *      - I2C;
 *      - Blue LED;
 */
void init_gpio(void);

/**
 * @brief Update the display informations
 *
 * @param message the message that will be ploted in display OLED
 * @param y position on vertical that the message will be ploted
 * @param clear if the display must be cleaned
 *
 */
void update_display(char *message, int16_t x, uint8_t y, bool clear);

/**
 * @brief Function to read the ADC conversor
 *
 * @param channel analog channel to read
 * @return value in percentage readded in selected channel
 *
 */
double read_sensor(uint8_t channel);

int main() {
    // init gpios and stdio functions
    stdio_init_all();
    init_gpio();

    // init adc channels
    adc_gpio_init(WATER_LEVEL_SENSOR);
    adc_gpio_init(RAIN_LEVEL_SENSOR);
    adc_init();

    PIO pio = pio0;
    ws = init_ws2812b(pio, PIN_WS2812B);

    // get ws and ssd struct
    init_display();
    update_display("FLOOD STATION", -1, 18, true);
    update_display("starting...", -1, 28, false);
    sleep_ms(2000);

    // create queues
    xQueueData = xQueueCreate(20, sizeof(flood_station_t));

    // create all tasks
    xTaskCreate(vControlModeTask, "Mode controller", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vReadSensorsTask, "Read sensors", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vNormalModeTask, "Normal mode", configMINIMAL_STACK_SIZE, NULL, 1, &xNormalMode);
    xTaskCreate(vAlertModeTask, "Alert mode", configMINIMAL_STACK_SIZE, NULL, 1, &xAlertMode);

    // init tasks
    vTaskStartScheduler();
    panic_unsupported();

    return 0;
}

// ============================================================================================================= //
// ======================================== FUNCTIONS CODE ===================================================== //
// ============================================================================================================= //

void vReadSensorsTask(void *params) {
    vTaskDelay(pdMS_TO_TICKS(500)); // aguarda para as tasks dos modos iniciarem e pararem.
    flood_station_t data;
    while (1) {
        data.rain_level = read_sensor(RAIN_LEVEL_SENSOR);
        data.water_level = read_sensor(WATER_LEVEL_SENSOR);
        printf("[Task: %s] water level: %.2f || rain level: %.2f\n",
            pcTaskGetName(NULL),
            data.water_level,
            data.rain_level
        );
        xQueueSend(xQueueData, &data, 0);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}


void vControlModeTask(void *params) {
    vTaskDelay(pdMS_TO_TICKS(500)); // aguarda para as tasks dos modos iniciarem e pararem.
    update_display("FLOOD STATION", -1, 2, true);
    
    flood_station_t data;

    // controle para saber se o modo em que a estação de inundação está já foi ativado
    // para que não seja ativado de forma consecutiva, sem necessidade.
    bool mode = false;
    while(1) {
        if (xQueueReceive(xQueueData, &data, portMAX_DELAY) == pdTRUE) {
            printf("[Task: %s] water level: %.2f || rain level: %.2f\n",
                   pcTaskGetName(NULL),
                   data.water_level,
                   data.rain_level);
            if (data.rain_level >= 80.0 || data.water_level >= 70){ // Modo alerta
                if(mode) {
                    // alterna a task entre NormalMode e AlertMode
                    vTaskSuspend(xNormalMode);
                    vTaskDelay(pdMS_TO_TICKS(1));
                    vTaskResume(xAlertMode);

                    // avisa no display OLED
                    update_display("DANGER!", -1, 32, false);
                    update_display("HIGH LEVEL!", -1, 42, false);
                    mode = false;
                }
            } else { // Modo normal
                if(!mode) {
                    // alterna a task entre NormalMode e AlertMode
                    vTaskSuspend(xAlertMode);
                    vTaskDelay(pdMS_TO_TICKS(1));
                    vTaskResume(xNormalMode);

                    // limpa as ultimas linhas do display OLED
                    update_display("              ", -1, 32, false);
                    update_display("              ", -1, 42, false);
                    mode = true;
                }
            }
            char buffer[20];
            snprintf(buffer,20,"WATER: %.2f%%", data.water_level);
            update_display(buffer, 2, 12, false);
            snprintf(buffer, 20, "RAIN: %.2f%%", data.rain_level);
            update_display(buffer, 2, 22, false);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void vNormalModeTask(void *params) {
    vTaskSuspend(NULL); // supende a si mesmo, dando o poder de controle para vControlModeTask
    while(1) {
        ws2812b_plot(ws, &GREEN_TL);
        gpio_put(PIN_GREEN_LED, 1);
        gpio_put(PIN_RED_LED, 0);
        pwm_set_gpio_level(BUZZER, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void vAlertModeTask(void *params) {
    vTaskSuspend(NULL); // supende a si mesmo, dando o poder de controle para vControlModeTask
    while(1) {
        ws2812b_plot(ws, &DANGER);
        gpio_put(PIN_GREEN_LED, 0);
        gpio_put(PIN_RED_LED, 1);
        pwm_set_gpio_level(BUZZER, PWM_WRAP / 4);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void init_display() {
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, I2C_ADDRESS, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
}

void init_gpio() {
    /** initialize i2c communication */
    int baudrate = 400 * 1000; // 400kHz baud rate for i2c communication
    i2c_init(I2C_PORT, baudrate);

    // set GPIO pin function to I2C
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SCL);

    /** initialize RGB LED */
    init_rgb_led();

    /** initialize buttons */
    init_push_button(PIN_BUTTON_A);
    init_push_button(PIN_BUTTON_B);

    /** initialize buzzer */
    pwm_init_(BUZZER);
    pwm_setup(BUZZER, PWM_DIVISER, PWM_WRAP);
    pwm_start(BUZZER, 0);
}

void update_display(char *message, int16_t x, uint8_t y, bool clear) {
    if (clear)
        ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 0, 0, 128, 64, true, false);
    uint8_t x_pos;
    if (x < 0)
        x_pos = 64 - (strlen(message) * 4);
    else
        x_pos = x;
    ssd1306_draw_string(&ssd, message, x_pos, y);
    ssd1306_send_data(&ssd); // update display
}

double read_sensor(uint8_t channel) {
    if (channel < 26)
        return 0.0;
    adc_select_input(channel - 26);
    double value = (double)adc_read();
    return value * (100.0f / 4095.0f);
}