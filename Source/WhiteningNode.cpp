#include "WhiteningNode.h"
#include <fstream>
#include <chrono>

using namespace Eigen;
using std::sqrt;


//Change all names for the relevant ones, including "Processor Name"
WhiteningNode::WhiteningNode() : 
    GenericProcessor("Whitening")
    , abstractFifo(100)
{
    subprocessorToDraw = 0;
    numSubprocessors = -1;
    samplingRate = 0;
    numChannels = 0;
}

WhiteningNode::~WhiteningNode()
{

}

void WhiteningNode::updateSettings()
{

    std::cout << "Setting num inputs on WhiteningNode to " << getNumInputs() << std::endl;

    numChannelsInSubprocessor.clear();
    subprocessorSampleRate.clear();


    //Assum only one subprocessor, and all data channel has the same sampling rate
    if (getNumInputs() > 0) {
        samplingRate = getDataChannel(0)->getSampleRate();
        numChannels = getNumInputs();

    }
    
    displayBuffers = std::make_shared<AudioSampleBuffer>(8, 100);

    std::cout << "Re-setting num inputs on WhiteningNode to " << numChannels << std::endl;
    if (numChannels > 0)
    {
        std::cout << "Sample rate = " << samplingRate << std::endl;
    }

    //calculate the number of data channel. Note: this function will be called many times
    numDataChannel = 0;
    for (int n = 0; n < dataChannelArray.size(); n++) {
        if (dataChannelArray[n]->getChannelType() == DataChannel::HEADSTAGE_CHANNEL) {
            numDataChannel++;
        }
    }

    std::cout << "Whitening: Number of data channel: " << numDataChannel << std::endl;

    resizeBuffer();


}



uint32 WhiteningNode::getChannelSourceId(const InfoObjectCommon* chan)
{
    return getProcessorFullId(chan->getSourceNodeID(), chan->getSubProcessorIdx());
}

uint32 WhiteningNode::getDataSubprocId(int chan) const
{
    if (chan < 0 || chan >= getTotalDataChannels())
    {
        return 0;
    }

    return getChannelSourceId(getDataChannel(chan));
}

void WhiteningNode::setSubprocessor(uint32 sp)
{
    subprocessorToDraw = sp;
    std::cout << "WhiteningNode setting subprocessor to " << sp << std::endl;
}

uint32 WhiteningNode::getSubprocessor() const
{
    return subprocessorToDraw;
}

int WhiteningNode::getNumSubprocessorChannels()
{
    if (subprocessorToDraw != 0)
    {
        return numChannelsInSubprocessor[subprocessorToDraw];
    }
    return 0;
}

float WhiteningNode::getSubprocessorSampleRate(uint32 subprocId)
{
    auto entry = subprocessorSampleRate.find(subprocId);
    if (entry != subprocessorSampleRate.end())
    {
        return entry->second;
    }
    return 0.0f;
}

bool WhiteningNode::resizeBuffer()
{
    //Resize and reset the buffer

    int totalResized = 0;

    ScopedLock displayLock(displayMutex);

    int nSamples = (int)samplingRate * bufferLength; //always return the first subprocessor
    int nInputs = numDataChannel; // the display buffer only contains headstage data channels

    std::cout << "Resizing buffer " << ". Samples: " << nSamples << ", Inputs: " << nInputs << std::endl;

    if (nSamples > 0 && nInputs > 0)
    {
        abstractFifo.setTotalSize(nSamples);
        displayBuffers->setSize(nInputs, nSamples); // add extra channel for TTLs
        displayBuffers->clear();

        //displayBufferIndices[currSubproc].clear();
        //displayBufferIndices[currSubproc].insert(displayBufferIndices[currSubproc].end(), nInputs + 1, 0);
        displayBufferIndices.clear();
        displayBufferIndices.insert(displayBufferIndices.end(), nInputs, 0);
    }

    //clear some book keeping variables as well
    readyChannel = 0;
    isBufferReady = false;
    isNeedWhiteningUpdate = true;
    //m_whiteningMatrixReady = false;

    ProcessorEditor* editor = (ProcessorEditor*)getEditor();
    editor->setWhiteningStatus("Waiting...");

    return true;

}

void WhiteningNode::calculateWhiteningMatrix() {
    // Calculate the whitening matrix based on data in the buffer

     /*
    the AudioSampleBuffer is using a HeapBlock underneath. It contains two main segments. The first segment contains
    a list of pointers to the beginning memory location of each channel's data. The second segment contains the actual data.
    getArrayofReadPointers() will return the pointer to the list of channel pointers, while getReadpointers() will return the pointer to the data itself
    data is arranged in channel-major format

    */

    /*ofstream file;
    file.open("whitening_results.txt");*/

    auto start = chrono::system_clock::now();

    // Copy data over to the 
  /*  AudioSampleBuffer buffer_copy;
    buffer_copy.makeCopyOf(buffer);*/
    int numSample = displayBuffers->getNumSamples();
    cout << "Calculating whitening matrix now" << endl;


    std::cout << "Number of channel in whitening plugin: " << numDataChannel << std::endl;
    auto buffer_ptr = displayBuffers->getWritePointer(0); //get the beginning of the data
    Eigen::Map<Matrix<float,Dynamic, Dynamic, RowMajor>> m(buffer_ptr, numDataChannel, numSample); // by default MatrixXf is column-major


    auto mean = m.rowwise().mean();

    //file << "mean" << endl << mean << endl;;
    
    //std::cout << mean << endl;

    // subtract the mean
    m = m.colwise() - mean;

     //Covariance matrix
    auto AAt = m * m.transpose() / numSample;

    // SVD
    Eigen::BDCSVD<MatrixXf> svdSolver;
    svdSolver.compute(AAt, Eigen::ComputeFullU|Eigen::ComputeFullV);

    auto U = svdSolver.matrixU();
    auto V = svdSolver.matrixV();
    auto S = svdSolver.singularValues();


    // Apply whitening
    auto sinv = S.matrix().cwiseSqrt().cwiseInverse().asDiagonal();
    m_W = (U * sinv * V.transpose());

    //file << "W" << endl << m_W << endl;


    m_whiteningMatrixReady = true;
    isNeedWhiteningUpdate = false;

    //file.close();

    auto end = chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    cout << "Whitening took " << elapsed_seconds.count() << "s" << endl;

    ProcessorEditor* editor = (ProcessorEditor*)getEditor();
    editor->setWhiteningStatus("Ready");


}


void WhiteningNode::applyWhitening(AudioSampleBuffer& buffer) {
    int numSample = buffer.getNumSamples();
    auto buffer_ptr = buffer.getWritePointer(0);
    Map<Matrix<float, Dynamic, Dynamic, RowMajor>> input_data(buffer_ptr, numDataChannel, numSample);

    // remove mean
    auto mean = input_data.rowwise().mean();
    input_data = input_data.colwise() - mean;

    //whitening
    //input_data.array() *= 0;
    input_data = m_W * input_data;
}

void WhiteningNode::process(AudioSampleBuffer& buffer)
{
    // 1. place any new samples into the displayBuffer
  //std::cout << "Display node sample count: " << nSamples << std::endl; ///buffer.getNumSamples() << std::endl;
    if (true)
    {
        ScopedLock displayLock(displayMutex);

        if (readyChannel < numDataChannel)
        {
            //Store data in the displaybuffer so that we can estimate the whitening matrix later


            for (int chan = 0; chan < numDataChannel; ++chan)
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

                    // Wrap around
                    displayBuffers->copyFrom(chan,                      // destChannel
                        0,                         // destStartSample
                        buffer,                    // source
                        chan,                      // source channel
                        samplesLeft,               // source start sample
                        extraSamples);             // numSamples

                    displayBufferIndices[chan] = extraSamples;

                    readyChannel++;
                }
            }


       
        }
        else {
            if (isNeedWhiteningUpdate) {
                cout << "Buffer is ready" << std::endl;
                calculateWhiteningMatrix();
                cout << "Whitening matrix updated" << endl;
            }
   
        }

        // allow whitening to run even through the buffer is 
        if (m_whiteningMatrixReady) {
            if (isApplyWhitening) {
                applyWhitening(buffer);
            }
        }
    }

	 
}


AudioProcessorEditor* WhiteningNode::createEditor() {
    editor = new ProcessorEditor(this,true);
    return editor;
}

void WhiteningNode::resetBuffer() {
    resizeBuffer();
}


void WhiteningNode::setBufferLength(double length) {
    bufferLength = length;
    resizeBuffer();
}