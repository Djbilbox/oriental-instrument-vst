#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PluginEditor.h"

/*
    iOSStandalone.cpp — Main iOS application entry point adapter.

    Bridges JUCE's standard plugin processor/editor to a standalone iOS app.
    The PluginProcessor handles all audio synthesis + DSP.
    The editor (PluginEditor) drives the UI with all GUI components.

    iOS 14.0+ requirement for AVAudioEngine integration and Metal rendering.

    Freemium: ORIENTAL_DEMO_IOS macro limits to 3 instruments, 3 presets each,
    2 effects (Reverb, Delay). Full features unlock via StoreKit2 IAP.
*/

class OrientalInstrumentiOSApplication : public juce::JUCEApplicationBase
{
public:
    OrientalInstrumentiOSApplication() = default;

    const juce::String getApplicationName() override
    {
        return "Oriental Instrument";
    }

    const juce::String getApplicationVersion() override
    {
        return "1.0.0";
    }

    bool moreThanOneInstanceAllowed() override { return true; }

    void anotherInstanceStarted (const juce::String&) override {}

    void systemRequestedQuit() override
    {
        quit();
    }

    void suspended() override
    {
        // iOS app backgrounded — pause audio processing
        if (auto* processor = getMainProcessor())
        {
            processor->releaseResources();
        }
    }

    void resumed() override
    {
        // iOS app resumed from background — reinitialize audio
        if (auto* processor = getMainProcessor())
        {
            // Audio engine will re-init on next play
        }
    }

    void shutdown() override
    {
        // Clean up processor
        mainWindow = nullptr;
    }

    void initialise (const juce::String&) override
    {
        mainWindow = std::make_unique<MainWindow> (getApplicationName());
    }

    /*
        Global accessor for the main audio processor.
        Used by UI components and IAP manager to access synthesis state.
    */
    static OrientalInstrumentProcessor* getMainProcessor()
    {
        auto* app = dynamic_cast<OrientalInstrumentiOSApplication*> (JUCEApplicationBase::getInstance());
        if (app && app->mainWindow)
            return app->mainWindow->getProcessor();
        return nullptr;
    }

private:
    class MainWindow : public juce::DocumentWindow
    {
    public:
        explicit MainWindow (const juce::String& name)
            : DocumentWindow (name,
                            juce::Desktop::getInstance().getDefaultLookAndFeel()
                                .findColour (ResizableWindow::backgroundColourId),
                            DocumentWindow::allButtons,
                            true)
        {
            // Create processor with proper audio session config for iOS
            processor = std::make_unique<OrientalInstrumentProcessor>();

            // Create editor UI
            auto editor = std::make_unique<OrientalInstrumentEditor> (*processor);

            // Set window properties
            setUsingNativeTitleBar (true);
            setContentOwned (editor.release(), true);

#if JUCE_IOS
            // iOS full-screen, respect notch/safe area
            setFullScreen (true);

            // Allow rotation to both portrait and landscape
            Desktop::getInstance().setOrientationsEnabled (
                Desktop::portrait | Desktop::landscapeLeft | Desktop::landscapeRight
            );
#endif

            setVisible (true);
        }

        ~MainWindow() override
        {
            processor = nullptr;
        }

        OrientalInstrumentProcessor* getProcessor() { return processor.get(); }

        void closeButtonPressed() override
        {
            JUCEApplicationBase::getInstance()->systemRequestedQuit();
        }

    private:
        std::unique_ptr<OrientalInstrumentProcessor> processor;
    };

    std::unique_ptr<MainWindow> mainWindow;
};

// JUCE application entry point
START_JUCE_APPLICATION (OrientalInstrumentiOSApplication)
