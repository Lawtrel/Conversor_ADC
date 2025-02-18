#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

#define GREEN_PIN 11
#define BLUE_PIN 12
#define RED_PIN 13
#define Joy_X 26
#define Joy_Y 27
#define JOYSTIC 22
#define BTN_A 5
#define I2C_SDA 14
#define I2C_SCL 15
#define DEADZONE 50
#define MOVING_AVG_SIZE 5
ssd1306_t display;
uint8_t ssd[ssd1306_buffer_length];
struct render_area frame_area = {
    0, ssd1306_width - 1, 0, ssd1306_n_pages - 1};
bool led_green_state = false;
bool pwm_enabled = true;
int border_style= 0;

int adc_x_buffer[MOVING_AVG_SIZE];
int adc_y_buffer[MOVING_AVG_SIZE];
int buffer_index_x = 0, buffer_index_y = 0;

void init_display();
void update_display(int x, int y);
void draw_border();
int moving_average(int *buffer, int new_value, int *index);

void button_callback(uint gpio, uint32_t events) {
    static uint32_t last_time = 0;
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (now - last_time <200) return;
    last_time = now;

    if (gpio == JOYSTIC)
    {
        led_green_state = !led_green_state;
        gpio_put(GREEN_PIN, led_green_state);
        //altera a borda do display
        border_style = (border_style + 1) % 3;
        draw_border();
    } else if (gpio == BTN_A) {
        pwm_enabled = !pwm_enabled;
        pwm_set_enabled(pwm_gpio_to_slice_num(RED_PIN), pwm_enabled);
        pwm_set_enabled(pwm_gpio_to_slice_num(BLUE_PIN), pwm_enabled);

        if (!pwm_enabled) {
            pwm_set_gpio_level(RED_PIN, 0);
            pwm_set_gpio_level(BLUE_PIN, 0);
        }
    }
}

void setup() {
    //inicializar adc para ler os valores de x e y
    stdio_init_all();
    adc_init();
    adc_gpio_init(Joy_X);
    adc_gpio_init(Joy_Y);
    gpio_set_function(RED_PIN, GPIO_FUNC_PWM);
    gpio_set_function(BLUE_PIN, GPIO_FUNC_PWM);
    gpio_init(GREEN_PIN);
    gpio_set_dir(GREEN_PIN, GPIO_OUT);

    uint slices_red = pwm_gpio_to_slice_num(RED_PIN);
    uint slices_blue = pwm_gpio_to_slice_num(BLUE_PIN);
    //Configurar PWM para Controle de Brilho
    pwm_set_wrap(slices_red, 4095);
    pwm_set_wrap(slices_blue, 4095);
    pwm_set_enabled(slices_red, true);
    pwm_set_enabled(slices_blue, true);

    pwm_set_gpio_level(RED_PIN, 0);
    pwm_set_gpio_level(BLUE_PIN, 0);
    //Configurar Btn com pull-up
    gpio_init(JOYSTIC);
    gpio_set_dir(JOYSTIC, GPIO_IN);
    gpio_pull_up(JOYSTIC);
    gpio_set_irq_enabled_with_callback(JOYSTIC, GPIO_IRQ_EDGE_FALL, true, &button_callback);


    gpio_init(BTN_A);
    gpio_set_dir(BTN_A, GPIO_IN);
    gpio_pull_up(BTN_A);
    gpio_set_irq_enabled_with_callback(BTN_A, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    for (int i = 0; i < MOVING_AVG_SIZE; i++) {
        adc_select_input(0);
        adc_x_buffer[i] = adc_read();
        adc_select_input(1);
        adc_y_buffer[i] = adc_read();
    }

    //inicializa o display
    init_display();
    draw_border();
}
int main() {
    setup();
    int x, y;
    while (1) {
        adc_select_input(0);
        int raw_adc_x = adc_read();
        adc_select_input(1);
        int raw_adc_y = adc_read();

        int adc_x = moving_average(adc_x_buffer, raw_adc_x, &buffer_index_x);
        int adc_y = moving_average(adc_y_buffer, raw_adc_y, &buffer_index_y);

        
        x = ((adc_y - 2048) * 58) / 2048 + 63;
        y = ((2048 - adc_x) * 30) / 2048 + 32;

        if (pwm_enabled) {
            if (adc_x != 2048) pwm_set_gpio_level(RED_PIN, adc_x);
            else pwm_set_gpio_level(RED_PIN, 0);

            if (adc_y != 2048) pwm_set_gpio_level(BLUE_PIN, adc_y);
            else pwm_set_gpio_level(BLUE_PIN, 0);
        }
        update_display(x, y);
        sleep_ms(100);
    }
    
}
int moving_average(int *buffer, int new_value, int *index) {
    buffer[*index] = new_value;
    *index = (*index + 1) % MOVING_AVG_SIZE;
    
    int sum = 0;
    for (int i = 0; i < MOVING_AVG_SIZE; i++) {
        sum += buffer[i];
    }
    return sum / MOVING_AVG_SIZE;
}

void init_display() {
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init();
    calculate_render_area_buffer_length(&frame_area);
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);
}

void update_display(int x, int y) {
    char buffer[20];
    memset(ssd, 0, ssd1306_buffer_length);
    draw_border();
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            ssd1306_set_pixel(ssd, x + i, y + j, true);
        }
    }
    sprintf(buffer, "X: %d Y: %d", x,y);
    ssd1306_draw_string(ssd, 5, 50, buffer);
    render_on_display(ssd, &frame_area);
}

void draw_border() {
    for (int i = 0; i < ssd1306_width; i++) {
        if (border_style == 0 || (i % 4 == 0 && border_style == 1)) {
            ssd1306_set_pixel(ssd, i, 0 , true);
            ssd1306_set_pixel(ssd, i, ssd1306_height - 1 , true);
        }
    }
    for (int i = 0; i < ssd1306_height; i++) {
        if (border_style == 0 || (i % 4 == 0 && border_style == 1)) {
            ssd1306_set_pixel(ssd, 0, i, true);
            ssd1306_set_pixel(ssd, ssd1306_width - 1, i , true);
        }
    }    
}