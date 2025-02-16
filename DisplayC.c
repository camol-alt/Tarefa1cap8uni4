#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"

#define VRX_PIN 26
#define VRY_PIN 27
#define SW_PIN 22
#define RED_LED_PIN 13
#define BLUE_LED_PIN 12
#define GREEN_LED_PIN 11
#define BUTTON_A 5
#define I2C_SDA 14
#define I2C_SCL 15
#define DEADZONE 100 // Faixa de tolerância ao redor do valor central

static volatile bool led_state = false;
static volatile bool leds_enabled = true;
static volatile int border_style = 0;

// Adicionando flags para debounce
static volatile bool debounce_joystick = false;
static volatile bool debounce_button_A = false;

uint pwm_init_gpio(uint gpio, uint wrap) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true);
    return slice_num;
}

void debounce(uint gpio) {
    sleep_ms(50); 
    if (!gpio_get(gpio)) { // Se ainda estiver pressionado, espera até soltar
        while (!gpio_get(gpio)) {
            tight_loop_contents(); // Evita travamento da CPU
        }
    }
    sleep_ms(50);
}

// Callback único para os dois botões
void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == SW_PIN && !debounce_joystick) {
        debounce_joystick = true;
        led_state = !led_state;
        gpio_put(GREEN_LED_PIN, led_state);
        border_style = (border_style + 1) % 3; // Alterna entre 3 estilos de borda
        printf("Botão do Joystick pressionado, estado do LED verde: %d, estilo de borda: %d\n", led_state, border_style);
    } 
    else if (gpio == BUTTON_A && !debounce_button_A) {
        debounce_button_A = true;
        leds_enabled = !leds_enabled;
        if (!leds_enabled) {
            pwm_set_gpio_level(RED_LED_PIN, 0);
            pwm_set_gpio_level(BLUE_LED_PIN, 0);
        }
        printf("Botão A pressionado, LEDs PWM %s\n", leds_enabled ? "ativados" : "desativados");
    }
}

void setup_gpio(uint gpio, bool is_input, bool pull_up) {
    gpio_init(gpio);
    gpio_set_dir(gpio, is_input ? GPIO_IN : GPIO_OUT);
    if (pull_up) {
        gpio_pull_up(gpio);
    }
}

int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(VRX_PIN);
    adc_gpio_init(VRY_PIN);

    uint pwm_wrap = 4095;
    uint red_pwm_slice = pwm_init_gpio(RED_LED_PIN, pwm_wrap);
    uint blue_pwm_slice = pwm_init_gpio(BLUE_LED_PIN, pwm_wrap);

    gpio_init(GREEN_LED_PIN);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
    gpio_put(GREEN_LED_PIN, false);

    gpio_init(SW_PIN);
    gpio_set_dir(SW_PIN, GPIO_IN);
    gpio_pull_up(SW_PIN);

    setup_gpio(BUTTON_A, true, true);
    gpio_set_irq_enabled_with_callback(SW_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_t disp;
    ssd1306_init(&disp, 128, 64, false, 0x3C, i2c1);
    ssd1306_config(&disp);

    uint32_t last_print_time = 0;

    while (true) {
        // Reseta debounce quando o botão for solto
        if (debounce_joystick && gpio_get(SW_PIN)) {
            debounce(SW_PIN);
            debounce_joystick = false;
        }
        if (debounce_button_A && gpio_get(BUTTON_A)) {
            debounce(BUTTON_A);
            debounce_button_A = false;
        }

        adc_select_input(1); // Invertendo os eixos X e Y
        uint16_t vrx_value = adc_read();

        adc_select_input(0); // Invertendo os eixos X e Y
        uint16_t vry_value = adc_read();

        if (leds_enabled) {
            if (abs(vrx_value - 2048) < DEADZONE && abs(vry_value - 2048) < DEADZONE) {
                pwm_set_gpio_level(RED_LED_PIN, 0);
                pwm_set_gpio_level(BLUE_LED_PIN, 0);
            } else {
                uint16_t red_led_brightness = abs((int)vrx_value - 2048) * 2;
                uint16_t blue_led_brightness = abs((int)vry_value - 2048) * 2;

                pwm_set_gpio_level(RED_LED_PIN, red_led_brightness);
                pwm_set_gpio_level(BLUE_LED_PIN, blue_led_brightness);
            }
        }

        float red_duty_cycle = (vrx_value / 4095.0) * 100;
        float blue_duty_cycle = (vry_value / 4095.0) * 100;

        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        if (current_time - last_print_time >= 1000) {
            printf("VRX: %u, VRY: %u\n", vrx_value, vry_value);
            printf("Duty Cycle RED LED: %.2f%%\n", red_duty_cycle);
            printf("Duty Cycle BLUE LED: %.2f%%\n", blue_duty_cycle);
        }

        ssd1306_fill(&disp, false);
        uint8_t square_x = (vry_value * 120) / 4095; // Invertendo os eixos X e Y
        uint8_t square_y = ((4095 - vrx_value) * 56) / 4095; // Invertendo os eixos X e Y
        ssd1306_rect(&disp, square_y, square_x, 8, 8, true, true);

        // Alterna entre diferentes estilos de borda
        if (border_style == 1) {
            ssd1306_rect(&disp, 0, 0, 128, 64, true, false);
        } else if (border_style == 2) {
            ssd1306_rect(&disp, 2, 2, 124, 60, true, false);
        }

        ssd1306_send_data(&disp);
        sleep_ms(100);
    }

    return 0;
}