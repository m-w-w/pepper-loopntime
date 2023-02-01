/***** LoopnTime.h *****/

// looper synced to a clock input

#pragma once

#include <LoopControl.h>
#include <NextClock.h>
#include <libraries/Debounce/Debounce.h>

class LoopnTime {

    public:
    
        // Constructor
        LoopnTime() {};

        // Set up
        bool setup(int sampleRate, int numLoops, int maxBufferLength, bool verbose);

        // Process LoopControl and NextClock
        float process(float clockIn, float multDiv, float audioIn, float loopSelect, std::vector<int> buttonDigitalRead);

        // Process retriggering of loops when playback reaches fade
        void processLoopEnding();

        // Interface button 1 Rec/Overdub
        void button1RecOverdub();

        // Interface button 2 Play/Stop
        void button2PlayStop();

        // Destructor
        ~LoopnTime() {};
    
        // Declare
        LoopControl lc;
        NextClock nc;
        Debounce debounceB1_;
        Debounce debounceB2_;
        int button1Countdown_ = -1;
        bool button1Activated_ = false;
        int button2Countdown_ = -1;
        bool button2Activated_ = false;
        int envelopeLength_ = 0;
        int samplesUntilLoopEnd_ = -1;
        std::vector<bool> retriggerPlaybackActivated_;
        std::vector<int> retriggerCountdown_;

    private:

};
