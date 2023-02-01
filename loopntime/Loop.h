/***** Loop.h *****/

// Class for a loop

#pragma once
#include <vector>

class Loop {
    
    public:
        
        // Constructors: the one with arguments automatically calls setup()
        Loop() {}

        // Setup requires sample rate. Returns true on success.
        bool setup(int sampleRate, int maxBufferLength, bool autoPlay, bool verbose);
        
        // Reset the buffer
        bool reset();

        // Set the first loop layer length
        bool setLoopLayer1Length(int mainloop_maxidx);

        // Get the first loop layer length
        int getLoopLayer1Length();

        // Get the samples until a retrigger should happen
        int getSamplesUntilRetrigger();

        // Trigger the start of recording
        bool triggerStartRec();

        // Trigger the end of recording
        bool triggerEndRec();

        // Trigger the transfer from stm buffer to ltm buffer
        bool triggerBufferTransfer();

        // Trigger the clearing of the STM buffer ("undo")
        bool triggerEmptyBufferSTM();

        // Trigger the start of playback
        bool triggerStartPlayback(); 

        // Trigger the end of playback
        bool triggerEndPlayback();
        
        // Process the amplitude envelopes
        bool amp();

        // Write to the loop buffer
        bool bufferWrite(float in);

        // Read from the loop buffer
        float bufferRead();

        // Indices for writing and reading
        bool counter();

        // Process
        float process(float in);

        // Update the gui information
        bool guiInfo(float in);

        // Destructor
        ~Loop() {}

        // Declare
        int sampleRate_;
        bool autoPlay_;
        bool verbose_;
        int maxBufferLength_;
        int envelopeLength_;
        int maxMainLoopMaxIdx_;
        int mainLoopMaxIdx_; 
        int extendedLoopMaxIdx_;
        std::vector<float> bufferSTM_;
        std::vector<float> bufferLTM_;
        float outSTM1_ = 0;
        float outSTM2_ = 0;
        float outLTM1_ = 0;
        float outLTM2_ = 0;
        float out1_ = 0;
        float out2_ = 0;
        int read1_ = -1;
        int read2_ = -1;
        float readAmp1_ = -1;
        float readAmp2_ = -1;
        int readAmpO_c_ = -1;
        int readAmpC_c_ = -1;
        int rec1_ = -1;
        int rec2_ = -1;
        float recAmp1_ = -1;
        float recAmp2_ = -1;
        int recAmpO_c_ = -1;
        int recAmpC_c_ = -1;
        int recAmp_c_ = -1;
        int transfer1_ = -1;
        int transfer2_ = -1;
        int transfer_c_ = -1; 
        int emptySTM1_ = -1;
        int emptySTM2_ = -1;
        int emptySTM_c_ = -1;        
        bool emptyBufferSTM_ = true;        
        bool emptyBufferLTM_ = true; 
        int loopCount_ = 0;
        int recEnding_c_ = -1;
        int recState_ = 0; 
        bool isPlaying_ = false;
        bool isRecording_ = false;
        enum {recStateOff_ = 0, recStateOn_, recStateEnding_};    
        int spf_ = 0;
        float guiAmpIn_ = 0;
        float guiAmpOut_ = 0;
        int guiAmp_c_ = -1;
        int guiMaxBufferLength_ = 0;
        int guiLoopLayer1Length_ = 0;
        int guiRead1_ = 0;
};
