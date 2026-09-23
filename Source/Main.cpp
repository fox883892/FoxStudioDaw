#include <juce_gui_extra/juce_gui_extra.h>
#include "MainComponent.h"

class FoxStudioApplication final : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "FOX STUDIO DAW"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }
    void initialise(const juce::String&) override { window = std::make_unique<Window>(); }
    void shutdown() override { window.reset(); }
    void systemRequestedQuit() override { quit(); }

private:
    class Window final : public juce::DocumentWindow
    {
    public:
        Window() : DocumentWindow("FOX STUDIO DAW", juce::Colour(0xff121318), allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);
            setResizable(true, true);
            centreWithSize(1440, 900);
            setVisible(true);
        }
        void closeButtonPressed() override { JUCEApplication::getInstance()->systemRequestedQuit(); }
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Window)
    };
    std::unique_ptr<Window> window;
};

START_JUCE_APPLICATION(FoxStudioApplication)
