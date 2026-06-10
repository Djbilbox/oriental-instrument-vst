#include "PresetBrowser.h"
#include "../Utils/Constants.h"

PresetBrowser::PresetBrowser(PresetManager& pm, juce::AudioProcessorValueTreeState& state)
    : presetManager(pm), apvts(state)
{
    listBox.setModel(this);
    listBox.setColour(juce::ListBox::backgroundColourId, juce::Colours::transparentBlack);
    listBox.setRowHeight(40);
    addAndMakeVisible(listBox);

    searchBox.setTextToShowWhenEmpty("Search preset, city, key...", juce::Colour(0xFF555555));
    searchBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x80000000));
    searchBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0xFF8A6018));
    searchBox.setColour(juce::TextEditor::textColourId, juce::Colour(0xFFCCCCCC));
    searchBox.onTextChange = [this]() { searchFilter = searchBox.getText(); updateFilteredList(); };
    addAndMakeVisible(searchBox);

    const char* tabNames[] = { "\xf0\x9f\x8e\xbb VLN", "\xf0\x9f\x8e\xb8 OUD", "\xf0\x9f\x8c\xac NEY", "\xf0\x9f\x8e\xb5 QAN", "\xf0\x9f\xa5\x81 DARB", "\xf0\x9f\xaa\x95 RAB", "\xf0\x9f\x8e\xba MIZ" };
    for (int i = 0; i < 7; ++i)
    {
        instrumentTabs[static_cast<size_t>(i)].setButtonText(tabNames[i]);
        instrumentTabs[static_cast<size_t>(i)].setClickingTogglesState(true);
        instrumentTabs[static_cast<size_t>(i)].setRadioGroupId(101); // groupe radio exclusif
        instrumentTabs[static_cast<size_t>(i)].setColour(juce::TextButton::buttonColourId, juce::Colour(0x66000000));
        instrumentTabs[static_cast<size_t>(i)].setColour(juce::TextButton::textColourOnId, juce::Colour(OrientalConstants::Colors::GOLD));
        instrumentTabs[static_cast<size_t>(i)].setColour(juce::TextButton::textColourOffId, juce::Colour(0xFFAAAAAA));
        instrumentTabs[static_cast<size_t>(i)].onClick = [this, i]()
        {
            setInstrumentFilter(static_cast<OrientalConstants::Instrument>(i));
            // Notify APVTS of instrument change
            if (auto* p = apvts.getParameter("instrument"))
                p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(i)));
        };
        addAndMakeVisible(instrumentTabs[static_cast<size_t>(i)]);
    }

    // Highlight first tab
    instrumentTabs[0].setToggleState(true, juce::dontSendNotification);
    updateFilteredList();
}

void PresetBrowser::paint(juce::Graphics& g)
{
    g.setColour(juce::Colour(0xC70A0805));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 2.0f);
    g.setColour(juce::Colour(OrientalConstants::Colors::GLASS_BORDER));
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 2.0f, 1.0f);
}

void PresetBrowser::resized()
{
    auto bounds = getLocalBounds().reduced(4);
    searchBox.setBounds(bounds.removeFromTop(26));
    bounds.removeFromTop(4);

    auto tabArea = bounds.removeFromTop(28);
    int tabWidth = tabArea.getWidth() / 7;
    for (int i = 0; i < 7; ++i)
        instrumentTabs[static_cast<size_t>(i)].setBounds(tabArea.removeFromLeft(tabWidth));

    bounds.removeFromTop(4);
    listBox.setBounds(bounds);
}

int PresetBrowser::getNumRows()
{
    return static_cast<int>(filteredIndices.size());
}

void PresetBrowser::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowNumber < 0 || rowNumber >= static_cast<int>(filteredIndices.size()))
        return;

    auto& preset = presetManager.getPreset(filteredIndices[static_cast<size_t>(rowNumber)]);

    if (rowIsSelected)
    {
        g.setColour(juce::Colour(0x24C8922A));
        g.fillRoundedRectangle(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 4.0f);
        g.setColour(juce::Colour(OrientalConstants::Colors::GOLD_DIM));
        g.drawRoundedRectangle(0.5f, 0.5f, static_cast<float>(width) - 1.0f, static_cast<float>(height) - 1.0f, 4.0f, 1.0f);
    }

    const int pad = 6;
    const int numW = 18;
    const int line1Y = 2;
    const int line1H = 22;
    const int line2Y = 22;
    const int line2H = 16;

    // Numéro (centré verticalement)
    g.setColour(juce::Colour(0xFF555555));
    g.setFont(juce::Font(9.0f));
    g.drawText(juce::String(rowNumber + 1), 0, 0, numW, height, juce::Justification::centred);

    // ── Ligne 1 : Nom du preset (grand, Cinzel-like) ───────────────────────
    g.setColour(rowIsSelected ? juce::Colour(OrientalConstants::Colors::GOLD_LIGHT)
                              : juce::Colour(0xFFE8E8E8));
    g.setFont(juce::Font("Cinzel", 13.0f, juce::Font::plain));
    g.drawText(preset.name, numW + pad, line1Y, width - numW - pad * 2, line1H,
               juce::Justification::bottomLeft);

    // ── Ligne 2 : City  •  Key  •  Maqam (petit, discret) ─────────────────
    juce::String subLine = preset.city;
    if (preset.key.isNotEmpty() && preset.key != "-")
        subLine += "  \xe2\x80\xa2  " + preset.key;
    if (preset.maqam.isNotEmpty() && preset.maqam != "-")
        subLine += "  \xe2\x80\xa2  " + preset.maqam;

    g.setColour(juce::Colour(rowIsSelected ? OrientalConstants::Colors::GOLD_DIM
                                           : 0xFF888888u));
    g.setFont(juce::Font(9.5f));
    g.drawText(subLine, numW + pad, line2Y, width - numW - pad * 2, line2H,
               juce::Justification::topLeft);
}

void PresetBrowser::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    if (row >= 0 && row < static_cast<int>(filteredIndices.size()))
    {
        int presetIndex = filteredIndices[static_cast<size_t>(row)];
        presetManager.selectPreset(presetIndex);

        // Write preset index to APVTS — this triggers parameterChanged in Processor
        if (auto* p = apvts.getParameter("preset"))
            p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(presetIndex)));

        listBox.repaint();
    }
}

void PresetBrowser::setInstrumentFilter(OrientalConstants::Instrument inst)
{
    currentInstrument = inst;

    // Update tab highlighting
    for (int i = 0; i < 7; ++i)
        instrumentTabs[static_cast<size_t>(i)].setToggleState(i == static_cast<int>(inst), juce::dontSendNotification);

    updateFilteredList();
}

void PresetBrowser::updateFilteredList()
{
    filteredIndices.clear();
    for (int i = 0; i < presetManager.getNumPresets(); ++i)
    {
        auto& p = presetManager.getPreset(i);
        if (p.instrument != currentInstrument) continue;
        if (searchFilter.isNotEmpty())
        {
            if (!p.name.containsIgnoreCase(searchFilter)
                && !p.city.containsIgnoreCase(searchFilter)
                && !p.key.containsIgnoreCase(searchFilter)
                && !p.maqam.containsIgnoreCase(searchFilter))
                continue;
        }
        filteredIndices.push_back(i);
    }
    listBox.updateContent();
    listBox.repaint();
}
