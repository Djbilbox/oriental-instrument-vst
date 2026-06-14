/*
  ==============================================================================
    AndroidStandalone.cpp - Oriental Instrument Android Standalone App
  ==============================================================================

    Wraps OrientalInstrumentProcessor into a JUCE standalone application
    for Android. Handles audio routing via OpenSLES, MIDI input from keyboard,
    and UI lifecycle callbacks from the Android Activity (JavaActivity).

    Freemium model (ORIENTAL_DEMO_ANDROID):
    - 3 instruments (Violin, Oud, Ney) instead of 7
    - 3 presets per instrument instead of 10
    - 2 effects (Reverb, Delay) instead of 8
    - In-app purchase to unlock full version
*/

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Mobile/AndroidActivity.h"
#include "Mobile/UI/AndroidUIAdapter.h"
#include "Mobile/IAP/AndroidIAPManager.h"

namespace OrientalMobileApp
{
    // Global singleton instance
    class OrientalAndroidApp : public juce::JUCEApplication
    {
    public:
        OrientalAndroidApp() = default;

        const juce::String getApplicationName() override
        {
            return "Oriental Instrument";
        }

        const juce::String getApplicationVersion() override
        {
            return "1.0.0";
        }

        bool moreThanOneInstanceAllowed() override
        {
            return false;
        }

        void initialise(const juce::String& commandLine) override
        {
            // Initialize audio processor
            processor = std::make_unique<OrientalInstrumentProcessor>();

            // Initialize IAP manager for Google Play Billing
            iapManager = std::make_unique<AndroidIAPManager>();
            iapManager->initialize();

            // Create main window with responsive UI
            mainWindow = std::make_unique<OrientalMainWindow>(
                "Oriental Instrument",
                processor.get(),
                iapManager.get()
            );

            mainWindow->setVisible(true);

            juce::Logger::writeToLog("Oriental Instrument Android started (DEMO mode)");
        }

        void shutdown() override
        {
            juce::Logger::writeToLog("Oriental Instrument shutting down...");
            mainWindow = nullptr;
            processor = nullptr;
            iapManager = nullptr;
        }

        void systemRequestedQuit() override
        {
            quit();
        }

        void anotherInstanceStarted(const juce::String& commandLine) override
        {
        }

    private:
        std::unique_ptr<OrientalInstrumentProcessor> processor;
        std::unique_ptr<AndroidIAPManager> iapManager;
        std::unique_ptr<juce::DocumentWindow> mainWindow;

        class OrientalMainWindow : public juce::DocumentWindow
        {
        public:
            OrientalMainWindow(
                const juce::String& name,
                OrientalInstrumentProcessor* proc,
                AndroidIAPManager* iap
            )
                : juce::DocumentWindow(name,
                                       juce::Desktop::getInstance()
                                           .getDefaultLookAndFeel()
                                           .findColour(
                                               juce::ResizableWindow::backgroundColourId),
                                       juce::DocumentWindow::allButtons),
                  processor(proc),
                  iapManager(iap)
            {
                // Create the editor (main UI component)
                auto* editor = new OrientalInstrumentEditor(*processor);

                // Wrap in responsive UI adapter for mobile screen sizes
                auto* uiAdapter = new AndroidUIAdapter(editor, iapManager);

                setContentOwned(uiAdapter, true);

                // Set size based on screen dimensions
                auto screenSize = juce::Desktop::getInstance().getDisplays()
                                     .getPrimaryDisplay()
                                     ->userArea.getSize();

                // Target portrait orientation on mobile
                int targetWidth = juce::jmin(screenSize.getWidth(), 540);
                int targetHeight = juce::jmin(screenSize.getHeight(), 860);

                setResizable(true, true);
                centreWithSize(targetWidth, targetHeight);

                setUsingNativeTitleBar(true);
            }

            void closeButtonPressed() override
            {
                juce::JUCEApplication::getInstance()->systemRequestedQuit();
            }

        private:
            OrientalInstrumentProcessor* processor;
            AndroidIAPManager* iapManager;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrientalMainWindow)
        };
    };

} // namespace OrientalMobileApp

// JUCE macro to instantiate the app
START_JUCE_APPLICATION(OrientalMobileApp::OrientalAndroidApp)
