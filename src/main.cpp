#include <Arduino.h>
#include <FastLED.h>
#include "../lib/pins.h"
#include "driver/gpio.h"
#include "InterruptTimer.h"

#define PROG  2   // 1: Ladeprogramm, 2: Hauptprogramm
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

  pinMode(MOTOR_R_DIR, OUTPUT); 
  pinMode(MOTOR_R_PWM, OUTPUT);
  pinMode(MOTOR_L_DIR, OUTPUT);
  pinMode(MOTOR_L_PWM, OUTPUT);

  pinMode(BUTTONSLEFT, INPUT_PULLUP);
  pinMode(BUTTONSRIGHT, INPUT_PULLUP);

}

void loop(){
  checkVoltage();  
  if(digitalRead(BUTTONSLEFT) == 0 || digitalRead(BUTTONSRIGHT) == 0){
    leds[LED_RV] = CRGB::Purple;
    leds[LED_LV] = CRGB::Purple;
    FastLED.show();
    delay(500);
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
      batteryVoltage = batteryVoltage + analogRead((ADC_UB));
      adcCount++;
    }
    if(adcCount >= 200){
      durchschnitt = batteryVoltage/adcCount;
      Serial.printf("Durchschnitt: %u\n", durchschnitt);
      if(durchschnitt > 3.69 * ADC_1V){
        
        ledAllColour(CRGB::Green);

        adcCount = 0;
      } else if(durchschnitt <= 3.69 * ADC_1V && durchschnitt > 3.3 * ADC_1V){
        
        ledAllColour(CRGB::Yellow);

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