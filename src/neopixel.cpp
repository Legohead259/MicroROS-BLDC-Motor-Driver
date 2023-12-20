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
}

void LEDStateMachine::executeState() {
    switch (currentState) {
        case IDLE_NO_CONNECTION:
            asyncBlink(YELLOW);
            break;
        
        case IDLE_WITH_CONNECTION:
            strip.setPixelColor(0, YELLOW);
            strip.show();
            isLEDOn = true;
            break;

        case PARTIAL_FORWARD:
            asyncBlink(GREEN, 125);
            break;

        case PARTIAL_REVERSE:
            asyncBlink(RED, 125);
            break;
        
        default:
            break;
    }
}

void LEDStateMachine::asyncBlink(uint32_t color, uint32_t interval) {
    if (_lastMillis+interval <= millis()) { // Asynchronous blink
        if (!isLEDOn) {
            strip.setPixelColor(0, color);
            strip.show();
            isLEDOn = true;
        } else {
            strip.setPixelColor(0, LED_OFF);
            strip.show();
            isLEDOn = false;
        }
        _lastMillis = millis();
    }
}