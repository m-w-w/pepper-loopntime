/***** Loop.cpp *****/

#include "Loop.h"
#include <cstdio>
#include <math.h> /* fabs, round */

// Setup
bool Loop::setup(int sampleRate, int maxBufferLength, bool autoPlay, bool verbose){

    // Vars to set once
    sampleRate_ = sampleRate;
    autoPlay_ = autoPlay;
    verbose_ = verbose;
    maxBufferLength_ = maxBufferLength * sampleRate;
    envelopeLength_ = (int)(sampleRate * 0.05);
    maxMainLoopMaxIdx_ = maxBufferLength_ - envelopeLength_ - 1;
    bufferSTM_.resize(maxBufferLength_);
    bufferLTM_.resize(maxBufferLength_);
    spf_ = (int)(sampleRate_/30) - 1; // Gui samples/frame assuming 30 frames/sec

    // Vars to be reset
    mainLoopMaxIdx_ = maxMainLoopMaxIdx_ + 1;
    extendedLoopMaxIdx_ = mainLoopMaxIdx_ + 1;
    
    return true;
}


// Reset
bool Loop::reset(){

    if (verbose_) {std::printf("reset\n");}

    Loop::triggerEmptyBufferSTM();

    mainLoopMaxIdx_ = maxMainLoopMaxIdx_ + 1;
    extendedLoopMaxIdx_ = mainLoopMaxIdx_ + 1;
    outSTM1_ = 0;
    outSTM2_ = 0;
    outLTM1_ = 0;
    outLTM2_ = 0;
    out1_ = 0;
    out2_ = 0;
    read1_ = -1;
    read2_ = -1;
    readAmp1_ = -1;
    readAmp2_ = -1;
    readAmpO_c_ = -1;
    readAmpC_c_ = -1;
    rec1_ = -1;
    rec2_ = -1;
    recAmp1_ = -1;
    recAmp2_ = -1;
    recAmpO_c_ = -1;
    recAmpC_c_ = -1;
    recAmp_c_ = -1;
    transfer1_ = -1;
    transfer2_ = -1;
    transfer_c_ = -1; 
    emptySTM1_ = -1; 
    emptySTM2_ = -1; 
    emptySTM_c_ = -1;        
    emptyBufferSTM_ = true;  
    emptyBufferLTM_ = true; 
    loopCount_ = 0;
    recEnding_c_ = -1;
    recState_ = 0; 
    isPlaying_ = false;
    isRecording_ = false;
    guiAmpIn_ = 0;
    guiAmpOut_ = 0;
    guiAmp_c_ = -1;
    guiMaxBufferLength_ = 0;
    guiLoopLayer1Length_ = 0;
    guiRead1_ = 0;
    
    return true;
}


// Set the first loop layer length
bool Loop::setLoopLayer1Length(int x) {

    if (verbose_) {std::printf("setLoopLayer1Length\n");}

    mainLoopMaxIdx_ = x;
    extendedLoopMaxIdx_ = mainLoopMaxIdx_ + envelopeLength_;

    Loop::triggerStartPlayback();

    return true;
}


// Get the first loop layer length
int Loop::getLoopLayer1Length() {

    int x;
    if (loopCount_ == 0) {
        x = 0;
    }
    else if (extendedLoopMaxIdx_ > maxMainLoopMaxIdx_) {
        // Before the loop length is known, extendedLoopMaxIdx_ is greater
        // than the maximum possible loop length 
        x = read1_; 
    }
    else {
        x = mainLoopMaxIdx_;
    }

    return x;
}

// Get the samples until a retrigger should happen
int Loop::getSamplesUntilRetrigger() {

    int x;
    if (extendedLoopMaxIdx_ > maxMainLoopMaxIdx_) {
        // Before the loop length is known, extendedLoopMaxIdx_ is greater
        // than the maximum possible loop length 
        x = maxMainLoopMaxIdx_; // send maximal value
    }
    else {
        x = mainLoopMaxIdx_ - read1_;
    }

    return x;
}


// Trigger the start of recording
bool Loop::triggerStartRec() {

    if (verbose_) {std::printf("triggerStartRec\n");}

    loopCount_ += 1;
    emptyBufferSTM_ = false;
    if (isPlaying_ == false) {Loop::triggerStartPlayback();};
    if (loopCount_ == 1) {Loop::triggerStartPlayback();}  
    if (recState_ == recStateEnding_) {rec2_ = rec1_;} // immediate overdub
    recState_ = recStateOn_;
    rec1_ = read1_;
    if (loopCount_ > 1) {recAmpO_c_ = envelopeLength_;} 
    if (loopCount_ > 1) {Loop::triggerBufferTransfer();}

    return true;
}


// Trigger the end of recording
bool Loop::triggerEndRec() {

    if (verbose_) {std::printf("triggerEndRec\n");}

    if (loopCount_ == 1) {Loop::setLoopLayer1Length(rec1_);}
    if (loopCount_ > 1) {recAmpC_c_ = envelopeLength_;} 
    if (loopCount_ >= 1) {
        recState_ = recStateEnding_;
        recEnding_c_ = envelopeLength_;
    }
    emptyBufferSTM_ = false;

    return true;
}


// Trigger the start of playback
bool Loop::triggerStartPlayback() {

    if (verbose_) {std::printf("triggerStartPlayback\n");}
    isPlaying_ = true;
    if (read1_ != -1) { 
        if (readAmpC_c_ == -1) {
            read2_ = read1_;
            readAmpC_c_ = envelopeLength_;
        }
    }
    read1_ = -1;
    readAmpO_c_ = envelopeLength_;

    return true;
}


// Trigger the end of playback
bool Loop::triggerEndPlayback() {

    if (verbose_) {std::printf("triggerEndPlayback\n");}

    isPlaying_ = false;
    
    return true;
}


// Trigger the transfer from stm buffer to ltm buffer
bool Loop::triggerBufferTransfer() {

    if (verbose_) {std::printf("triggerBufferTransfer\n");}

    // Only transfer if STM hasn't been cleared out / has data in it
    if (emptyBufferSTM_ == false) {
        emptyBufferLTM_ = false;
        transfer_c_ = mainLoopMaxIdx_ + 1;
        transfer1_ = read1_;
        transfer2_ = read2_;
    }

    return true;
}


// Trigger the clearing of the STM buffer ("undo")
bool Loop::triggerEmptyBufferSTM() {

    if (verbose_) {std::printf("triggerEmptyBufferSTM\n");}

    if (emptyBufferSTM_ == false) {
        
        // Set the buffer status to empty
        emptyBufferSTM_ = true;
    
        // Set the idxs/counter to clear the buffer
        emptySTM_c_ = mainLoopMaxIdx_ + 1;
        emptySTM1_ = read1_;
        emptySTM2_ = read2_;
    
        // Reduce the loop count by 1
        loopCount_ -= 1;
        if (loopCount_ < 1) {Loop::reset();}

    }

    return true;
}


// Process the amplitude envelopes
bool Loop::amp() {

    // Recording to the buffer - main amp env that opens and stays open
    if (recState_ == recStateOn_) { 
        if (recAmpO_c_ >= 0) {
            recAmp1_ = 1 - static_cast<float>(recAmpO_c_)/static_cast<float>(envelopeLength_);
        } 
        else {
            recAmp1_ = 1;
        }
    } 
    else if (recState_ == recStateEnding_) {
        if (recAmpO_c_ >= 0) {
            recAmp1_ = 1 - static_cast<float>(recAmpO_c_)/static_cast<float>(envelopeLength_);
        } 
        else {
            recAmp1_ = 0;
        }
    }
    else {
        recAmp1_ = 0;
    }

    // Recording to the buffer - fader amp env that closes and stays closed
    if (recState_ != recStateOff_) {
        if (recAmpC_c_ >= 0) {
            recAmp2_ = static_cast<float>(recAmpC_c_)/static_cast<float>(envelopeLength_);
        } 
        else {
            recAmp2_ = 1;
        }
    } 
    else {
        recAmp2_ = 0;
    }

    // Playing from the buffer - main amp env that opens and stays open
    if (read1_ >= 0) {
        if (readAmpO_c_ >= 0) {
            readAmp1_ = 1 - static_cast<float>(readAmpO_c_)/static_cast<float>(envelopeLength_);
        } 
        else {
            readAmp1_ = 1;
        }
    } 
    else {
        readAmp1_ = 0;
    }

    // Playing from the buffer - fader amp env that closes and stays closed
    if (read2_ >= 0) {
        if (readAmpC_c_ >= 0) {
            readAmp2_ = static_cast<float>(readAmpC_c_)/static_cast<float>(envelopeLength_);
        } 
        else {
            readAmp2_ = 0;
        }
    } 
    else {
        readAmp2_ = 0;
    }

    return true;
}


// Write to the loop buffer
bool Loop::bufferWrite(float in){

    float in1 = 0;
    float in2 = 0;
    float useAmp = 0;
    float prevValLTM1 = 0;
    float prevValLTM2 = 0;
    float prevValAmp = .95; // previous loops can be dampened
    
    // Erase the STM buffer if necessary
    if (emptySTM1_ != -1) {bufferSTM_[emptySTM1_] = 0;} // pre reset
    if (emptySTM2_ != -1) {bufferSTM_[emptySTM2_] = 0;} // pre reset
    if (rec1_ != -1) {if (loopCount_ == 1) {bufferSTM_[rec1_] = 0;}} // post reset
    if (rec2_ != -1) {if (loopCount_ == 1) {bufferSTM_[rec2_] = 0;}} // post reset
    
    // Erase the LTM buffer if all layers have been reset/erased
    if (rec1_ != -1) {if (emptyBufferLTM_ == true) {bufferLTM_[rec1_] = 0;}}
    if (rec2_ != -1) {if (emptyBufferLTM_ == true) {bufferLTM_[rec2_] = 0;}}
        
    // Transfer from the STM to the LTM buffer if necessary, and zero out STM
    if (transfer1_ != -1) {
        prevValLTM1 = bufferLTM_[transfer1_];
        bufferLTM_[transfer1_] = prevValLTM1 * prevValAmp + bufferSTM_[transfer1_];
        bufferSTM_[transfer1_] = 0;
    }
    
    if (transfer2_ != -1) { 
        prevValLTM2 = bufferLTM_[transfer2_];
        bufferLTM_[transfer2_] = prevValLTM2 * prevValAmp + bufferSTM_[transfer2_];
        bufferSTM_[transfer2_] = 0;
    }

    // Record to the STM buffer if necessary 
    if (rec1_ != -1) {
        if (recState_ == recStateOn_){
            useAmp = recAmp1_;
        }
        else {
            useAmp = recAmp2_;
        }
        in1 = in * useAmp;
        bufferSTM_[rec1_] = bufferSTM_[rec1_] * prevValAmp + in1;
    }

    if (rec2_ != -1) { 
        in2 = in * recAmp2_;
        // If a new layer/transfer has just started (immediate overdub),
        // then write to LTM as this section is part of the previous loop
        // Else, it is part of the current loop count layer in STM
        bool transferStarting = (mainLoopMaxIdx_ + 1 - transfer_c_) < envelopeLength_; 
        if (transfer_c_ != -1 && transferStarting) {
            prevValLTM2 = bufferLTM_[rec2_];
            bufferLTM_[rec2_] = prevValLTM2 * prevValAmp + in2; // Writes to **LTM**
        }
        else {
            bufferSTM_[rec2_] = bufferSTM_[rec2_] * prevValAmp + in2; // Writes to **STM** 
        }
    }

    return true;
}


// Read from the loop buffer
float Loop::bufferRead() {

    outSTM1_ = 0;
    outSTM2_ = 0;
    outLTM1_ = 0;
    outLTM2_ = 0;
    out1_ = 0;
    out2_ = 0;
    
    if (read1_ != -1) { 
        outLTM1_ = bufferLTM_[read1_];
        outSTM1_ = bufferSTM_[read1_];
        out1_ = (outLTM1_ + outSTM1_) * readAmp1_;
    }
    
    if (read2_ != -1) {
        outLTM2_ = bufferLTM_[read2_];
        outSTM2_ = bufferSTM_[read2_];
        out2_ = (outLTM2_ + outSTM2_) * readAmp2_;
    }

    float out = out1_ + out2_;
    
    return out;
}


// Indices for writing and reading
bool Loop::counter() {

    // Reaching the max buffer length will fail (no useful loop) 
    if (rec1_ >= maxMainLoopMaxIdx_) {Loop::reset();}

    // If recording idxs exceed the layer 1 loop length
    if (rec1_ >= mainLoopMaxIdx_) { 
        rec2_ = rec1_;
        rec1_ = -1;
        if (loopCount_ > 1) {recAmpO_c_ = envelopeLength_;}
    }
    if (rec2_ >= extendedLoopMaxIdx_) {rec2_ = -1;} 

    // If read idxs exceed the layer 1 loop length
    if (read1_ >= mainLoopMaxIdx_) { 
        read2_ = read1_;
        read1_ = -1;
        readAmpC_c_ = envelopeLength_;
        isPlaying_ = false; 
        if (autoPlay_==true) {
            if (verbose_) {std::printf("autoplay: ");}
            Loop::triggerStartPlayback();
        }
    }
    if (read2_ >= extendedLoopMaxIdx_) {read2_ = -1;}

    // If transfer idxs exceed the layer 1 loop length
    if (transfer1_ >= mainLoopMaxIdx_) { 
        transfer2_ = transfer1_;
        transfer1_ = -1;
    }
    if (transfer2_ >= extendedLoopMaxIdx_) {transfer2_ = -1;} 

    // If emptySTM idxs exceed the layer 1 loop length
    if (emptySTM1_ >= mainLoopMaxIdx_) { 
        emptySTM2_ = emptySTM1_;
        emptySTM1_ = -1;
    }
    if (emptySTM2_ >= extendedLoopMaxIdx_) {emptySTM2_ = -1;} 
      
    // Update the recording state/status if necessary
    if (recEnding_c_ > 0) {  
        recEnding_c_ -= 1;
    }
    else { 
        recEnding_c_ -= 1;
        if (recState_ == recStateEnding_) {recState_ = recStateOff_;}
    }
    if (recState_ > 0) {
        isRecording_ = true;
    }
    else {
        isRecording_ = false;
    }

    // Advance idxs
    if (recState_ != recStateOff_) {rec1_ += 1;}
    if (recState_ == recStateOff_) {rec1_ = -1;}
    if (rec2_ != -1) {rec2_ += 1;}  

    if (isPlaying_) {read1_ += 1;}           
    if (!isPlaying_) {read1_ = -1;}       
    if (read2_ != -1) {read2_ += 1;}

    if (transfer_c_ != -1) {transfer1_ += 1;}
    if (transfer2_ != -1) {transfer2_ += 1;}

    if (emptySTM_c_ != -1) {emptySTM1_ += 1;}
    if (emptySTM2_ != -1) {emptySTM2_ += 1;}

    // Stop or hold transfer, emptySTM, emptyLTM counters if countdown ends
    if (transfer_c_ != -1) {transfer_c_ -= 1;}
    if (transfer_c_ <= -1) {
        transfer_c_ = -1;
        transfer1_ = -1;
        transfer2_ = -1;
    }
    if (emptySTM_c_ != -1) {emptySTM_c_ -= 1;}
    if (emptySTM_c_ <= -1) {
        emptySTM_c_ = -1;
        emptySTM1_ = -1;
        emptySTM2_ = -1;
    }

    // Count down with the amp opening/closing counters or hold at -1
    if (recAmpO_c_ != -1) {recAmpO_c_ -= 1;} // the amp opening counter
    if (recAmpO_c_ < -1) {recAmpO_c_ = -1;} 
    if (recAmpC_c_ != -1) {recAmpC_c_ -= 1;} // the amp closing counter
    if (recAmpC_c_ < -1) {recAmpC_c_ = -1;} 
    if (readAmpO_c_ != -1) {readAmpO_c_ -= 1;} // the amp opening counter
    if (readAmpO_c_ < -1) {readAmpO_c_ = -1;} 
    if (readAmpC_c_ != -1) {readAmpC_c_ -= 1;} // the amp closing counter
    if (readAmpC_c_ < -1) {readAmpC_c_ = -1;}
        
    // Exception: if there is no loop, overwrite the read idxs
    if (loopCount_ == 0) {
        read1_ = -1;
        read2_ = -1;
        isPlaying_ = false;
    }

    // Exception: if loop 1 is ending without an immediate overdub, turn off rec1
    if (loopCount_ == 1) {
        if (recState_ == recStateEnding_) {rec1_ = -1;}
    }

    return true;
}


// Process everything
float Loop::process(float in) {

    // Advance read and/or write counters
    Loop::counter();

    // Set the amplitude envelopes
    Loop::amp();

    // Write to the buffer
    Loop::bufferWrite(in);

    // Read from the buffer
    float out = Loop::bufferRead();

    // Update the gui information
    Loop::guiInfo(out); 

    return out;
}

// Update the gui information
bool Loop::guiInfo(float in) {

    guiAmp_c_ += 1;

    if (guiAmp_c_ <= spf_) {
        // Accumulate
        guiAmpIn_ += fabs(in);
    } 
    if (guiAmp_c_ > spf_) {
        // Update and reset
        guiAmpOut_ = guiAmpIn_/spf_;
        guiAmpIn_ = 0;
        guiAmp_c_ = 0;
    }

    guiMaxBufferLength_ = round(maxBufferLength_/spf_);
    guiLoopLayer1Length_ = round(Loop::getLoopLayer1Length()/spf_);
    guiRead1_ = round(read1_/spf_);

    return true;
} 
