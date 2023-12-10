#include <Arduino.h>

#include <Tone.h>
#include "../../keyboard/src/constants.h"
#include <Adafruit_MCP3008.h>




Adafruit_MCP3008 inputAdcWhiteKeys;
Adafruit_MCP3008 inputAdcBlackKeys;

Tone speaker0;
Tone speaker1;
Tone speaker2;

constexpr unsigned highThreshold = 512;
constexpr byte emptyByte = 0;


void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    pinMode(3, OUTPUT);

    //Initiate USB serial communication to allow debugging via USB
    Serial.begin(9600);
    //Write startup message to USB serial, 'F' to store string in Flash over RAM
    Serial.println(F("Init"));

    //Initialise speakers
    speaker0.begin(3);
    speaker1.begin(5);
    speaker2.begin(6);

    //Initialise the ADCs managing, white and Black piano keys respectively
    inputAdcWhiteKeys.begin(WHITE_ADC_PINS[3],WHITE_ADC_PINS[2],WHITE_ADC_PINS[1],WHITE_ADC_PINS[0]);inputAdcBlackKeys.begin(BLACK_ADC_PINS[3],BLACK_ADC_PINS[2],BLACK_ADC_PINS[1],BLACK_ADC_PINS[0]);

}

bool readHigh(const unsigned reading)
{
    //Converts analogue reading to boolean (0 or 1) for usage in Bitwise calculations
    return reading > highThreshold;
}





byte readKeys(Adafruit_MCP3008 *keys)
{
    //sets input mask to the empty byte '00000000'
    byte inputMask = emptyByte;

    for(uint8_t channel = 1; channel < 8; channel++)
    {
        //applies bitwise OR between the current channel reading and corresponding bit in the bitmask
        inputMask |= static_cast<byte>(readHigh(keys->readADC(channel)) << channel);
    }

    return inputMask;
}

bool recording = false;
byte whiteBitMask = 0;
byte blackBitMask = 0;

unsigned long startTime = 0;

bool playback = false;

void loop() {

}
