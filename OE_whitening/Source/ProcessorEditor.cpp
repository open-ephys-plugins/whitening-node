
#include "ProcessorEditor.h"
#include "ProcessorEditor.h"
#include <stdio.h>

ProcessorEditor::ProcessorEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors = true)
    : GenericEditor(parentNode, useDefaultParameterEditors)

{
    desiredWidth = 150;
    bufferSizeString = "10";

    bufferLengthLabel = new Label("Buffer size", "Buffer size (s):");
    bufferLengthValue = new Label("buffer size value", bufferSizeString);


    bufferLengthLabel->attachToComponent(bufferLengthValue,false);
    bufferLengthLabel->setJustificationType(juce::Justification::right);
    bufferLengthLabel->setFont(Font("Small Text", 12, Font::plain));
    bufferLengthLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(bufferLengthLabel);

    bufferLengthValue->setBounds(15, 60, 120, 18);
    bufferLengthValue->setFont(Font("Default", 15, Font::plain));
    bufferLengthValue->setColour(Label::textColourId, Colours::white);
    bufferLengthValue->setColour(Label::backgroundColourId, Colours::grey);
    bufferLengthValue->setEditable(true);
    bufferLengthValue->addListener(this);
    bufferLengthValue->setTooltip("Set the low cut for the selected channels");
    addAndMakeVisible(bufferLengthValue);

}

ProcessorEditor::~ProcessorEditor()
{
}


void ProcessorEditor::labelTextChanged(Label* label)
{
    Value val = label->getTextValue();
    std::cout << double(val.getValue()) << std::endl;
  
}
