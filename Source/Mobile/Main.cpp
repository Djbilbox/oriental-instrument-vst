#include <JuceHeader.h>

#if ORIENTAL_IOS
    #include "iOS/MainViewController.h"
    using MainComponent = iOSMainComponent;
#elif ORIENTAL_ANDROID
    #include "Android/MainActivity.h"
    using MainComponent = AndroidMainComponent;
#endif

class OrientalInstrumentApplication : public juce::JUCEApplication
{
public:
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
        return true;
    }

    void initialise(const juce::String& commandLine) override
    {
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String& commandLine) override
    {
    }

private:
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(const juce::String& name)
            : juce::DocumentWindow(name,
                                   juce::Colours::black,
                                   allButtons,
                                   true)
        {
            setContentOwned(std::make_unique<MainComponent>().release(), true);

            #if JUCE_ANDROID || JUCE_IOS
                setFullScreen(true);
                setResizable(true, false);
            #else
                setBounds(100, 100, 800, 600);
                setResizable(true, true);
            #endif

            setVisible(true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(OrientalInstrumentApplication)
