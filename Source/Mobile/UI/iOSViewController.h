#pragma once

#ifdef JUCE_IOS
    #include <UIKit/UIKit.h>
    #include <AVFoundation/AVFoundation.h>
#endif

#include <JuceHeader.h>

class OrientalInstrumentProcessor;

/*
    iOSViewController.h — Native iOS controller for touch input + device rotation.

    Handles:
    - Multi-touch gestures (pitch/mod wheels, piano, knob drags)
    - Device orientation (portrait, landscape, responsive layout)
    - Audio session initialization & interruption handling
    - MIDI input from external controllers

    Bridges native iOS APIs with JUCE's audio processor via Objective-C++.
*/

#ifdef JUCE_IOS

@interface OrientalInstrumentiOSViewController : UIViewController

@property (nonatomic, assign) class OrientalInstrumentProcessor* audioProcessor;
@property (nonatomic, strong) AVAudioSession* audioSession;

- (instancetype)initWithProcessor:(class OrientalInstrumentProcessor*)processor;

// Audio session management
- (void)initializeAudioSession;
- (void)handleAudioInterruption:(NSNotification*)notification;

// Touch handling
- (void)handleTouchBegan:(UITouch*)touch withEvent:(UIEvent*)event;
- (void)handleTouchMoved:(UITouch*)touch withEvent:(UIEvent*)event;
- (void)handleTouchEnded:(UITouch*)touch withEvent:(UIEvent*)event;
- (void)handleTouchCancelled:(UITouch*)touch withEvent:(UIEvent*)event;

// Device rotation
- (void)handleDeviceOrientationChange:(NSNotification*)notification;

// MIDI input callbacks
- (void)onMidiNoteOn:(uint8_t)note velocity:(uint8_t)velocity;
- (void)onMidiNoteOff:(uint8_t)note;
- (void)onMidiControlChange:(uint8_t)controller value:(uint8_t)value;
- (void)onMidiPitchBend:(uint16_t)pitchBendValue;

@end

#endif
