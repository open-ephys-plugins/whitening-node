
#include "WhiteningNodeEditor.h"
#include <stdio.h>
#include "GridLayout.h"


WhiteningNodeEditor::WhiteningNodeEditor(WhiteningNode* parentNode, bool useDefaultParameterEditors = true)
    : GenericEditor(parentNode, useDefaultParameterEditors)

{
    processor = parentNode;
    
    desiredWidth = 300;

    bufferSizeLabel.reset(new juce::Label("Buffer Size Label",
        TRANS("Buffer size (s):\n")));
    addAndMakeVisible(bufferSizeLabel.get());
    bufferSizeLabel->setJustificationType(juce::Justification::centredLeft);
    bufferSizeLabel->setColour(juce::Label::textColourId, juce::Colours::darkgrey);
    bufferSizeLabel->setFont(Font("Small Text", 12, Font::plain));

    bufferSizeValue.reset(new juce::Label("Buffer Size Value",
        TRANS("10")));
    addAndMakeVisible(bufferSizeValue.get());
    bufferSizeValue->setJustificationType(juce::Justification::centredLeft);
    bufferSizeValue->setEditable(true,true,true);
    bufferSizeValue->setColour(juce::Label::backgroundColourId, juce::Colours::grey);
    bufferSizeValue->setColour(juce::Label::textColourId, juce::Colours::white);
    bufferSizeValue->addListener(this);
    bufferSizeValue->setFont(Font("Default", 16, Font::plain));

    whiteningStatusLabel.reset(new juce::Label("Whtening status label",
        TRANS("Whitening Matrix")));
    addAndMakeVisible(whiteningStatusLabel.get());
    whiteningStatusLabel->setJustificationType(juce::Justification::centredLeft);
    whiteningStatusLabel->setColour(juce::Label::textColourId, juce::Colours::darkgrey);
    whiteningStatusLabel->setFont(Font("Small Text", 12, Font::plain));

    whiteningStatusValue.reset(new juce::Label("new label",
        TRANS("Waiting...\n")));
    addAndMakeVisible(whiteningStatusValue.get());
    whiteningStatusValue->setJustificationType(juce::Justification::centredLeft);
    whiteningStatusValue->setEditable(false, false, false);
    whiteningStatusValue->setColour(juce::Label::backgroundColourId, juce::Colours::grey);
    whiteningStatusValue->setColour(juce::Label::textColourId, juce::Colours::white);
    whiteningStatusValue->setFont(Font("Default", 16, Font::plain));

    whiteningToggle.reset(new UtilityButton("Apply Whitening", Font("Default", 12, Font::plain)));
    addAndMakeVisible(whiteningToggle.get());
    whiteningToggle->setButtonText(TRANS("Apply whitening"));
    whiteningToggle->setClickingTogglesState(true);
    whiteningToggle->setToggleState(true, sendNotification);
    whiteningToggle->addListener(this);

    resetButton.reset(new UtilityButton("Reset Buffer", Font("Default", 12, Font::plain)));
    addAndMakeVisible(resetButton.get());
    resetButton->setButtonText(TRANS("Reset buffer"));
    resetButton->addListener(this);

}

void WhiteningNodeEditor::updateToggleState(Button* button) {
    if (button == whiteningToggle.get()) {
        bool isApplyWhitening = button->getToggleState();
        std::cout << "setting isApplyWhitening to " << isApplyWhitening << std::endl;
        processor->setApplyWhitening(isApplyWhitening);
    }
}


void WhiteningNodeEditor::resized() {

    bufferSizeLabel->setBounds(15, 25, 120, 25);
    bufferSizeValue->setBounds(20, 50, 105, 25);
    whiteningStatusLabel->setBounds(150, 25, 140, 25);
    whiteningStatusValue->setBounds(150, 50, 135, 25);
    resetButton->setBounds(20, 90, 105, 30);
    whiteningToggle->setBounds(160, 90, 120, 30);

}

WhiteningNodeEditor::~WhiteningNodeEditor()
{
}


void WhiteningNodeEditor::labelTextChanged(Label* label)
{
    if (label == bufferSizeValue.get()) {
        resetBuffer();
    }
  
}

void WhiteningNodeEditor::buttonEvent(Button* button) 
{
    if (button == resetButton.get()) {
        resetBuffer();
    }
    else if (button == whiteningToggle.get()) {
        updateToggleState(button);
    }
}

void WhiteningNodeEditor::resetBuffer() {
    //reset the whitening buffer
    double bufferLengthNew = double(bufferSizeValue->getTextValue().getValue());
    std::cout << "Reseting the buffer to " << bufferSizeValue->getText() << " s" << endl;

    //Set the correct buffer length
    processor->setBufferLength(bufferLengthNew);

}

void WhiteningNodeEditor::setWhiteningStatus(String status)
{
    //If set from other than the main thread, need to acquire the lock first
    {
        MessageManagerLock mmlock;
        whiteningStatusValue->setText(status, juce::NotificationType::dontSendNotification);

    }
}

void WhiteningNodeEditor::saveCustomParameters(XmlElement* xml)
{
    std::cout << "Writing whitening editor parameters" << std::endl;
    xml->setAttribute("Type", "WhiteningEditor");
    String bufferSize = bufferSizeValue->getTextValue().getValue();
    XmlElement* textLabelValues = xml->createNewChildElement("VALUES");
    textLabelValues->setAttribute("BufferSize", bufferSize);



}

void WhiteningNodeEditor::loadCustomParameters(XmlElement* xml)
{
    std::cout << "Loading Whitening editor parameters" << std::endl;

    forEachXmlChildElement(*xml, xmlNode)
    {
        if (xmlNode->hasTagName("VALUES"))
        {
            String bufferSize = "10";
            bufferSize = xmlNode->getStringAttribute("BufferSize", bufferSize);
            bufferSizeValue->setText(bufferSize, sendNotification);
        }
    }
}

