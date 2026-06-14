cmake_minimum_required(VERSION 3.22)
project(OrientalInstrument VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(JUCE)

# Embed the UI fonts (Cinzel / Inter, OFL) + background photo → BinaryData::*.
# Shared by both the full and DEMO plugin targets.
juce_add_binary_data(OrientalBinaryData
    SOURCES
        Resources/Fonts/CinzelBold.ttf
        Resources/Fonts/CinzelRegular.ttf
        Resources/Fonts/InterBold.ttf
        Resources/Fonts/InterRegular.ttf
        Resources/Fonts/UnifrakturCookBold.ttf
        Resources/Images/bg_algerie.png
)

set(ORIENTAL_SOURCES
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

# Mobile-specific shared sources (DSP + abstractions)
set(ORIENTAL_MOBILE_SOURCES
    Source/Mobile/MobileAudioContext.cpp
    Source/Mobile/UI/MobileUIController.cpp
    Source/Mobile/UI/MobileKeyboard.cpp
    Source/Mobile/UI/MobileKnob.cpp
    Source/Mobile/UI/MobilePresetBrowser.cpp
    Source/Mobile/UI/FreemiumIndicator.cpp
    Source/Mobile/IAP/iOSIAPManager.cpp
    Source/Mobile/IAP/AndroidIAPManager.cpp
)

# Build one plugin target (full or DEMO). DEMO defines ORIENTAL_DEMO=1 which
# limits PresetManager to 5 presets per instrument.
function(add_oriental_target tgt product code is_demo)
    juce_add_plugin(${tgt}
        COMPANY_NAME "DJBILBOX BEATS"
        PLUGIN_MANUFACTURER_CODE DJBX
        PLUGIN_CODE ${code}
        FORMATS VST3
        PRODUCT_NAME ${product}
        PLUGIN_NAME ${product}
        PLUGIN_DESC "Oriental Virtual Instrument - Maqam Edition"
        IS_SYNTH TRUE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE
        VST3_CATEGORIES Instrument Synth
        BUNDLE_ID com.djbilbox.${tgt}
        COPY_PLUGIN_AFTER_BUILD FALSE
    )

    target_sources(${tgt} PRIVATE ${ORIENTAL_SOURCES})
    juce_generate_juce_header(${tgt})
    target_include_directories(${tgt} PRIVATE Source)

    target_compile_definitions(${tgt}
        PUBLIC
            JUCE_VST3_CAN_REPLACE_VST2=0
            JUCE_STRICT_REFCOUNTEDPOINTER=1
            JUCE_WEB_BROWSER=0
            JUCE_USE_CURL=0
            JUCE_DISPLAY_SPLASH_SCREEN=0
    )
    if(is_demo)
        target_compile_definitions(${tgt} PUBLIC ORIENTAL_DEMO=1)
    endif()

    target_link_libraries(${tgt}
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
endfunction()

add_oriental_target(OrientalInstrument     "Oriental Instrument"      OrIn FALSE)
add_oriental_target(OrientalInstrumentDemo "Oriental Instrument DEMO" OrDm TRUE)

# ============================================================================
# iOS Standalone App
# ============================================================================
if(JUCE_PLATFORM_IOS OR CMAKE_SYSTEM_NAME STREQUAL "iOS")
    juce_add_gui_app(OrientalInstrumentiOS
        COMPANY_NAME "DJBILBOX BEATS"
        PRODUCT_NAME "Oriental Instrument"
        DESCRIPTION "Oriental Virtual Instrument - Maqam Edition (iOS)"
        BUNDLE_ID com.djbilbox.orientalinstrument.ios
        ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/Icon-iOS.png"
    )

    target_sources(OrientalInstrumentiOS PRIVATE
        ${ORIENTAL_SOURCES}
        ${ORIENTAL_MOBILE_SOURCES}
        Source/Mobile/Main.cpp
        Source/Mobile/iOS/MainViewController.cpp
    )

    juce_generate_juce_header(OrientalInstrumentiOS)
    target_include_directories(OrientalInstrumentiOS PRIVATE Source)

    target_compile_definitions(OrientalInstrumentiOS
        PUBLIC
            JUCE_STRICT_REFCOUNTEDPOINTER=1
            JUCE_WEB_BROWSER=0
            JUCE_USE_CURL=0
            ORIENTAL_MOBILE=1
            ORIENTAL_IOS=1
    )

    target_link_libraries(OrientalInstrumentiOS
        PRIVATE
            juce::juce_audio_basics
            juce::juce_audio_devices
            juce::juce_audio_formats
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

    # iOS-specific settings
    set_target_properties(OrientalInstrumentiOS PROPERTIES
        MACOSX_DEPLOYMENT_TARGET "13.0"
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
    )
endif()

# ============================================================================
# Android Standalone App
# ============================================================================
if(JUCE_PLATFORM_ANDROID OR CMAKE_SYSTEM_NAME STREQUAL "Android")
    juce_add_gui_app(OrientalInstrumentAndroid
        COMPANY_NAME "DJBILBOX BEATS"
        PRODUCT_NAME "Oriental Instrument"
        DESCRIPTION "Oriental Virtual Instrument - Maqam Edition (Android)"
        BUNDLE_ID com.djbilbox.orientalinstrument.android
    )

    target_sources(OrientalInstrumentAndroid PRIVATE
        ${ORIENTAL_SOURCES}
        ${ORIENTAL_MOBILE_SOURCES}
        Source/Mobile/Main.cpp
        Source/Mobile/Android/MainActivity.cpp
    )

    juce_generate_juce_header(OrientalInstrumentAndroid)
    target_include_directories(OrientalInstrumentAndroid PRIVATE Source)

    target_compile_definitions(OrientalInstrumentAndroid
        PUBLIC
            JUCE_STRICT_REFCOUNTEDPOINTER=1
            JUCE_WEB_BROWSER=0
            JUCE_USE_CURL=0
            ORIENTAL_MOBILE=1
            ORIENTAL_ANDROID=1
    )

    target_link_libraries(OrientalInstrumentAndroid
        PRIVATE
            juce::juce_audio_basics
            juce::juce_audio_devices
            juce::juce_audio_formats
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
endif()
