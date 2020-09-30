#pragma once

#include <EditorHeaders.h>


class ProcessorEditor : public GenericEditor,
    public Label::Listener
{
public:
    ProcessorEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors);
    virtual ~ProcessorEditor();
    void labelTextChanged(Label* label);

private:

    String bufferSizeString;
    Label* bufferLengthLabel;
    Label* bufferLengthValue;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessorEditor);
};
