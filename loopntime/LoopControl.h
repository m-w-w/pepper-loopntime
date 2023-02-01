/***** LoopControl.h *****/

// Class for controlling a loop

#pragma once
#include "LoopControl.h"
#include "Loop.h"
#include <libraries/Debounce/Debounce.h>

class LoopControl {

    public:

        // Constructor
        LoopControl() {};

        // Setup
        bool setup(int sampleRate, int numLoops, int maxBufferLength, bool autoPlay, bool verbose);
        
        // Select loop
        void loopSelector(float loopSelect);

        // Interface button 1 Rec/Overdub
        void button1RecOverdub();

        // Interface button 2 Play/Stop
        void button2PlayStop();

        // Interface button 3 Undo
        void button3Undo();

        // Interface button 4 Reset
        void button4Reset();

        // Debounce and trigger buttons
        void buttons(std::vector<int> buttonDigitalRead);

        // Send information to gui at frame rate
        void gui();

        // Should the recording LED be on (i.e., any loop recording)
        void recLED();

        // Process all buttons and loops
        float process(float in, float loopSelect, std::vector<int> buttonDigitalRead);

       // Destructor
        ~LoopControl() {};

        // Declare
        std::vector<Loop> loop;
        int numLoops_;
        int whichLoop_;
        Debounce debounceB1_;
        Debounce debounceB2_;
        Debounce debounceB3_;
        Debounce debounceB4_;
        float fps_ = 30;                    // frames per second for the gui
        int spfGUI_;                        // samples per frame for the gui
        int GUI_c_ = -1;
        std::vector<int> guiInfo_;
        bool recLED_ = false;

};
