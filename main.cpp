#include "mbed.h"
#include <cstdint>

// Nucleo On-Board
#define PIN_NUC_LED PA_5
#define PIN_NUC_SW  PC_13
// Shield
#define PIN_SW1     PA_1
#define PIN_SW2     PA_4
#define PIN_SW3     PB_0
#define PIN_DOUT    PA_9
#define PIN_SCLK    PA_8
#define PIN_LTCH    PB_5
#define PIN_LED1    PB_6
#define PIN_LED2    PA_7
#define PIN_LED3    PA_6
#define PIN_LED4    PA_5
#define PIN_BUZZER  PB_3


/*** 7 segment display data ***/
//  0bHGFEDCBA active low
const uint16_t segment_data[17] = {
    //HGFEDCBAxxxx1234
    0b1100000000000000, // 0
    0b1111100100000000, // 1
    0b1010010000000000, // 2
    0b1011000000000000, // 3
    0b1001100100000000, // 4
    0b1001001000000000, // 5
    0b1000001000000000, // 6
    0b1111100000000000, // 7
    0b1000000000000000, // 8
    0b1001000000000000, // 9
    0b1000100000000000, // A
    0b1000001100000000, // B
    0b1100011000000000, // C
    0b1010000100000000, // D
    0b1000011000000000, // E
    0b1000111000000000, // F
    0b1111111100000000, // Off
};
// 7 segment display digits
const uint16_t segment_digit[4] = {
    //xxxx1234
    0b00001000,
    0b00000100,
    0b00000010,
    0b00000001
};


/*** global vars ***/
// main vars
uint8_t bMainUpdate = 0;
// display vars
uint8_t bDisplayDigit = 0;
uint8_t bDisplayPoint = 4; // decimal point place (4 for none)
uint8_t bDisplayValues[4] = {16, 16, 16, 16}; // display values for each digit


Ticker tMainUpdate;

/*** Input / Output ***/
//BusOut busLeds(PIN_LED1, PIN_LED2, PIN_LED3, PIN_LED4);
BusOut busLeds(PIN_LED4, PIN_LED3, PIN_LED2, PIN_LED1);
DigitalIn pinSW1(PIN_SW1);
DigitalIn pinSW2(PIN_SW2);
DigitalIn pinSW3(PIN_SW3);
DigitalOut pinDOUT(PIN_DOUT);
DigitalOut pinSCLK(PIN_SCLK);
DigitalOut pinLTCH(PIN_LTCH);
DigitalOut pinBuzzer(PIN_BUZZER);


// prototypes
void vMainUpdate();
void vShiftOut(uint16_t wData);
void vDisplayUpdate();


// main() runs in its own thread in the OS
int main() {
    // main update interrupt
    tMainUpdate.attach(&vMainUpdate, 2500us);
    // leds off
    busLeds = 0xF;

    while(1) {
        // update stuff
        vDisplayUpdate();

        // wait for next update period
        while(bMainUpdate);
        bMainUpdate=1;
    }
}


void vMainUpdate() {
    bMainUpdate = 0;
}


void vShiftOut(uint16_t wData) {
    for(uint8_t i=0; i<16; i++) {
        // MSB first
        pinDOUT = (wData & (0x8000 >> i));
        // send clock pulse
        pinSCLK = 1;
        pinSCLK = 0;
    }
    // latch outputs
    pinLTCH = 1;
    pinLTCH = 0;
}


void vDisplayUpdate() {
    if(bDisplayPoint == bDisplayDigit)
        vShiftOut(segment_digit[bDisplayDigit] | segment_data[bDisplayValues[bDisplayDigit]] & ~(1 << 15));
    else
        vShiftOut(segment_digit[bDisplayDigit] | segment_data[bDisplayValues[bDisplayDigit]]);

    bDisplayDigit++;
    if(bDisplayDigit > 3) bDisplayDigit = 0;
}