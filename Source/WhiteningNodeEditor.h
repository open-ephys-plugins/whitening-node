#pragma once

#include <EditorHeaders.h>
#include "WhiteningNode.h"

class WhiteningNode;
class WhiteningNodeEditor : public GenericEditor,
    public juce::Label::Listener
{
public:
    WhiteningNodeEditor(WhiteningNode* parentNode, bool useDefaultParameterEditors);
    void updateToggleState(juce::Button* button);
    virtual ~WhiteningNodeEditor();
    void resized() override;
    void labelTextChanged(juce::Label* label);
    void buttonEvent(juce::Button* button) override;
    void resetBuffer();
    void setWhiteningStatus(String status);
    void saveCustomParameters(XmlElement* xml);
    void loadCustomParameters(XmlElement* xml);
     

private:

    WhiteningNode* processor;
    String bufferSizeString;
    std::unique_ptr<juce::Label> bufferSizeLabel;
    std::unique_ptr<juce::Label> bufferSizeValue;
    std::unique_ptr<juce::Label> whiteningStatusLabel;
    std::unique_ptr<juce::Label> whiteningStatusValue;
    std::unique_ptr<juce::ToggleButton> whiteningToggle;
    std::unique_ptr<juce::TextButton> resetButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WhiteningNodeEditor);
};
