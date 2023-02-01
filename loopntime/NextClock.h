/***** NextClock.h *****/

// Class for multiplying or dividing a clock, and 
// for predicting samples until the next clock pulse

#pragma once

#include <vector>
#include <algorithm>
#include <libraries/Debounce/Debounce.h>

class NextClock {
    
    public:
        
        // Constructors: the one with arguments automatically calls setup()
        NextClock() {};

        // Setup requires sample rate. Returns true on success.
        bool setup(int sampleRate, int nClockEvents);
        
        // Calculating the median IOI
        int median(std::vector<int> v);
        
        // Process the incoming clock signal
        void process(float in, float multiplier);
        
        // Destructor
        ~NextClock() {};

        // Declare
        int nClockEvents_;                      // Number of clock events for median
        int pulseLength_;                       // Duration of clock pulse out (e.g., LED duration)
        int samplesSinceLastClockInput_ = 0;    
        int samplesSinceLastClockOutput_ = 0;
        int samplesUntilNextClockInput_ = 0;
        int inputClockCounter_ = 0;             
        int startupCounter_ = 0;
        int medianClockInputIOI_ = 0;
        int clockOutIOI_ = 0;
        int writePointerClockInputIOIList_ = 0;
        int pulseCounterOutput_ = 0;
        int pulseCounterInput_ = 0;
        std::vector<int> clockInputIOIList_;
        std::vector<float> multDivValSet_;
        Debounce clockPulseDebounced_;
        int multIdx_ = 0;
        float m_ = 0;
        int pulseInput_ = 0;
        int pulseOutput_ = 0;
        int samplesUntilNextClockOutput_ = 0;
        bool clockReady_ = false;

};