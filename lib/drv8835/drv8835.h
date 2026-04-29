#pragma once
#include <stdint.h>

void motor_init(void);
void motor_set_left(int16_t speed);
void motor_set_right(int16_t speed);
