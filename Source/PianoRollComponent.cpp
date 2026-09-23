#include "PianoRollComponent.h"

#include <algorithm>
#include <cmath>

PianoRollComponent::PianoRollComponent()
{
    setOpaque(true);
    notes.push_back({ 60, 0.0, 2.0, false });
    notes.push_back({ 64, 2.0, 2.0, false });
    notes.push_back({ 67, 4.0, 4.0, false });
    for (auto& note : notes)
        updateNoteBounds(note);
}

int PianoRollComponent::noteToY(int midiNote) const
{
    return static_cast<int>((108 - midiNote) * pixelsPerNote);
}

int PianoRollComponent::yToNote(int y) const
{
    return juce::jlimit(0, 127, 108 - static_cast<int>(std::floor((float)y / pixelsPerNote)));
}

int PianoRollComponent::beatToX(double beat) const
{
    return keyboardWidth + static_cast<int>(std::round(beat * pixelsPerBeat));
}

double PianoRollComponent::xToBeat(int x) const
{
    return std::max(0.0, (double)(x - keyboardWidth) / pixelsPerBeat);
}

void PianoRollComponent::updateNoteBounds(NoteEvent& note)
{
    note.selected = false;
}

PianoRollComponent::NoteEvent* PianoRollComponent::findNoteAt(int x, int y)
{
    for (auto it = notes.rbegin(); it != notes.rend(); ++it)
    {
        const auto noteX = beatToX(it->startBeat);
        const auto noteY = noteToY(it->midiNote);
        const auto width = static_cast<int>(std::round(it->lengthBeats * pixelsPerBeat));
        const auto height = static_cast<int>(std::round(pixelsPerNote));

        if (x >= noteX && x <= noteX + width && y >= noteY && y <= noteY + height)
            return &(*it);
    }

    return nullptr;
}

void PianoRollComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff14191f));

    for (int note = 0; note <= 127; ++note)
    {
        const bool isBlack = juce::MidiMessage::isMidiNoteBlack(note);
        const int y = noteToY(note);
        g.setColour(isBlack ? juce::Colour(0xff202733) : juce::Colour(0xff2b3039));
        g.fillRect(0, y, keyboardWidth, static_cast<int>(pixelsPerNote));
    }

    for (int x = keyboardWidth; x < getWidth(); x += static_cast<int>(pixelsPerBeat))
        g.setColour(juce::Colour(0xff3c4553));
        g.drawVerticalLine(x, 0.0f, (float)getHeight());

    for (const auto& note : notes)
    {
        const auto x = beatToX(note.startBeat);
        const auto y = noteToY(note.midiNote);
        const auto width = static_cast<int>(std::round(note.lengthBeats * pixelsPerBeat));
        const auto height = static_cast<int>(std::round(pixelsPerNote));

        g.setColour(note.selected ? juce::Colour(0xff7ee0a6) : juce::Colour(0xff4fa3ff));
        g.fillRoundedRectangle(juce::Rectangle<float>((float)x, (float)y, (float)width, (float)height), 2.0f);
    }
}

void PianoRollComponent::resized()
{
    repaint();
}

void PianoRollComponent::mouseDown(const juce::MouseEvent& event)
{
    if (event.x < keyboardWidth)
        return;

    auto* note = findNoteAt(event.x, event.y);
    if (note != nullptr)
    {
        note->selected = true;
        repaint();
        return;
    }

    createNoteAt(event.x, event.y);
}

void PianoRollComponent::mouseDrag(const juce::MouseEvent& event)
{
    if (event.x < keyboardWidth)
        return;

    for (auto& note : notes)
    {
        if (note.selected)
        {
            note.startBeat = xToBeat(event.x);
            note.midiNote = yToNote(event.y);
        }
    }

    repaint();
}

void PianoRollComponent::mouseDoubleClick(const juce::MouseEvent& event)
{
    if (event.x < keyboardWidth)
        return;

    createNoteAt(event.x, event.y);
}

void PianoRollComponent::createNoteAt(int x, int y)
{
    if (x < keyboardWidth)
        return;

    NoteEvent note;
    note.midiNote = yToNote(y);
    note.startBeat = xToBeat(x);
    note.lengthBeats = 1.0;
    note.selected = true;
    notes.push_back(note);
    repaint();
}
