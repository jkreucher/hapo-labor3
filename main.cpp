/* HAPO Labor Versuch 3
 * ********************
 *
 * Jannik Kreucher
 * Benedikt Wendling
 */

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
#define PIN_POTI    A0

// user parameters (times 2.5ms)
#define BUTTON_LONG     200
#define BUTTON_SHORT    20


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

// states
enum eDisplayMode {
    StateCompare,
    StateSettings
};


/*** global vars ***/
// main vars
volatile uint8_t bMainUpdate = 0;
// display vars
uint8_t bDisplayDigit = 0;
uint8_t bDisplayPoint = 4; // decimal point place (4 for none)
uint8_t bDisplayValues[4] = {16, 16, 16, 16}; // display values for each digit
uint8_t bDisplayState = StateCompare;
uint16_t wDecimalCounter = 0;
// adc stuff
int16_t wAdcValue = 0;
int16_t wAdcSetpoint = 1600;
uint8_t bAdcEnableCompare = 0;
// buttons
uint16_t bSW1_Timer = 0;
uint16_t bSW2_Timer = 0;
uint16_t bSW3_Timer = 0;
uint8_t bButtonUpEnable = 0;
uint16_t bButtonUpCounter = 0;
uint8_t bButtonDownEnable = 0;
uint16_t bButtonDownCounter = 0;
// serial port
char serialBuffer[16];


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
AnalogIn pinPot(PIN_POTI);
BufferedSerial serialPort(USBTX, USBRX);


/*** Prototypes ***/
void vMainUpdate();
void vShiftOut(uint16_t wData);
void vDisplayUpdate();
void vCheckButtons();


// main() runs in its own thread in the OS
int main() {
    // leds off
    busLeds = 0xF;
    pinBuzzer = 1;
    // default state
    bDisplayState = StateCompare;

    // serial setup
    serialPort.set_baud(115200);
    serialPort.set_format(8, BufferedSerial::None, 1); // 8n1
    serialPort.set_blocking(false);

    // main update interrupt
    tMainUpdate.attach(&vMainUpdate, 2500us);

    while(1) {
        // check serial port
        if(uint32_t num = serialPort.read(serialBuffer, sizeof(serialBuffer)) > 0) {
            // write back
            switch(serialBuffer[0]) {
                case 'a':
                    // change state to compare
                    bDisplayState = StateCompare;
                    // enable/disable compare
                    if(bAdcEnableCompare) {
                        bAdcEnableCompare = 0;
                    } else {
                        bAdcEnableCompare = 1;
                    }
                    break;

                case '+':
                    // change state to settings
                    bDisplayState = StateSettings;
                    // increment setpoint
                    if(wAdcSetpoint < 3300) wAdcSetpoint += 10;
                    else wAdcSetpoint = 3300;
                    break;

                case '-':
                    // change state to settings
                    bDisplayState = StateSettings;
                    // decrement setpoint
                    if(wAdcSetpoint > 0) wAdcSetpoint -= 10;
                    else wAdcSetpoint = 0;
                    break;
            }
        }
        

        // recalculate
        wAdcValue = (float)(3.3 * pinPot) * 1000.0;

        // compare setpoint to actual value
        if((wAdcValue > wAdcSetpoint) && (bAdcEnableCompare)) pinBuzzer = 0;
        else pinBuzzer = 1;

        // update stuff
        vDisplayUpdate();
        vCheckButtons();

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
    // state machine
    switch(bDisplayState) {
        case StateCompare:
            // show if compre mode is enabled
            if(bAdcEnableCompare) bDisplayValues[3] = 0xC;
            else bDisplayValues[3] = 16;
            // show analog voltage
            bDisplayPoint = 2;
            bDisplayValues[0] = (wAdcValue /   10) % 10;
            bDisplayValues[1] = (wAdcValue /  100) % 10;
            bDisplayValues[2] = (wAdcValue / 1000) % 10;
            break;
        
        case StateSettings:
            // blink decimal point
            if(wDecimalCounter == 0) {
                // reset timer
                wDecimalCounter = 400;
                // toggle decimal point
                if(bDisplayPoint < 4) bDisplayPoint = 4;
                else bDisplayPoint = 2;
            } else {
                // decrement timer
                wDecimalCounter--;
            }
            // show set voltage
            bDisplayValues[0] = (wAdcSetpoint /   10) % 10;
            bDisplayValues[1] = (wAdcSetpoint /  100) % 10;
            bDisplayValues[2] = (wAdcSetpoint / 1000) % 10;
            bDisplayValues[3] = 16;
            break;
    }

    if(bDisplayPoint == bDisplayDigit)
        vShiftOut(segment_digit[bDisplayDigit] | segment_data[bDisplayValues[bDisplayDigit]] & ~(1 << 15));
    else
        vShiftOut(segment_digit[bDisplayDigit] | segment_data[bDisplayValues[bDisplayDigit]]);

    bDisplayDigit++;
    if(bDisplayDigit > 3) bDisplayDigit = 0;
}


void vCheckButtons() {
    // check switch 1
    if(pinSW1 == 0) {
        if(bSW1_Timer <= BUTTON_LONG) {
            if(bSW1_Timer == BUTTON_SHORT) {
                // event: short press
            }
            if(bSW1_Timer == BUTTON_LONG) {
                // event: long press
                // enable/disable compare
                if(bAdcEnableCompare) bAdcEnableCompare = 0;
                else bAdcEnableCompare = 1;
            }
            bSW1_Timer++;
        } 
    } else {
        if((bSW1_Timer >= BUTTON_SHORT) && (bSW1_Timer <= BUTTON_LONG)) {
            // event: released from short
            // switch between states
            if(bDisplayState == StateCompare) bDisplayState = StateSettings;
            else if(bDisplayState == StateSettings) bDisplayState = StateCompare;
        }
        if(bSW1_Timer > BUTTON_LONG) {
            // event: released from long
        }
        bSW1_Timer = 0;
    }

    // check switch 2
    if(pinSW2 == 0) {
        if(bSW2_Timer <= BUTTON_LONG) {
            if(bSW2_Timer == BUTTON_SHORT) {
                // event: short press
            }
            if(bSW2_Timer == BUTTON_LONG) {
                // event: long press
                if(bDisplayState == StateSettings)
                    bButtonUpEnable = 1;
            }
            bSW2_Timer++;
        } 
    } else {
        if((bSW2_Timer >= BUTTON_SHORT) && (bSW2_Timer <= BUTTON_LONG)) {
            // event: released from short
            if(bDisplayState == StateSettings) {
                if(wAdcSetpoint < 3300) wAdcSetpoint += 10;
                else wAdcSetpoint = 3300;
            }
        }
        if(bSW2_Timer > BUTTON_LONG) {
            // event: released from long
            bButtonUpEnable = 0;
        }
        bSW2_Timer = 0;
    }

    // check switch 3
    if(pinSW3 == 0) {
        if(bSW3_Timer <= BUTTON_LONG) {
            if(bSW3_Timer == BUTTON_SHORT) {
                // event: short press
            }
            if(bSW3_Timer == BUTTON_LONG) {
                // event: long press
                if(bDisplayState == StateSettings)
                    bButtonDownEnable = 1;
            }
            bSW3_Timer++;
        } 
    } else {
        if((bSW3_Timer >= BUTTON_SHORT) && (bSW3_Timer <= BUTTON_LONG)) {
            // event: released from short
            if(bDisplayState == StateSettings) {
                if(wAdcSetpoint > 0) wAdcSetpoint -= 10;
                else wAdcSetpoint = 0;
            }
        }
        if(bSW3_Timer > BUTTON_LONG) {
            // event: released from long
            bButtonDownEnable = 0;
        }
        bSW3_Timer = 0;
    }


    // check button counters
    if(bButtonUpEnable) {
        if(bButtonUpCounter == 0) {
            // reset counter
            bButtonUpCounter = 40;
            // count up
            if(wAdcSetpoint < 3300) wAdcSetpoint += 100;
            else wAdcSetpoint = 3300;
        } else {
            bButtonUpCounter--;
        }
    } else {
        bButtonUpCounter = 0;
    }

    if(bButtonDownEnable) {
        if(bButtonDownCounter == 0) {
            // reset counter
            bButtonDownCounter = 40;
            // count down
            if(wAdcSetpoint > 0) wAdcSetpoint -= 100;
            else wAdcSetpoint = 0;
        } else {
            bButtonDownCounter--;
        }
    } else {
        bButtonDownCounter = 0;
    }
}