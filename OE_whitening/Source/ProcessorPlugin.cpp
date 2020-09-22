#include "ProcessorPlugin.h"
#include "Eigen/Dense"

using namespace ProcessorPluginSpace;
using Eigen::MatrixXd;

//Change all names for the relevant ones, including "Processor Name"
ProcessorPlugin::ProcessorPlugin() : 
    GenericProcessor("Whitening")
    , abstractFifo(100)
    , bufferLength(10.0f)
{
    subprocessorToDraw = 0;
    numSubprocessors = -1;
    samplingRate = 0;
    numChannels = 0;
}

ProcessorPlugin::~ProcessorPlugin()
{

}

void ProcessorPlugin::updateSettings()
{

    std::cout << "Setting num inputs on ProcessorPlugin to " << getNumInputs() << std::endl;

    numChannelsInSubprocessor.clear();
    subprocessorSampleRate.clear();


    //Assum only one subprocessor, and all data channel has the same sampling rate
    if (getNumInputs() > 0) {
        samplingRate = getDataChannel(0)->getSampleRate();
        numChannels = getNumInputs();

    }
    
    displayBuffers = std::make_shared<AudioSampleBuffer>(8, 100);

    std::cout << "Re-setting num inputs on ProcessorPlugin to " << numChannels << std::endl;
    if (numChannels > 0)
    {
        std::cout << "Sample rate = " << samplingRate << std::endl;
    }



    resizeBuffer();


}



uint32 ProcessorPlugin::getChannelSourceId(const InfoObjectCommon* chan)
{
    return getProcessorFullId(chan->getSourceNodeID(), chan->getSubProcessorIdx());
}

uint32 ProcessorPlugin::getDataSubprocId(int chan) const
{
    if (chan < 0 || chan >= getTotalDataChannels())
    {
        return 0;
    }

    return getChannelSourceId(getDataChannel(chan));
}

void ProcessorPlugin::setSubprocessor(uint32 sp)
{
    subprocessorToDraw = sp;
    std::cout << "ProcessorPlugin setting subprocessor to " << sp << std::endl;
}

uint32 ProcessorPlugin::getSubprocessor() const
{
    return subprocessorToDraw;
}

int ProcessorPlugin::getNumSubprocessorChannels()
{
    if (subprocessorToDraw != 0)
    {
        return numChannelsInSubprocessor[subprocessorToDraw];
    }
    return 0;
}

float ProcessorPlugin::getSubprocessorSampleRate(uint32 subprocId)
{
    auto entry = subprocessorSampleRate.find(subprocId);
    if (entry != subprocessorSampleRate.end())
    {
        return entry->second;
    }
    return 0.0f;
}

bool ProcessorPlugin::resizeBuffer()
{
    int totalResized = 0;

    ScopedLock displayLock(displayMutex);

    int nSamples = (int)samplingRate * bufferLength; //always return the first subprocessor
    int nInputs = numChannels;

    std::cout << "Resizing buffer " << ". Samples: " << nSamples << ", Inputs: " << nInputs << std::endl;

    if (nSamples > 0 && nInputs > 0)
    {
        abstractFifo.setTotalSize(nSamples);
        displayBuffers->setSize(nInputs + 1, nSamples); // add extra channel for TTLs
        displayBuffers->clear();

        //displayBufferIndices[currSubproc].clear();
        //displayBufferIndices[currSubproc].insert(displayBufferIndices[currSubproc].end(), nInputs + 1, 0);
        displayBufferIndices.clear();
        displayBufferIndices.insert(displayBufferIndices.end(), nInputs + 1, 0);
    }

    return true;

}

void ProcessorPlugin::process(AudioSampleBuffer& buffer)
{
    // 1. place any new samples into the displayBuffer
  //std::cout << "Display node sample count: " << nSamples << std::endl; ///buffer.getNumSamples() << std::endl;

    if (true)
    {
        ScopedLock displayLock(displayMutex);


        if (true)
        {
            int channelIndex = -1;

            for (int chan = 0; chan < buffer.getNumChannels(); ++chan)
            {


                const int samplesLeft = displayBuffers->getNumSamples() - displayBufferIndices[chan];
                const int nSamples = getNumSamples(chan);

                if (nSamples < samplesLeft)
                {
                    displayBuffers->copyFrom(chan,                      // destChannel
                        displayBufferIndices[chan],  // destStartSample
                        buffer,                    // source
                        chan,                      // source channel
                        0,                         // source start sample
                        nSamples);                 // numSamples

                    displayBufferIndices[chan] = displayBufferIndices[chan] + nSamples;
                }
                else
                {
                    const int extraSamples = nSamples - samplesLeft;

                    displayBuffers->copyFrom(chan,                      // destChannel
                        displayBufferIndices[chan],  // destStartSample
                        buffer,                    // source
                        chan,                      // source channel
                        0,                         // source start sample
                        samplesLeft);              // numSamples

                    displayBuffers->copyFrom(chan,                      // destChannel
                        0,                         // destStartSample
                        buffer,                    // source
                        chan,                      // source channel
                        samplesLeft,               // source start sample
                        extraSamples);             // numSamples

                    displayBufferIndices[chan] = extraSamples;
                }
            }
            std::cout << "Buffer index of chan 0" << displayBufferIndices[0] << std::endl;

        }
    }

	//Do whatever processing needed

	//MatrixXd m(2, 2);
	//m(0, 0) = 3;
	//m(1, 0) = 2.5;
	//m(0, 1) = -1;
	//m(1, 1) = m(1, 0) + m(0, 1);
	//std::cout << m << std::endl;
	
	 
}

