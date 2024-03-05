/**
 * @file neopixel.h
 * @author Braidan Duffy
 * 
 * @section intro Introduction
 * This class integrates the `StateMachine` superclass and defines some NeoPixel-specific functions.
 * The OlympianESC evaluation board uses a NeoPixel RGB LED for user feedback.
 * This includes showing connection and error states, motor controls, and identification testing.
 * The color and pattern of the LED depends on the system state as discussed in the next section.
 * 
 * @section colors_patterns LED Colors and Patterns
 * The NeoPixel RGB LED can be commanded to show a practically infinite range of colors in 255 brightness steps.
 * For the system states defined in `state_machine.h`, we can build a table for colors and patterns users can
 * associate with each state:
 * 
 * | System State           | Color(s)          | Pattern               |
 * | -----------------------|-------------------|-----------------------|
 * | IDLE_NO_CONNECTION     | Yellow            | Blinking              |
 * | IDLE_WITH_CONNECTION   | Yellow            | Solid                 |
 * | PARTIAL_FORWARD        | Green             | Blinking              |
 * | PARTIAL_REVERSE        | Red               | Blinking              |
 * | FULL_FORWARD           | Green             | Solid                 |
 * | FULL_REVERSE           | Red               | Solid                 |
 * | DEVICE_IDENTIFY        | White, Magenta    | Blinking, alternating |
 * | MOTOR_DISABLED         | Orange, Blue      | Blinking, alternating |
 * | SENSOR_FAULT           | Orange, Magenta   | Blinking, alternating |
 * | DRIVER_FAULT           | Orange, Cyan      | Blinking, alternating |
 * | TRANSPORT_FAULT        | Orange, Yellow    | Blinking, alternating |
 * 
 * @version 1.0
 * @date 2024-03-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef NEOPIXEL_H
#define NEOPIXEL_H

#include <Adafruit_NeoPixel.h>
#include "state_machine.h"

#define NUM_PIXELS 1
#define MAX_BRIGHTNESS 32
#define BLINK_PERIOD 500 // ms
#define IDENTIFY_TIMEOUT 5000 // ms

extern Adafruit_NeoPixel strip;

const uint32_t LED_OFF  = strip.Color(0, 0, 0);
const uint32_t WHITE    = strip.Color(255, 255, 255);
const uint32_t BLUE     = strip.Color(0, 0, 255);
const uint32_t GREEN    = strip.Color(0, 255, 0);
const uint32_t RED      = strip.Color(255, 0, 0);
const uint32_t CYAN     = strip.Color(0, 255, 255);
const uint32_t MAGENTA  = strip.Color(255, 0, 255);
const uint32_t YELLOW   = strip.Color(255, 255, 0);
const uint32_t ORANGE   = strip.Color(255, 165, 0);

// Class representing the state machine
class LEDStateMachine : public StateMachine {
public:
    /**
     * @brief Construct a new LEDStateMachine object
     * 
     * @param currentStatePtr Reference to the global `currentState` variable
     */
    LEDStateMachine(State &currentStatePtr) : StateMachine(currentStatePtr) {}

    /**
     * @brief Initializes the NeoPixel LED and sets its brightness to `MAX_BRIGHTNESS`
     * 
     */
    void begin(void);

    /**
     * @brief Executes various LED state tasks based on the current system state.
     * This should be executed in a continuous loop as it needs constant updating.
     * Any LED functions executed within should be asynchronous (non-blocking).
     * If applicable, they should also be thread safe.
     */
    void executeState();

private:
    /**
     * @brief Sets the NeoPixel LED to the color
     * 
     * @param color Desired color to display on the LED.
     */
    void setLEDColor(uint32_t color);

    /**
     * @brief Asynchronously blinks the LED without blocking code execution (like when using `delay()`)
     * 
     * @param color Desired color to display on the LED
     * @param interval Interval the LED stays ON and OFF
     */
    void asyncBlink(uint32_t color, uint32_t interval=BLINK_PERIOD);

    /**
     * @brief Asynchronously alternates between two different LED colors.
     * 
     * @param firstColor The first desired color to display on the LED.
     * @param secondColor The second desired color to display on the LED.
     * @param interval The interval each color will be displayed before switching.
     */
    void asyncAlternatingBlink(uint32_t firstColor, uint32_t secondColor, uint32_t interval=BLINK_PERIOD);
};

extern LEDStateMachine ledStateMachine;

#endif // NEOPIXEL_H