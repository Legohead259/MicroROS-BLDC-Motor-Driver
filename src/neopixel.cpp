#include "neopixel.h"

#define LED_DIN_PIN 2
#define NUM_LEDS 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

LEDStateMachine ledStateMachine = LEDStateMachine(systemState);

TaskHandle_t ledTask;

bool isLEDOn = false;
uint32_t _lastMillis = 0;

void LEDStateMachine::begin() {
    strip.begin();
    strip.setBrightness(MAX_BRIGHTNESS);
    strip.show(); // Initialize all pixels to 'off'

    // Set up LED task in RTOS
    // xTaskCreatePinnedToCore(
    //     ledTaskCallback, /* Function to implement the task */
    //     "Diagnostic LED", /* Name of the task */
    //     2500,  /* Stack size in words */
    //     NULL,  /* Task input parameter */
    //     1,  /* Priority of the task */
    //     &ledTask,  /* Task handle. */
    //     0); /* Core where the task should run */
}

void LEDStateMachine::executeState() {
    switch (currentState) {
        case IDLE_NO_CONNECTION:
            if (_lastMillis+BLINK_PERIOD <= millis()) {
                if (!isLEDOn) {
                    strip.setPixelColor(0, YELLOW);
                    strip.show();
                    isLEDOn = true;
                } else {
                    strip.setPixelColor(0, LED_OFF);
                    strip.show();
                    isLEDOn = false;
                }
                _lastMillis = millis();
            }
            break;
        
        case IDLE_WITH_CONNECTION:
            break;
        
        default:
            break;
    }
}