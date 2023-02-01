/***** LoopControl.cpp *****/

#include "LoopControl.h"
#include <cstdio>

// Setup
bool LoopControl::setup(int sampleRate, int numLoops, int maxBufferLength, bool autoPlay, bool verbose) {

    numLoops_ = numLoops; 
    for (int i=0; i<numLoops_; i++) {
        
        Loop l;
        loop.push_back(l);
        loop[i].setup(sampleRate, maxBufferLength, autoPlay, verbose);
        
        // Five integers of info per loop
        guiInfo_.push_back(0);
        guiInfo_.push_back(0);
        guiInfo_.push_back(0);
        guiInfo_.push_back(0);
        guiInfo_.push_back(0);
        
    }

    // Set debounce interval for all buttons
    int debounceInterval = .1 * sampleRate;
    debounceB1_.setup(debounceInterval);
    debounceB2_.setup(debounceInterval);
    debounceB3_.setup(debounceInterval);
    debounceB4_.setup(debounceInterval);

    // Set samples per frame for the gui
    spfGUI_ = (1 / fps_) * sampleRate;

    return true;
}

// Select loop
void LoopControl::loopSelector(float loopSelect) {
    whichLoop_ = (int)(loopSelect*numLoops_);
}

// Interface button 1 Rec/Overdub
void LoopControl::button1RecOverdub() {

    if (loop[whichLoop_].verbose_) {std::printf("Loop %i: button1RecOverdub\n", whichLoop_);}

    if (loop[whichLoop_].recState_!=loop[whichLoop_].recStateOn_) {
        loop[whichLoop_].triggerStartRec(); 
    }
    else if (loop[whichLoop_].recState_==loop[whichLoop_].recStateOn_) {
        if (loop[whichLoop_].verbose_) {std::printf("Loop %i: Immediate overdub\n", whichLoop_);}
        loop[whichLoop_].triggerEndRec(); 
        loop[whichLoop_].triggerStartRec(); // Immediate overdub 
    }

}

// Interface button 2 Play/Stop
void LoopControl::button2PlayStop() {
    
    if (loop[whichLoop_].verbose_) {std::printf("Loop %i: button2PlayStop\n", whichLoop_);}

    if (loop[whichLoop_].isPlaying_==false) {
        loop[whichLoop_].triggerStartPlayback(); 
    }
    else {
        if (loop[whichLoop_].recState_==loop[whichLoop_].recStateOn_) {
            loop[whichLoop_].triggerEndRec(); // No immediate overdub but continues playing
        }
        else {
            loop[whichLoop_].triggerEndPlayback();
        }
    }
}

// Interface button 3 Undo
void LoopControl::button3Undo() {

    if (loop[whichLoop_].verbose_) {std::printf("Loop %i: button3Undo\n", whichLoop_);}
    loop[whichLoop_].triggerEmptyBufferSTM();

}

// Interface button 4 Reset
void LoopControl::button4Reset() {

    if (loop[whichLoop_].verbose_) {std::printf("button4Reset\n");}
    loop[whichLoop_].reset();

}

// Debounce and trigger buttons if necessary
void LoopControl::buttons(std::vector<int> buttonDigitalRead) {
    
    // Debounce from digital read
    debounceB1_.process(buttonDigitalRead[0]);
    debounceB2_.process(buttonDigitalRead[1]);
    debounceB3_.process(buttonDigitalRead[2]);
    debounceB4_.process(buttonDigitalRead[3]);

    // Trigger button activities if pressed
    if (debounceB1_.edgeDetected()==1) {LoopControl::button1RecOverdub();}
    if (debounceB2_.edgeDetected()==1) {LoopControl::button2PlayStop();}
    if (debounceB3_.edgeDetected()==1) {LoopControl::button3Undo();}
    if (debounceB4_.edgeDetected()==1) {LoopControl::button4Reset();}
        
}

// Send information to gui at frame rate
void LoopControl::gui() {

    // Send information to the gui once per frame
    // Technically this isn't the entire loop nor the best timing.
    // It begins slightly before the loop (duration of envelope opening)
    // and skips the tail end fade, but has the correct overall duration.
    // Negligible issues if the cross-fade duration is small.
    if (GUI_c_ >= spfGUI_) {
        for (int i = 0; i < numLoops_; i++) {
            guiInfo_[i*5+0] = loop[i].guiMaxBufferLength_;
            guiInfo_[i*5+1] = loop[i].guiLoopLayer1Length_;
            guiInfo_[i*5+2] = loop[i].guiRead1_;
            guiInfo_[i*5+3] = (int)(1000 * loop[i].guiAmpOut_);
            guiInfo_[i*5+4] = (int)(i==whichLoop_);
        }
        GUI_c_ = 0;
    }
    GUI_c_ += 1;

}

// Control recording LEDs (i.e., is *any* loop recording)
void LoopControl::recLED() {
    
    int recTally = 0;
    for (int i=0; i< loop.size(); i++) {recTally += loop[i].isRecording_;}
    recLED_ = recTally > 0;

}

// Process all buttons and loops
float LoopControl::process(float in, float loopSelect, std::vector<int> buttonDigitalRead) {

    // Update information to/from controls
    LoopControl::loopSelector(loopSelect);
    LoopControl::buttons(buttonDigitalRead);
    LoopControl::recLED();

    // Process next sample across all loops
    float out = 0;
    for (int i=0; i<numLoops_; i++) {
        out += loop[i].process(in);
    }

    // Update gui info
    LoopControl::gui();

    return out;
}
