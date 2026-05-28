#include <Arduino.h>
#include <FastLED.h>
#include "pins.h"
#include "driver/gpio.h"

#define PROG  1   // 1: Ladeprogramm, 2: Hauptprogramm
#define ADC_1V 775

CRGB leds[NUM_LEDS];

void ledAllBlack();
void ledAllColour(CRGB colour);

#if(PROG == 1)

//Lade
void setup(){

  pinMode(MOTOR_R_DIR, OUTPUT); 
  pinMode(MOTOR_R_PWM, OUTPUT);
  pinMode(MOTOR_L_DIR, OUTPUT);
  pinMode(MOTOR_L_PWM, OUTPUT);

  digitalWrite(MOTOR_R_DIR, LOW);
  digitalWrite(MOTOR_R_PWM, LOW);
  digitalWrite(MOTOR_L_DIR, LOW);
  digitalWrite(MOTOR_L_PWM, LOW);

  pinMode(LED_COLOURSENSOR, OUTPUT);
  digitalWrite(LED_COLOURSENSOR, LOW);

  //LED initialisieren
  FastLED.addLeds<SK9822,DATA_PIN,CLOCK_PIN,RBG>(leds, NUM_LEDS);

  leds[LED_RV] = CRGB::Green;
  leds[LED_RH] = CRGB::Black;
  leds[LED_LV] = CRGB::Black;
  leds[LED_LH] = CRGB::Black;
  FastLED.show();

  leds[LED_RV] = CRGB::Black;
  FastLED.show();

  //ESP Sleep
  esp_sleep_enable_timer_wakeup(4000000UL);
  esp_deep_sleep_start();
}

void loop(){

}

#elif(PROG == 2)

#include <BluetoothSerial.h>
#include <driver/adc.h>

void checkVoltage();

//Haupt
void setup(){

  Serial.begin(115200);

  pinMode(ADC_UB, INPUT);

  //LED initialisieren
  FastLED.addLeds<SK9822,DATA_PIN,CLOCK_PIN,RBG>(leds, NUM_LEDS);

  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten((adc1_channel_t)ADC_UB, ADC_ATTEN_DB_12);
  gpio_set_direction((gpio_num_t)BUTTONSLEFT, GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)BUTTONSRIGHT, GPIO_MODE_INPUT);
  gpio_set_pull_mode((gpio_num_t)BUTTONSLEFT, GPIO_PULLUP_ONLY);
  gpio_set_pull_mode((gpio_num_t)BUTTONSRIGHT, GPIO_PULLUP_ONLY);

  pinMode(MOTOR_R_DIR, OUTPUT); 
  pinMode(MOTOR_R_PWM, OUTPUT);
  pinMode(MOTOR_L_DIR, OUTPUT);
  pinMode(MOTOR_L_PWM, OUTPUT);
}

void loop(){
  checkVoltage();  
  if(gpio_get_level((gpio_num_t)BUTTONSLEFT) || gpio_get_level((gpio_num_t)BUTTONSRIGHT)){
    leds[LED_RV] = CRGB::Purple;
    leds[LED_LV] = CRGB::Purple;
    FastLED.show();
  }
}

void checkVoltage(){
  int start = 0;
  int adcCount = 0;
  unsigned long batteryVoltage = 0;
  unsigned long durchschnitt = 0;

  if(millis() >= start + 10){
    start = millis();

    while(adcCount < 200){
      batteryVoltage = batteryVoltage + adc1_get_raw((adc1_channel_t)ADC_UB);
      adcCount++;
    }
    if(adcCount >= 200){
      durchschnitt = batteryVoltage/adcCount;
      Serial.printf("Durchschnitt: %u\n", durchschnitt);
      if(durchschnitt > 3.69 * ADC_1V){
        
        ledAllColour(CRGB::Green);
        
        delay(20);

        ledAllBlack();

        adcCount = 0;
      } else if(durchschnitt <= 3.69 * ADC_1V && durchschnitt > 3.3 * ADC_1V){
        
        ledAllColour(CRGB::Yellow);

        delay(20);

        ledAllBlack();

        adcCount = 0;
      } else if(durchschnitt < 3.3 * ADC_1V){
        while(1){
          digitalWrite(MOTOR_R_DIR, LOW);
          digitalWrite(MOTOR_R_PWM, LOW);
          digitalWrite(MOTOR_L_DIR, LOW);
          digitalWrite(MOTOR_L_PWM, LOW);

          pinMode(LED_COLOURSENSOR, OUTPUT);
          digitalWrite(LED_COLOURSENSOR, LOW);

          //LED initialisieren
          FastLED.addLeds<SK9822,DATA_PIN,CLOCK_PIN,RBG>(leds, NUM_LEDS);

          ledAllColour(CRGB::Red);

          delay(20);

          ledAllBlack();

          //ESP Sleep
          esp_sleep_enable_timer_wakeup(4000000UL);
          esp_deep_sleep_start();
        }
      }
    }
  } 
}

#endif



void ledAllBlack(){
  leds[LED_RV] = CRGB::Black;
  leds[LED_RH] = CRGB::Black;
  leds[LED_LV] = CRGB::Black;
  leds[LED_LH] = CRGB::Black;
  FastLED.show();
}

void ledAllColour(CRGB colour){
  leds[LED_RV] = colour;
  leds[LED_RH] = colour;
  leds[LED_LV] = colour;
  leds[LED_LH] = colour;
  FastLED.show();
}