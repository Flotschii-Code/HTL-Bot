#include "Arduino.h"
#include "InterruptTimer.h"
#include "driver/timer.h"

static hw_timer_t* timer = nullptr; // Timer Pointer

volatile uint32_t counter1 = 0;
volatile uint32_t counter2 = 0;

void IRAM_ATTR onTimer(){
    counter1++;
    counter2++;
}

void InterruptTimer_begin(uint32_t interval_us) {
    timer = timerBegin(0, 80, true); // Timer 0, prescaler 80 (1us per tick), count up
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, interval_us, true); // Set alarm to interval_us
    timerAlarmEnable(timer); // Enable the alarm
}

void resetCounter1() {
    counter1 = 0;
}

void resetCounter2() {
    counter2 = 0;
}

uint32_t getCounter1() {
    return counter1;
}

uint32_t getCounter2() {
    return counter2;
}