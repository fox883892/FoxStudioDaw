#pragma once

#include <vector>
#include <juce_gui_extra/juce_gui_extra.h>

class PianoRollComponent final : public juce::Component
{
public:
    PianoRollComponent();
    ~PianoRollComponent() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void mouseDoubleClick(const juce::MouseEvent&) override;

private:
    struct NoteEvent
    {
        int midiNote = 60;
        double startBeat = 0.0;
        double lengthBeats = 1.0;
        bool selected = false;
    };

    int keyboardWidth = 60;
    double pixelsPerBeat = 56.0;
    double pixelsPerNote = 12.0;
    std::vector<NoteEvent> notes;

    int noteToY(int midiNote) const;
    int yToNote(int y) const;
    int beatToX(double beat) const;
    double xToBeat(int x) const;
    void createNoteAt(int x, int y);
    void updateNoteBounds(NoteEvent& note);
    NoteEvent* findNoteAt(int x, int y);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoRollComponent)
};
