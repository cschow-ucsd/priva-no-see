#include <Adafruit_CircuitPlayground.h>
#include "calibration.h"
#include "blinds.h"
#include "motor.h"

#define MOTOR_CW A10
#define MOTOR_CCW A9

void setup()
{
    CircuitPlayground.begin();
    Serial.begin(9600);

    //set up motor pins
    pinMode(MOTOR_CW, OUTPUT);
    pinMode(MOTOR_CCW, OUTPUT);
}

void loop()
{
    //log light sensor
    Serial.print("Brightness: "); //log sensor
    Serial.println(CircuitPlayground.lightSensor());

    //create blinds state
    static struct BlindsState state = {
        .isTimerOn = false,
        .lastOverrideMillis = 0L,
        .timerMillis = 0L,
        .isContinuousOverrideOn = false,
        .lastLightState = true,
        .isClosed = false,
        .lightThreshold = MAX_BRIGHT,
        .darkThreshold = MIN_BRIGHT,
        .openCallback = []() {
            motorOpenBlinds(MOTOR_CW, MOTOR_CCW);
        },
        .closeCallback = []() {
            motorCloseBlinds(MOTOR_CW, MOTOR_CCW);
        }};

    //calibrates blinds state properties
    calibrate(&state);

    //controller
    operateBlinds(&state);

    //check computer inputs for debugging
    processSerialInputs(&state);

    //run every second
    delay(1000);
}

//send inputs from computer
void processSerialInputs(struct BlindsState *state)
{
    char input = Serial.read();
    if (input == 'o')
        overrideBlinds(state);
}