#include "MobilePresetBrowser.h"

MobilePresetBrowser::MobilePresetBrowser(OrientalInstrumentProcessor& proc, PresetManager& mgr)
    : processor(proc), presetManager(mgr)
{
    scrollBar = std::make_unique<juce::ScrollBar>(false);
    scrollBar->addListener(this);
    addAndMakeVisible(*scrollBar);

    updatePresetList();
}

void MobilePresetBrowser::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey.withAlpha(0.8f));

    // Draw preset items
    for (const auto& item : visiblePresets)
    {
        // Highlight selected
        if (item.index == selectedPresetIndex)
            g.setColour(juce::Colours::red.withAlpha(0.6f));
        else
            g.setColour(item.available ? juce::Colours::grey : juce::Colours::darkgrey);

        g.fillRect(item.bounds);

        // Text
        g.setColour(item.available ? juce::Colours::white : juce::Colours::grey);
        g.setFont(14.0f);
        g.drawText(item.name, item.bounds.reduced(5), juce::Justification::left, true);

        // Border
        g.setColour(juce::Colours::black);
        g.drawRect(item.bounds, 1.0f);
    }
}

void MobilePresetBrowser::resized()
{
    auto bounds = getLocalBounds();
    int scrollWidth = 15;

    scrollBar->setBounds(bounds.removeFromRight(scrollWidth));

    updatePresetList();
}

void MobilePresetBrowser::updatePresetList()
{
    visiblePresets.clear();

    auto bounds = getLocalBounds().reduced(5);
    int itemHeight = 50;
    int maxVisible = (bounds.getHeight() - scrollBar->getHeight()) / itemHeight;

    int presetIndex = 0;
    for (int i = 0; i < OrientalConstants::PRESETS_PER_INSTRUMENT; ++i)
    {
        PresetItem item;
        item.index = presetIndex;
        item.name = juce::String("Preset ") + juce::String(i + 1);
        item.available = presetManager.isPresetAvailable(presetIndex);
        item.bounds = { bounds.getX(), bounds.getY() + (i - scrollOffset) * itemHeight,
                        bounds.getWidth() - 10, itemHeight };

        if (item.bounds.getY() > bounds.getY() && item.bounds.getY() < bounds.getBottom())
            visiblePresets.push_back(item);

        presetIndex++;
    }

    repaint();
}

void MobilePresetBrowser::mouseDown(const juce::MouseEvent& e)
{
    int presetIndex = getPresetAtPoint(e.getPosition());
    if (presetIndex >= 0 && presetManager.isPresetAvailable(presetIndex))
    {
        selectedPresetIndex = presetIndex;
        presetManager.selectPreset(currentInstrument, presetIndex);
        repaint();
    }
}

void MobilePresetBrowser::scrollBarMoved(juce::ScrollBar* scrollBar, double newRangeStart)
{
    scrollOffset = static_cast<int>(newRangeStart);
    updatePresetList();
}

int MobilePresetBrowser::getPresetAtPoint(juce::Point<int> pos)
{
    for (const auto& item : visiblePresets)
    {
        if (item.bounds.contains(pos))
            return item.index;
    }
    return -1;
}

void MobilePresetBrowser::setInstrument(int instIndex)
{
    if (instIndex >= 0 && instIndex < 7)
    {
        currentInstrument = instIndex;
        updatePresetList();
    }
}
