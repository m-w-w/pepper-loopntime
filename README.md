# LoopnTime

LoopnTime is a multi-channel eurorack looper that keeps in time with an analog clock signal. 

![gui gif](https://github.com/m-w-w/pepper-loopntime/blob/main/media/gui.gif)

Some features:
- The number of independent loops and the maximal length of the loops is adjustable. 
- Each loop has one layer of "undo". 
- The rate of the clock input can be divided or multiplied to clock output. 
- Recording and playback are synchronized with the clock output. A button press to record or overdub or stop recording will wait until the next event arrives. 
- Because analog clocks are imperfect, a loop can vary in length by a few samples from one replay to the next. Recording and playback are handled separately, and rely on cross-fading amplitude to smooth transitions and avoid discontinuity.

LoopnTime is implemented as a patch for the [Pepper eurorack module](https://github.com/BelaPlatform/bela-pepper/wiki). With some tweaks it would work in other contexts, e.g., as a guitar multi-looper pedal, or on other platforms. The clock mult/div class can be used separately. The loop class can be used separately as well, if you prefer to time everything manually.

# Quickstart:

- Create a new project and upload the files to the Bela Pepper.
- Under `Settings`, change `block size (audio frames)` to 128.
- Patch an incoming audio signal to `Audio In L`.
- Patch stereo output from `Audio Out L` (loop audio output) and `Audio Out R` (incoming audio unchanged).
- Patch a clock signal to `CV In 0`, ensuring that the `upper left knob` isn't attenuating the signal.
    - The rightmost LED (#10) will blink in time with the incoming clock pulse.
    - The LED two spots to the left (#8) will blink in time with the clock output mult/div rate, which is adjusted with the `right upper knob`:
        - Halfway = no change in rate, less than halfway = multiplying (2x, 4x, 8x), more than halfway = dividing (.5x, .25x, .125x)
- Use the `bottom right knob` to select which loop track is active. In this render file there are 4, and the active track is indicated in the first four LEDs. All button presses apply only to the active loop, but the clock is shared across loops.
    - Tip: If connected to a computer, click on the the Bela GUI to visualize the loops.
- Use the buttons to record/overdub (Button 1), playback/stop (Button 2), undo (Button 3), or clear the loop entirely (Button 4). `Buttons 1 and 2` are triggered at the next clock pulse. `Buttons 3 and 4` are immediate.
    - The first loop sets the loop length, up to some maximum. In this render file it is set to 15 seconds but it could be much longer. If the maximum is reached, the loop will not set and the recorded audio will be cleared.
    - To begin recording, press `Button 1` (record). LEDs #5-7 will light up when recording is active.
    - To set the first loop length and continue recording (overdub), press `Button 1` again. There is a slight attenuation of previous loop audio with each added overdub layer.
    - To set the first loop length and continue playback without recording (play), press `Button 2`. 
    - To end the playback (stop), press `Button 2` while not recording.

# Future features, someday, hopefully

- Trigger to write a loop track, or a mix of all loop tracks, to audio file.  
- Controls to mix/adjust amplitude and pan separately by loop track.
- Adding some type of delay á la Line 6 DL4.
    
# Feedback

If you have any comments or improvements to share, or if something isn't working, please let me know.

# Appreciation

- The [C++ Real-Time Audio Programming with Bela Youtube series](https://www.youtube.com/playlist?list=PLCrgFeG6pwQmdbB6l3ehC8oBBZbatVoz3). 
- The [Bela forum](https://forum.bela.io/).
