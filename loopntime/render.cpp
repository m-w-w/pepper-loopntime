/*

Bela Pepper LoopnTime Class

Built with project settings:
- Block size: 128 (64 seems ok but > MSW)
- Analog channels: 8
- Analog sample rate: 22050
- Digital channels: 16
- DAC level: 0
- ADC level: 0 
- PGA gain left/right: 0/0 

*/

#include <Bela.h>
#include <LoopnTime.h>
#include <libraries/Gui/Gui.h>

// Pin declarations
// https://github.com/BelaPlatform/bela-pepper/wiki/Pin-numbering
const unsigned int kLED01 = 6; // Pin number of the LED 01 - active track
const unsigned int kLED02 = 7; // Pin number of the LED 02 - active track
const unsigned int kLED03 = 10; // Pin number of the LED 03 - active track
const unsigned int kLED04 = 2; // Pin number of the LED 04 - active track
const unsigned int kLED05 = 3; // Pin number of the LED 05 - recording status
const unsigned int kLED06 = 0; // Pin number of the LED 06 - recording status
const unsigned int kLED07 = 1; // Pin number of the LED 07 - recording status
const unsigned int kLED08 = 4; // Pin number of the LED 08 - clock in
//const unsigned int kLED09 = 5; // Pin number of the LED 09 - unassigned
const unsigned int kLED10 = 8; // Pin number of the LED 10 - clock out

const unsigned int kButton1 = 15; // Pin number of pushbutton 1 - unassigned
const unsigned int kButton2 = 14; // Pin number of pushbutton 2 - unassigned
const unsigned int kButton3 = 13; // Pin number of pushbutton 3 - unassigned
const unsigned int kButton4 = 12; // Pin number of pushbutton 4 - unassigned

const unsigned int kAnalogInput0 = 0; // Number of analog channel 0 - clock in
const unsigned int kAnalogInput1 = 1; // Number of analog channel 1 - clock divider/mult
//const unsigned int kAnalogInput2 = 2; // Number of analog channel 2 - unassigned
//const unsigned int kAnalogInput3 = 3; // Number of analog channel 3 - unassigned
//const unsigned int kAnalogInput4 = 4; // Number of analog channel 4 - unassigned
//const unsigned int kAnalogInput5 = 5; // Number of analog channel 5 - unassigned
//const unsigned int kAnalogInput6 = 6; // Number of analog channel 6 - unassigned
const unsigned int kAnalogInput7 = 7; // Number of analog channel 7 - loop selector

const unsigned int kAnalogOutput0 = 0; // Number of analog channel 0 - clock out
//const unsigned int kAnalogOutput1 = 1; // Number of analog channel 1 - unassigned
//const unsigned int kAnalogOutput2 = 2; // Number of analog channel 2 - unassigned
//const unsigned int kAnalogOutput3 = 3; // Number of analog channel 3 - unassigned
//const unsigned int kAnalogOutput4 = 4; // Number of analog channel 4 - unassigned
//const unsigned int kAnalogOutput5 = 5; // Number of analog channel 5 - unassigned
//const unsigned int kAnalogOutput6 = 6; // Number of analog channel 6 - unassigned
//const unsigned int kAnalogOutput7 = 7; // Number of analog channel 7 - unassigned

// Audio frames per analog frame
int gAFPAF;

// loopntime class
LoopnTime loopntime;

// gui class
Gui gui;

// vector for button reading
std::vector<int> buttonDigitalRead;

// set up
bool setup(BelaContext *context, void *userData) {
    
    // Set up analog frames per audio frames
    gAFPAF = context->audioFrames / context->analogFrames;
    
    // Set up loopntime class
    int numLoops = 4; // how many simultaneous loop tracks
    int maxBufferLength = 15; // seconds
    bool verbose = false; // print information to cout (mode switch party)
    loopntime.setup(context->audioSampleRate, numLoops, maxBufferLength, verbose);

    // Set up the GUI
    gui.setup(context->projectName);

    // Set LED pin to an output
    pinMode(context, 0, kLED01, OUTPUT);
    pinMode(context, 0, kLED02, OUTPUT);
    pinMode(context, 0, kLED03, OUTPUT);
    pinMode(context, 0, kLED04, OUTPUT);
    pinMode(context, 0, kLED05, OUTPUT);
    pinMode(context, 0, kLED06, OUTPUT);
    pinMode(context, 0, kLED07, OUTPUT);
    pinMode(context, 0, kLED08, OUTPUT);
    pinMode(context, 0, kLED10, OUTPUT);
 
    return true;
}

void render(BelaContext *context, void *userData) {

    // Send information to the gui (comment out if not using)
    gui.sendBuffer(0, loopntime.lc.guiInfo_);
    
    for (unsigned int n = 0; n < context->audioFrames; n++) {
        
        // Read analog inputs 
        float clockIn = analogRead(context, n/gAFPAF, kAnalogInput0);
        float multDiv = analogRead(context, n/gAFPAF, kAnalogInput1);
        float loopSelect = analogRead(context, n/gAFPAF, kAnalogInput7);
        
        // Process the button states
        buttonDigitalRead = {
            digitalRead(context, n, kButton1),    
            digitalRead(context, n, kButton2),
            digitalRead(context, n, kButton3),
            digitalRead(context, n, kButton4),
        };

        // Read audio inputs 
        float audioIn = audioRead(context,n,0);
        
        // Process the loop and return audio output
        float audioOut = loopntime.process(clockIn, multDiv, audioIn, loopSelect, buttonDigitalRead);

        // Write the audio input and output to different channels
        audioWrite(context, n, 0, audioIn);     // right channel
        audioWrite(context, n, 1, audioOut);    // left channel
        
        // Write clock to analog out
        analogWrite(context, n/gAFPAF, kAnalogOutput0, loopntime.nc.pulseOutput_);

        // Write LED states
        std::vector<int> activeLoopStates = {0,0,0,0}; 
        activeLoopStates[loopntime.lc.whichLoop_] = 1; // LED of active loop
        digitalWriteOnce(context, n, kLED01, activeLoopStates[0]);          // Loop 1 is active
        digitalWriteOnce(context, n, kLED02, activeLoopStates[1]);          // Loop 2 is active 
        digitalWriteOnce(context, n, kLED03, activeLoopStates[2]);          // Loop 3 is active
        digitalWriteOnce(context, n, kLED04, activeLoopStates[3]);          // Loop 4 is active
        digitalWriteOnce(context, n, kLED05, loopntime.lc.recLED_);         // Currently recording (shared)
        digitalWriteOnce(context, n, kLED06, loopntime.lc.recLED_);         // Currently recording (shared)
        digitalWriteOnce(context, n, kLED07, loopntime.lc.recLED_);         // Currently recording (shared)
        digitalWriteOnce(context, n, kLED08, loopntime.nc.pulseOutput_);    // Clock out
        digitalWriteOnce(context, n, kLED10, loopntime.nc.pulseInput_);     // Clock in

    }

}

void cleanup(BelaContext *context, void *userData)
{

}
