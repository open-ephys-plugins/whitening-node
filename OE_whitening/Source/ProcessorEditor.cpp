
#include "ProcessorEditor.h"
#include "ProcessorPlugin.h"
#include <stdio.h>
#include "GridLayout.h"

using namespace ProcessorPluginSpace;

ProcessorEditor::ProcessorEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors = true)
    : GenericEditor(parentNode, useDefaultParameterEditors)

{
    desiredWidth = 300;

    float fontSize = 12;

    bufferSizeLabel.reset(new juce::Label("new label",
        TRANS("Buffer size (s):\n")));
    addAndMakeVisible(bufferSizeLabel.get());
    bufferSizeLabel->setJustificationType(juce::Justification::centredLeft);
    bufferSizeLabel->setEditable(false, false, false);
    bufferSizeLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    bufferSizeLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x00000000));

    bufferSizeValue.reset(new juce::Label("new label",
        TRANS("2")));
    addAndMakeVisible(bufferSizeValue.get());
    bufferSizeValue->setJustificationType(juce::Justification::centredLeft);
    bufferSizeValue->setEditable(true,true,true);
    bufferSizeValue->setColour(juce::Label::backgroundColourId, juce::Colours::grey);
    bufferSizeValue->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    bufferSizeValue->addListener(this);

    whiteningStatusLabel.reset(new juce::Label("new label",
        TRANS("Whitening matrix")));
    addAndMakeVisible(whiteningStatusLabel.get());
    whiteningStatusLabel->setJustificationType(juce::Justification::centredLeft);
    whiteningStatusLabel->setEditable(false, false, false);
    whiteningStatusLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    whiteningStatusLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x00000000));

    whiteningStatusValue.reset(new juce::Label("new label",
        TRANS("waiting...\n")));
    addAndMakeVisible(whiteningStatusValue.get());
    whiteningStatusValue->setJustificationType(juce::Justification::centredLeft);
    whiteningStatusValue->setEditable(false, false, false);
    whiteningStatusValue->setColour(juce::Label::backgroundColourId, juce::Colours::aquamarine);
    whiteningStatusValue->setColour(juce::Label::textColourId, juce::Colours::blue);
    whiteningStatusValue->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    whiteningStatusValue->setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x00000000));

    whiteningToggle.reset(new juce::ToggleButton("new toggle button"));
    addAndMakeVisible(whiteningToggle.get());
    whiteningToggle->setButtonText(TRANS("Apply whitening"));
    whiteningToggle->setToggleState(true,true);
    whiteningToggle->addListener(this);

    resetButton.reset(new juce::TextButton("new button"));
    addAndMakeVisible(resetButton.get());
    resetButton->setButtonText(TRANS("Reset buffer"));
    resetButton->addListener(this);

}

void ProcessorEditor::updateToggleState(Button* button) {
    if (button == whiteningToggle.get()) {
        bool isApplyWhitening = button->getToggleState();
        std::cout << "setting isApplyWhitening to " << isApplyWhitening << std::endl;
        ProcessorPlugin* processor = (ProcessorPlugin*)getProcessor();
        processor->setApplyWhitening(isApplyWhitening);
    }
}


void ProcessorEditor::resized() {
    auto area = getLocalBounds();
    //auto padding = 10;
    //auto leftPanelWidth = area.getWidth() / 2 - 2*padding;
    //auto contentItemHeight = 24;
    //auto titleSize = 30;

    //area.removeFromTop(titleSize);

    //auto leftPanel = area.removeFromLeft(leftPanelWidth);

    ////add padding
    //leftPanel.removeFromLeft(padding);
    //leftPanel.removeFromRight(padding);
    //area.removeFromLeft(padding);
    //area.removeFromRight(padding);

    //bufferSizeLabel->setBounds(leftPanel.removeFromTop(contentItemHeight));
    //bufferSizeValue->setBounds(leftPanel.removeFromTop(contentItemHeight));
    //whiteningToggle->setBounds(leftPanel.removeFromTop(contentItemHeight));

    //whiteningStatusLabel->setBounds(area.removeFromTop(contentItemHeight));
    //whiteningStatusValue->setBounds(area.removeFromTop(contentItemHeight));
    //resetButton->setBounds(area.removeFromTop(contentItemHeight));

    GridLayout layout(area, 3, 2);
    bufferSizeLabel->setBounds(layout.getBoundAt(0, 0));
    bufferSizeValue->setBounds(layout.getBoundAt(1,0));
    whiteningToggle->setBounds(layout.getBoundAt(2,0));

    whiteningStatusLabel->setBounds(layout.getBoundAt(0,1));
    whiteningStatusValue->setBounds(layout.getBoundAt(1,1));
    resetButton->setBounds(layout.getBoundAt(2,1));

}

ProcessorEditor::~ProcessorEditor()
{
}


void ProcessorEditor::labelTextChanged(Label* label)
{
    if (label == bufferSizeValue.get()) {
        resetBuffer();
    }
  
}

void ProcessorEditor::buttonClicked(Button* button) 
{
    if (button == resetButton.get()) {
        resetBuffer();
    }
    else if (button == whiteningToggle.get()) {
        updateToggleState(button);
    }
}

void ProcessorEditor::resetBuffer() {
    //reset the whitening buffer

    //Get underlying processor
    ProcessorPlugin* processor = (ProcessorPlugin*)getProcessor();

    double bufferLengthNew = double(bufferSizeValue->getTextValue().getValue());
    std::cout << "Reseting the buffer to " << bufferSizeValue << " s" << endl;

    //Set the correct buffer length
    processor->setBufferLength(bufferLengthNew);

    //Clear the buffer
    processor->resetBuffer();


}

void ProcessorEditor::setWhiteningStatus(String status)
{
    //If set from other than the main thread, need to acquire the lock first
    {
        MessageManagerLock mmlock;
        whiteningStatusValue->setText(status, juce::NotificationType::dontSendNotification);

    }
}

void ProcessorEditor::saveCustomParameters(XmlElement* xml)
{
    std::cout << "Writing whitening editor parameters" << std::endl;
    xml->setAttribute("Type", "WhiteningEditor");
    String bufferSize = bufferSizeValue->getTextValue().getValue();
    XmlElement* textLabelValues = xml->createNewChildElement("VALUES");
    textLabelValues->setAttribute("BufferSize", bufferSize);



}

void ProcessorEditor::loadCustomParameters(XmlElement* xml)
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

