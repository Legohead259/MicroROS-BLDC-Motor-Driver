#include "neopixel.h"

#define LED_DIN_PIN 2
#define NUM_LEDS 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_DATA, NEO_GRB + NEO_KHZ800);

LEDStateMachine ledStateMachine = LEDStateMachine(systemState);

TaskHandle_t ledTask;

bool isLEDOn = false;
uint32_t _lastMillis = 0;
uint32_t _deviceIdentTimerStart = 0;

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
            setLEDColor(YELLOW);
            break;

        case PARTIAL_FORWARD:
            asyncBlink(GREEN, 125);
            break;

        case PARTIAL_REVERSE:
            asyncBlink(RED, 125);
            break;

        case FULL_FORWARD:
            setLEDColor(GREEN);
            break;

        case FULL_REVERSE:
            setLEDColor(RED);
            break;

        case DEVICE_IDENTIFY:
            _deviceIdentTimerStart = (_deviceIdentTimerStart == 0) ? millis() : _deviceIdentTimerStart;

            if (millis()-_deviceIdentTimerStart < IDENTIFY_TIMEOUT) {
                asyncAlternatingBlink(WHITE, MAGENTA, 200);
            }
            else { // Return to previous system state after timeout ends
                changeSystemState(previousSystemState);
                _deviceIdentTimerStart = 0; // Reset timer
            }
            break;

        case MOTOR_DISABLED:
            asyncAlternatingBlink(ORANGE, BLUE);
            break;

        case SENSOR_FAULT:
            asyncAlternatingBlink(ORANGE, MAGENTA);
            break;

        case DRIVER_FAULT:
            asyncAlternatingBlink(ORANGE, CYAN);
            break;

        case TRANSPORT_FAULT:
            asyncAlternatingBlink(ORANGE, YELLOW);
            break;

        default:
            break;
    }
}

void LEDStateMachine::setLEDColor(uint32_t color) {
    strip.setPixelColor(0, color);
    strip.show();
    isLEDOn = !(color == LED_OFF);
}

void LEDStateMachine::asyncBlink(uint32_t color, uint32_t interval) {
    if (_lastMillis+interval <= millis()) { // Asynchronous blink
        isLEDOn ? setLEDColor(LED_OFF) : setLEDColor(color);
        _lastMillis = millis();
    }
}

void LEDStateMachine::asyncAlternatingBlink(uint32_t firstColor, uint32_t secondColor, uint32_t interval) {
    static bool _colorFlag = true;
    if (_lastMillis+interval <= millis()) {
        _colorFlag ? setLEDColor(firstColor) : setLEDColor(secondColor);
        _colorFlag = !_colorFlag; // Flip the color flag
        _lastMillis = millis(); // Update blink timer
    }
}