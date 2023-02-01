/***** LoopnTime.cpp *****/

#include "LoopnTime.h"

// Set up
bool LoopnTime::setup(int sampleRate, int numLoops, int maxBufferLength, bool verbose) {
    
    // Pass setup parameters to LoopControl setup
    bool autoPlay = false;
    lc.setup(sampleRate, numLoops, maxBufferLength, autoPlay, verbose);

    // Pass setup parameters to NextClock setup
    int nClockEvents = 5;
    nc.setup(sampleRate, nClockEvents);

    // The number of samples in the amplitude envelope ramp up/down
    // All loops share envelopeLength_, use the first one
    envelopeLength_ = lc.loop[0].envelopeLength_;

    // Set debounce interval for all buttons
    int debounceInterval = .1 * sampleRate;
    debounceB1_.setup(debounceInterval);
    debounceB2_.setup(debounceInterval);

    // Variables that depend on number of loops
    for (int i=0; i<numLoops; i++) {
        retriggerPlaybackActivated_.push_back(false);
        retriggerCountdown_.push_back(-1);
    }

    return true;

};

// Process LoopControl and NextClock
float LoopnTime::process(float clockIn, float multDiv, float audioIn, float loopSelect, std::vector<int> buttonDigitalRead) {

    // Create a vector of same length as button inputs
    std::vector<int> buttonDigitalRead_InTime = {0,0,0,0};

    // Debounce buttons 1 and 2 from digital read
    debounceB1_.process(buttonDigitalRead[0]);
    debounceB2_.process(buttonDigitalRead[1]);
    
    // Process retriggering of loops when playback reaches fade
    LoopnTime::processLoopEnding();

    // Trigger buttons if necessary
    if (debounceB1_.edgeDetected()==1) {LoopnTime::button1RecOverdub();}    // delayed
    if (debounceB2_.edgeDetected()==1) {LoopnTime::button2PlayStop();}      // delayed
    if (buttonDigitalRead[2]==1) {buttonDigitalRead_InTime[2]=1;}           // instantaneous
    if (buttonDigitalRead[3]==1) {buttonDigitalRead_InTime[3]=1;}           // instantaneous

    // Update which loop is accepting commands
    lc.loopSelector(loopSelect);

    // Process clock information
    nc.process(clockIn, multDiv);

    // If retrigger playback is activated, countdown; trigger playback when countdown ended
    for (int i=0; i<lc.numLoops_; i++) {
        if (retriggerPlaybackActivated_[i] == true) {
            retriggerCountdown_[i] -= 1;
            if (nc.samplesUntilNextClockOutput_ <= envelopeLength_) {
                lc.loop[i].triggerStartPlayback(); 
                retriggerPlaybackActivated_[i] = false;
                retriggerCountdown_[i] = -1;
            }
        }
    }

    // If B1 toggled on, countdown; trigger when countdown ended
    if (button1Activated_) {
        button1Countdown_ -= 1;
        if (button1Countdown_ <= 0) {
            buttonDigitalRead_InTime[0]=1;
            button1Activated_ = false;
            button1Countdown_ = -1;
        }
    }

    // If B2 toggled on, countdown; trigger when countdown ended
    if (button2Activated_) {
        button2Countdown_ -= 1;
        if (button2Countdown_ <= 0) {
            buttonDigitalRead_InTime[1]=1;
            button2Activated_ = false;
            button2Countdown_ = -1;
        }
    }

    // Process loop(s)
    // Note: buttons are debounced again within LoopControl
    float out = lc.process(audioIn, loopSelect, buttonDigitalRead_InTime);

    return out;

};

// Process automatic retriggering of loops when playback reaches fade
void LoopnTime::processLoopEnding() {
    int halfClockOutIOI = nc.clockOutIOI_ / 2;
    for (int i=0; i<lc.numLoops_; i++) {
        if (nc.clockReady_) {
            samplesUntilLoopEnd_ = lc.loop[i].getSamplesUntilRetrigger();
            if ((retriggerPlaybackActivated_[i] == false) & (lc.loop[i].isPlaying_ == true) & (samplesUntilLoopEnd_ <= halfClockOutIOI)) {
                retriggerPlaybackActivated_[i] = true;
                retriggerCountdown_[i] = nc.samplesUntilNextClockOutput_ - envelopeLength_;
                if (retriggerCountdown_[i] < envelopeLength_) {
                    retriggerCountdown_[i] += nc.clockOutIOI_; // Should happen rarely, unless IOI changes drastically
                }
            }
        }
    }
};

// Interface button 1 Rec/Overdub
void LoopnTime::button1RecOverdub() {
    
    // Toggle button 1
    if (nc.clockReady_) {
        button1Activated_ = !button1Activated_;
    }

    // Update counter
    if (button1Activated_) {
        button1Countdown_ = nc.samplesUntilNextClockOutput_ - envelopeLength_;
        if (button1Countdown_ < envelopeLength_) {
            button1Countdown_ += nc.clockOutIOI_;
        }
    }
    else {
        button1Countdown_ = -1;
    }

};

// Interface button 2 Play/Stop
void LoopnTime::button2PlayStop() {

    // Toggle button 2
    if (nc.clockReady_) {
        button2Activated_ = !button2Activated_;
    }

    // Update counter
    if (button2Activated_) {
        button2Countdown_ = nc.samplesUntilNextClockOutput_ - envelopeLength_;
        if (button2Countdown_ < envelopeLength_) {
            button2Countdown_ += nc.clockOutIOI_;
        }
    }
    else {
        button2Countdown_ = -1;
    }

};
