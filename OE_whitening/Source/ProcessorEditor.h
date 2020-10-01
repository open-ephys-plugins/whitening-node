#pragma once

#include <EditorHeaders.h>



class ProcessorEditor : public GenericEditor,
    public Label::Listener,
    public Button::Listener
{
public:
    ProcessorEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors);
    void updateToggleState(Button* button);
    virtual ~ProcessorEditor();
    void resized() override;
    void labelTextChanged(Label* label);
    void buttonClicked(juce::Button* button) override;
    void resetBuffer();
    void setWhiteningStatus(String status);
     

private:

    String bufferSizeString;
    std::unique_ptr<juce::Label> bufferSizeLabel;
    std::unique_ptr<juce::Label> bufferSizeValue;
    std::unique_ptr<juce::Label> whiteningStatusLabel;
    std::unique_ptr<juce::Label> whiteningStatusValue;
    std::unique_ptr<juce::ToggleButton> whiteningToggle;
    std::unique_ptr<juce::TextButton> resetButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessorEditor);
};
