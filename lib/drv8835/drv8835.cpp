#include "drv8835.h"

#include <pins.h>
#include <bit.h>

#include "driver/ledc.h"
#include "soc/gpio_struct.h"

static void motor_gpio_init(void) {
    GPIO.enable_w1ts = BIT(MOTOR_R_DIR);
    GPIO.enable_w1ts = BIT(MOTOR_L_DIR);

    GPIO.out_w1tc = BIT(MOTOR_R_DIR);
    GPIO.out_w1tc = BIT(MOTOR_L_DIR);
}

static void motor_pwm_timer_init(void) {
    ledc_timer_config_t timer = {}; // alles auf 0

    timer.speed_mode = LEDC_HIGH_SPEED_MODE;
    timer.timer_num = LEDC_TIMER_0;
    timer.duty_resolution = LEDC_TIMER_8_BIT;
    timer.freq_hz = 20000;
    timer.clk_cfg = LEDC_AUTO_CLK;

    ledc_timer_config(&timer);
}


static void motor_pwm_channel_init(void) {
    ledc_channel_config_t right = {
        .gpio_num = MOTOR_R_PWM,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&right);

    ledc_channel_config_t left = {
        .gpio_num = MOTOR_L_PWM,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&left);
}

void motor_init(void) {
    motor_gpio_init();
    motor_pwm_timer_init();
    motor_pwm_channel_init();
}

static void set_dir(uint8_t pin, bool forward) {
    if (forward)
        GPIO.out_w1tc = BIT(pin);
    else
        GPIO.out_w1ts = BIT(pin);
}

void motor_set_right(int16_t speed) {
    bool forward = speed >= 0;
    uint8_t duty = forward ? speed : -speed;

    set_dir(MOTOR_R_DIR, forward);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
}

void motor_set_left(int16_t speed) {
    bool forward = speed >= 0;
    uint8_t duty = forward ? speed : -speed;

    set_dir(MOTOR_L_DIR, forward);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
}
