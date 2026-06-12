cmake_minimum_required(VERSION 3.22)
project(OrientalInstrument VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(JUCE)

juce_add_plugin(OrientalInstrument
    COMPANY_NAME "DJBILBOX BEATS"
    PLUGIN_MANUFACTURER_CODE DJBX
    PLUGIN_CODE OrIn
    FORMATS VST3
    PRODUCT_NAME "Oriental Instrument"
    PLUGIN_NAME "Oriental Instrument"
    PLUGIN_DESC "Oriental Virtual Instrument - Maqam Edition"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    VST3_CATEGORIES Instrument Synth
    BUNDLE_ID com.djbilbox.orientalinstrument
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(OrientalInstrument
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/DSP/OrientalSynthesiser.cpp
        Source/DSP/OrientalVoice.cpp
        Source/DSP/SampleEngine.cpp
        Source/DSP/WavetableOscillator.cpp
        Source/DSP/ADSREnvelope.cpp
        Source/DSP/MaqamTuning.cpp
        Source/DSP/FXChain/FXChain.cpp
        Source/DSP/FXChain/ReverbFX.cpp
        Source/DSP/FXChain/DelayFX.cpp
        Source/DSP/FXChain/ChorusFX.cpp
        Source/DSP/FXChain/DistortionFX.cpp
        Source/DSP/FXChain/CompressorFX.cpp
        Source/DSP/FXChain/EQ3BandFX.cpp
        Source/DSP/FXChain/PhaserFX.cpp
        Source/DSP/FXChain/BitcrusherFX.cpp
        Source/Data/InstrumentProfiles.cpp
        Source/Data/PresetManager.cpp
        Source/GUI/OrientalLookAndFeel.cpp
        Source/GUI/KnobComponent.cpp
        Source/GUI/PresetBrowser.cpp
        Source/GUI/PianoComponent.cpp
        Source/GUI/FXPanel.cpp
        Source/GUI/WaveformDisplay.cpp
        Source/GUI/WheelComponent.cpp
        Source/GUI/BackgroundComponent.cpp
)

# Generate JuceHeader.h (every source uses #include <JuceHeader.h>).
# Required with the CMake API — the Projucer-style JuceLibraryCode/ is not wired in.
juce_generate_juce_header(OrientalInstrument)

# Embed the UI fonts (Cinzel / Inter, OFL) → BinaryData::CinzelBold_ttf, etc.
juce_add_binary_data(OrientalBinaryData
    SOURCES
        Resources/Fonts/CinzelBold.ttf
        Resources/Fonts/CinzelRegular.ttf
        Resources/Fonts/InterBold.ttf
        Resources/Fonts/InterRegular.ttf
        Resources/Fonts/UnifrakturCookBold.ttf
        Resources/Images/bg_algerie.png
)

target_include_directories(OrientalInstrument
    PRIVATE
        Source
)

target_compile_definitions(OrientalInstrument
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_STRICT_REFCOUNTEDPOINTER=1
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(OrientalInstrument
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        OrientalBinaryData
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
