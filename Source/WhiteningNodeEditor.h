#pragma once

#include <EditorHeaders.h>



class WhiteningNodeEditor : public GenericEditor,
    public Label::Listener,
    public Button::Listener
{
public:
    WhiteningNodeEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors);
    void updateToggleState(Button* button);
    virtual ~WhiteningNodeEditor();
    void resized() override;
    void labelTextChanged(Label* label);
    void buttonClicked(juce::Button* button) override;
    void resetBuffer();
    void setWhiteningStatus(String status);
    void saveCustomParameters(XmlElement* xml);
    void loadCustomParameters(XmlElement* xml);
     

private:

    String bufferSizeString;
    std::unique_ptr<juce::Label> bufferSizeLabel;
    std::unique_ptr<juce::Label> bufferSizeValue;
    std::unique_ptr<juce::Label> whiteningStatusLabel;
    std::unique_ptr<juce::Label> whiteningStatusValue;
    std::unique_ptr<juce::ToggleButton> whiteningToggle;
    std::unique_ptr<juce::TextButton> resetButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WhiteningNodeEditor);
};
