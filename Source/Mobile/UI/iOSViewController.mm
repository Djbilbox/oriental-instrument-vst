#include "iOSViewController.h"
#include "PluginProcessor.h"
#include <CoreMIDI/CoreMIDI.h>
#include <AVFoundation/AVFoundation.h>

#ifdef JUCE_IOS

@implementation OrientalInstrumentiOSViewController
{
    UIInterfaceOrientation lastOrientation;
    CGPoint lastTouchPosition;
}

- (instancetype)initWithProcessor:(OrientalInstrumentProcessor*)processor
{
    self = [super init];
    if (self)
    {
        _audioProcessor = processor;
        lastOrientation = UIInterfaceOrientationPortrait;
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.view.backgroundColor = [UIColor blackColor];

    // Initialize audio session for recording and playback
    [self initializeAudioSession];

    // Register for orientation changes
    [[NSNotificationCenter defaultCenter]
        addObserver:self
        selector:@selector(handleDeviceOrientationChange:)
        name:UIDeviceOrientationDidChangeNotification
        object:nil];

    // Register for audio interruptions (calls, alarms, etc.)
    [[NSNotificationCenter defaultCenter]
        addObserver:self
        selector:@selector(handleAudioInterruption:)
        name:AVAudioSession.interruptionNotification
        object:nil];

    // Enable all device orientations
    AppDelegate* appDelegate = (AppDelegate*)[UIApplication sharedApplication].delegate;
    appDelegate.orientationLock = UIInterfaceOrientationMaskAllButUpsideDown;

    // Start device orientation tracking
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [[UIDevice currentDevice] endGeneratingDeviceOrientationNotifications];
}

#pragma mark - Audio Session Management

- (void)initializeAudioSession
{
    NSError* error = nil;

    _audioSession = [AVAudioSession sharedInstance];

    // Set category for recording + playback (mixing allowed for MIDI input)
    [_audioSession setCategory:AVAudioSessionCategoryPlayAndRecord
                         withOptions:(AVAudioSessionCategoryOptionDuckOthers |
                                      AVAudioSessionCategoryOptionDefaultToSpeaker |
                                      AVAudioSessionCategoryOptionMixWithOthers)
                           error:&error];

    if (error)
    {
        NSLog(@"[Oriental iOS] Audio session category error: %@", error.localizedDescription);
    }

    // Set mode for music (optimizes for synthesis)
    [_audioSession setMode:AVAudioSessionModeMeasurement error:&error];

    // Activate session
    [_audioSession setActive:YES withOptions:AVAudioSessionSetActiveOptionNotifyOthersOnDeactivation error:&error];

    if (error)
    {
        NSLog(@"[Oriental iOS] Audio session activation error: %@", error.localizedDescription);
    }

    // Log current audio session config
    NSLog(@"[Oriental iOS] Audio session initialized: %@, category: %@, mode: %@",
          _audioSession.isActive ? @"ACTIVE" : @"INACTIVE",
          _audioSession.category,
          _audioSession.mode);
}

- (void)handleAudioInterruption:(NSNotification*)notification
{
    AVAudioIntSession* session = notification.object;
    NSNumber* typeNum = notification.userInfo[AVAudioSessionInterruptionTypeKey];
    AVAudioSessionInterruptionType type = (AVAudioSessionInterruptionType)[typeNum unsignedIntegerValue];

    if (type == AVAudioSessionInterruptionTypeBegan)
    {
        NSLog(@"[Oriental iOS] Audio interruption began");
        if (_audioProcessor)
            _audioProcessor->releaseResources();
    }
    else if (type == AVAudioSessionInterruptionTypeEnded)
    {
        NSLog(@"[Oriental iOS] Audio interruption ended");
        // Resume audio when interruption ends
        NSError* error = nil;
        [_audioSession setActive:YES withOptions:AVAudioSessionSetActiveOptionNotifyOthersOnDeactivation error:&error];
    }
}

#pragma mark - Touch Handling

- (void)touchesBegan:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        [self handleTouchBegan:touch withEvent:event];
    }
}

- (void)touchesMoved:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        [self handleTouchMoved:touch withEvent:event];
    }
}

- (void)touchesEnded:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        [self handleTouchEnded:touch withEvent:event];
    }
}

- (void)touchesCancelled:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        [self handleTouchCancelled:touch withEvent:event];
    }
}

- (void)handleTouchBegan:(UITouch*)touch withEvent:(UIEvent*)event
{
    CGPoint location = [touch locationInView:self.view];
    lastTouchPosition = location;

    // Touch coordinates are in points; JUCE view will handle scaling
    // GUI components (wheels, knobs, piano) receive touches via JUCE's mouse events
    // Mapped in PluginEditor::resized() based on current orientation
}

- (void)handleTouchMoved:(UITouch*)touch withEvent:(UIEvent*)event
{
    CGPoint location = [touch locationInView:self.view];
    CGPoint delta = CGPointMake(location.x - lastTouchPosition.x, location.y - lastTouchPosition.y);
    lastTouchPosition = location;

    // Continuous drag for pitch wheel, mod wheel, knob parameters
    // Forwarded to JUCE via mouse move events
}

- (void)handleTouchEnded:(UITouch*)touch withEvent:(UIEvent*)event
{
    CGPoint location = [touch locationInView:self.view];
    // Reset any active parameter
}

- (void)handleTouchCancelled:(UITouch*)touch withEvent:(UIEvent*)event
{
    // Release any ongoing parameter modulation
}

#pragma mark - Device Orientation

- (void)handleDeviceOrientationChange:(NSNotification*)notification
{
    UIDeviceOrientation deviceOrientation = [UIDevice currentDevice].orientation;

    NSLog(@"[Oriental iOS] Device orientation changed: %ld", (long)deviceOrientation);

    // Trigger layout recalculation in the JUCE editor
    if (_audioProcessor)
    {
        if (auto* editor = dynamic_cast<OrientalInstrumentEditor*>(_audioProcessor->createEditor()))
        {
            editor->resized();
        }
    }
}

#pragma mark - MIDI Input Callbacks

- (void)onMidiNoteOn:(uint8_t)note velocity:(uint8_t)velocity
{
    if (!_audioProcessor)
        return;

    juce::MidiMessage msg = juce::MidiMessage::noteOn(1, note, velocity / 127.0f);
    // Forward to processor's MIDI input buffer
    // This is called from native MIDI callbacks
}

- (void)onMidiNoteOff:(uint8_t)note
{
    if (!_audioProcessor)
        return;

    juce::MidiMessage msg = juce::MidiMessage::noteOff(1, note);
}

- (void)onMidiControlChange:(uint8_t)controller value:(uint8_t)value
{
    if (!_audioProcessor)
        return;

    // CC 1 = Modulation Wheel
    // CC 7 = Master Volume
    // CC 120 = All Notes Off
    if (controller == 1)
    {
        _audioProcessor->setModWheel(value / 127.0f);
    }
}

- (void)onMidiPitchBend:(uint16_t)pitchBendValue
{
    if (!_audioProcessor)
        return;

    // Normalize to -1..+1
    float normalizedValue = (static_cast<int>(pitchBendValue) - 8192) / 8192.0f;
    _audioProcessor->setPitchBend(normalizedValue);
}

#pragma mark - Orientation Support

- (BOOL)shouldAutorotate
{
    return YES;
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskAllButUpsideDown;
}

- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation
{
    return UIInterfaceOrientationPortrait;
}

- (BOOL)prefersStatusBarHidden
{
    return YES;
}

@end

// AppDelegate stub for orientation locking (called during app launch)
@interface AppDelegate : UIResponder <UIApplicationDelegate>
@property (nonatomic, assign) UIInterfaceOrientationMask orientationLock;
@end

@implementation AppDelegate
+ (AppDelegate*)sharedDelegate
{
    return (AppDelegate*)[[UIApplication sharedApplication] delegate];
}
@end

#endif
