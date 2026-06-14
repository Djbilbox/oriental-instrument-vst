#include "MobileKnob.h"

MobileKnob::MobileKnob(const juce::String& labelText, float initialValue)
    : value(initialValue), label(labelText)
{
    setRepaintsOnMouseActivity(true);
}

void MobileKnob::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    int size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    auto knobBounds = bounds.withSize(size, size).translated(
        (bounds.getWidth() - size) / 2,
        0
    );

    // Knob background circle
    g.setColour(juce::Colours::darkgrey);
    g.fillEllipse(knobBounds.toFloat());

    // Knob arc (0 to value)
    g.setColour(juce::Colours::red);
    float angle = value * juce::MathConstants<float>::twoPi - juce::MathConstants<float>::halfPi;
    juce::Path arcPath;
    auto center = knobBounds.getCentre().toFloat();
    float radius = knobBounds.getWidth() / 2.0f * 0.8f;

    arcPath.startNewSubPath(center.x + radius * std::cos(-juce::MathConstants<float>::halfPi),
                             center.y + radius * std::sin(-juce::MathConstants<float>::halfPi));
    arcPath.addCentredArc(center.x, center.y, radius, radius,
                          0, -juce::MathConstants<float>::halfPi, angle, false);

    g.strokePath(arcPath, juce::PathStrokeType(3.0f));

    // Pointer indicator
    g.setColour(juce::Colours::white);
    float pointerAngle = value * juce::MathConstants<float>::twoPi - juce::MathConstants<float>::halfPi;
    float pointerX = center.x + (radius - 5) * std::cos(pointerAngle);
    float pointerY = center.y + (radius - 5) * std::sin(pointerAngle);
    g.fillEllipse(pointerX - 3, pointerY - 3, 6, 6);

    // Label + value text
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    auto textBounds = bounds.removeFromBottom(30);
    g.drawText(label, textBounds.removeFromTop(15), juce::Justification::centred, true);

    if (formatValue)
    {
        juce::String valueStr = formatValue(value);
        g.drawText(valueStr, textBounds, juce::Justification::centred, true);
    }
    else
    {
        g.drawText(juce::String(static_cast<int>(value * 100)) + "%", textBounds, juce::Justification::centred, true);
    }

    // Border
    g.setColour(juce::Colours::grey);
    g.drawEllipse(knobBounds.toFloat().reduced(2), 1.0f);
}

void MobileKnob::mouseDown(const juce::MouseEvent& e)
{
    isDragging = true;
    lastMousePos = e.getPosition().toFloat();
}

void MobileKnob::mouseDrag(const juce::MouseEvent& e)
{
    if (!isDragging) return;

    auto pos = e.getPosition().toFloat();
    float yDelta = lastMousePos.y - pos.y;  // Upward drag increases value

    value += (yDelta / getHeight()) * dragSensitivity * 10.0f;
    value = juce::jlimit(0.0f, 1.0f, value);

    if (onValueChanged)
        onValueChanged(value);

    lastMousePos = pos;
    repaint();
}

void MobileKnob::mouseUp(const juce::MouseEvent& e)
{
    isDragging = false;
    repaint();
}

void MobileKnob::setValue(float v)
{
    value = juce::jlimit(0.0f, 1.0f, v);
    repaint();
}
