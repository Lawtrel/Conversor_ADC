#include <stdio.h>
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
#define Joy 22
#define BTN_A 5
#define I2C_SDA 14
#define I2C_SCL 15
int main() {
    stdio_init_all();

}
