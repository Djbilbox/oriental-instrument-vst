#include "MobileKeyboard.h"

MobileKeyboard::MobileKeyboard(OrientalInstrumentProcessor& proc)
    : processor(proc)
{
    // Initialize with standard range: 2 octaves of quarter-tones starting from C3 (36 = C1)
    baseNote = 36;
    numOctaves = 2;
    keysPerOctave = 24;  // Maqam quarter-tone system
    keys.resize(numOctaves * keysPerOctave);
}

void MobileKeyboard::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

    // Draw keyboard keys
    for (const auto& key : keys)
    {
        // Distinguish white vs. black keys (simplified: every 3rd and 6th quarter-tone is "black")
        bool isBlackKey = (key.midiNote % 24) % 6 == 3;

        juce::Colour keyColour = isBlackKey ? juce::Colours::darkgrey : juce::Colours::lightgrey;
        if (key.isPressed)
            keyColour = juce::Colours::red;

        g.setColour(keyColour);
        g.fillRect(key.bounds);

        // Border
        g.setColour(juce::Colours::black);
        g.drawRect(key.bounds, 1.0f);
    }
}

void MobileKeyboard::resized()
{
    updateKeyLayout();
}

void MobileKeyboard::updateKeyLayout()
{
    auto bounds = getLocalBounds().toFloat();
    int totalKeys = keys.size();

    if (totalKeys == 0) return;

    float keyWidth = bounds.getWidth() / totalKeys;
    float keyHeight = bounds.getHeight();

    for (int i = 0; i < totalKeys; ++i)
    {
        keys[i].midiNote = baseNote + i;
        keys[i].bounds = {
            bounds.getX() + i * keyWidth,
            bounds.getY(),
            keyWidth,
            keyHeight
        };
    }
}

void MobileKeyboard::mouseDown(const juce::MouseEvent& e)
{
    auto pos = e.getPosition();
    int note = getNoteAtPoint(pos);

    if (note >= 0)
    {
        processor.getSynthesiser().noteOn(1, note, 0.8f);
        for (auto& k : keys)
        {
            if (k.midiNote == note)
                k.isPressed = true;
        }
        repaint();
    }

    lastTouchPos = e.getPosition().toFloat();
}

void MobileKeyboard::mouseDrag(const juce::MouseEvent& e)
{
    auto pos = e.getPosition().toFloat();
    float yDelta = lastTouchPos.y - pos.y;  // Upward drag = pitch bend up

    // Normalize: screen height = ±1 octave pitch bend
    float screenHeight = getHeight();
    float bendAmount = juce::jlimit(-1.0f, 1.0f, (yDelta / screenHeight) * 2.0f);

    setPitchBend(bendAmount);
    lastTouchPos = pos;
}

void MobileKeyboard::mouseUp(const juce::MouseEvent& e)
{
    // Release all notes
    for (auto& k : keys)
    {
        if (k.isPressed)
        {
            processor.getSynthesiser().noteOff(1, k.midiNote, 0.5f, false);
            k.isPressed = false;
        }
    }

    // Reset pitch bend
    setPitchBend(0.0f);
    repaint();
}

int MobileKeyboard::getNoteAtPoint(juce::Point<int> pos)
{
    auto point = pos.toFloat();
    for (const auto& k : keys)
    {
        if (k.bounds.contains(point))
            return k.midiNote;
    }
    return -1;
}

void MobileKeyboard::setPitchBend(float normalizedValue)
{
    pitchBendAmount = normalizedValue;
    processor.setPitchBend(normalizedValue);
}
