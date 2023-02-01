/***** NextClock.cpp *****/

#include "NextClock.h"

// Setup. Returns true on success.
bool NextClock::setup(int sampleRate, int nClockEvents){
    
    // How many clock events for the median calculation
    nClockEvents_ = nClockEvents;
    
    // 50 ms clock pulses, seems good for LED
    pulseLength_ = (int)(sampleRate * .050); 
    
    // Setup the debounce class
    int debounceInterval_ = (int)(.050 * sampleRate);
    clockPulseDebounced_.setup(debounceInterval_);
    
    // Set up list of IOIs and the possible multiples
    clockInputIOIList_.resize(nClockEvents_);
    multDivValSet_ = {.125, 0.25, 0.5, 1, 2, 4, 8};

    return true;
}

// Calculate median of int vector 
int NextClock::median(std::vector<int> v) { 
    
    std::vector<int> vSort = v;
    std::sort(vSort.begin(), vSort.end());
    int median = (int)vSort[vSort.size()/2];
    
    return median; 
} 

// Turn incoming clock pulse to mult/div clock out, and return samples to next event
void NextClock::process(float in, float multiplier){

    // Process clock input

    // An timer of samples since the last clock input
    samplesSinceLastClockInput_ += 1;
    
    // A countdown of samples until the next expected clock input
    samplesUntilNextClockInput_ = medianClockInputIOI_ - samplesSinceLastClockInput_;
    
    // Detect whether the incoming signal is a new clock event
    clockPulseDebounced_.process(in > 0.05); // debounce accepts boolean
    int clockPulseEdge = clockPulseDebounced_.edgeDetected();
         
    // If there is a new event:
    // (1) log the IOI timer
    // (2) recalculate the median IOI
    // (3) reset the IOI timer
    if(clockPulseEdge == 1){
        
        // Tally that a clock input has occurred
        inputClockCounter_ += 1;
        startupCounter_ += 1;

        // Add the most recent IOI to the list
        clockInputIOIList_[writePointerClockInputIOIList_] = samplesSinceLastClockInput_;
        
        // Advance the write pointer and wrap if necessary
        writePointerClockInputIOIList_ += 1;
        if(writePointerClockInputIOIList_ >= clockInputIOIList_.size()){
            writePointerClockInputIOIList_ = 0;
        }
            
        // Calculate the median of the IOI list
        medianClockInputIOI_ = NextClock::median(clockInputIOIList_);
        
        // Reset the last IOI timer
        samplesSinceLastClockInput_ = 0;
    }

    
    
    // Process the multiplier with regard to current input IOI
    
    // Read the current divider/multiplier value
    multIdx_ = (int)(multiplier * 6 + 0.5);
    m_ = multDivValSet_[multIdx_];

    // Multiply/divide the clock out IOI 
    clockOutIOI_ = (int)(medianClockInputIOI_ * m_);
    
    
    
    // Process clock output

    // An count of samples since the next clock output
    samplesSinceLastClockOutput_ += 1;
    
    // If dividing or not changing the IOI, 
    // (1) Wait until the counter reaches output IOI, or 
    // (2) Retrigger if a new clock input arrives earlier than expected
    // Else if multiplying the IOI,
    // (1) Retrigger if a new clock input arrives, ignoring until the multiple is reached
    if (m_ <= 1) {
        
       if (samplesSinceLastClockOutput_ >= clockOutIOI_) {
            // Reset the count if it exceeds the IOI out
            samplesSinceLastClockOutput_ = 0;
       }
       
       if (samplesSinceLastClockInput_ == 0) {
            // Reset the count if a new input has arrived
            samplesSinceLastClockOutput_ = 0;
       }
       
    }
    else {

        if (inputClockCounter_ >= m_) {
            
            // Reset the count if a new input has arrived
            samplesSinceLastClockOutput_ = 0;

            // Reset the count of clock inputs
            inputClockCounter_ = 0;
        }
    }
    
    
    
    // Prepare info to be returned
    
    // For the input
    if (samplesSinceLastClockInput_ == 0) {
        pulseCounterInput_ = pulseLength_;
    }
    
    // Send the clock pulse out if necessary
    pulseInput_ = 0;
    if (--pulseCounterInput_ > 0) {
        pulseInput_ = 1;
    }
    else {
        pulseCounterInput_ = 0;
    }
    
    // For the output
    if (samplesSinceLastClockOutput_ == 0) {
        pulseCounterOutput_ = pulseLength_;
    }
    
    // Send the clock pulse out if necessary
    pulseOutput_ = 0;
    if (--pulseCounterOutput_ > 0) {
        pulseOutput_ = 1;
    }
    else {
        pulseCounterOutput_ = 0;
    }
    
    // Calculate samples until next clock output
    samplesUntilNextClockOutput_ = clockOutIOI_ - samplesSinceLastClockOutput_;
    
    // Send dummy output at startup until enough clock input arrives to calculate median
    if (startupCounter_ < nClockEvents_) {
        pulseInput_ = 0;
        pulseOutput_ = 0;
        samplesUntilNextClockOutput_ = 999999999;
    }
    else {
        startupCounter_ = nClockEvents_; // Hold here once reached
        clockReady_ = true;
    }
    
    return;
}
