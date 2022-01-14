//This prevents include loops. We recommend changing the macro to a name suitable for your plugin
#ifndef WHITENINGNODE_H_DEFINED
#define WHITENINGNODE_H_DEFINED

#include <ProcessorHeaders.h>
#include "Eigen/Dense"
#include "ProcessorEditor.h"

using namespace std;
using Eigen::MatrixXf;

class WhiteningNode : public GenericProcessor
{
public:
	/** The class constructor, used to initialize any members. */
	WhiteningNode();

	/** The class destructor, used to deallocate memory */
	~WhiteningNode();

	/** Indicates if the processor has a custom editor. Defaults to false */
	bool hasEditor() const { return true; }

	/** If the processor has a custom editor, this method must be defined to instantiate it. */
	AudioProcessorEditor* createEditor() override;

	/** Optional method that informs the GUI if the processor is ready to function. If false acquisition cannot start. Defaults to true */
	//bool isReady();

	/** Defines the functionality of the processor.

	The process method is called every time a new data buffer is available.

	Processors can either use this method to add new data, manipulate existing
	data, or send data to an external target (such as a display or other hardware).

	Continuous signals arrive in the "buffer" variable, event data (such as TTLs
	and spikes) is contained in the "events" variable.
	*/
	void process(AudioSampleBuffer& buffer) override;

	/** Handles events received by the processor

	Called automatically for each received event whenever checkForEvents() is called from process()		
	*/
	//void handleEvent(const EventChannel* eventInfo, const MidiMessage& event, int samplePosition) override;

	/** Handles spikes received by the processor

	Called automatically for each received event whenever checkForEvents(true) is called from process()
	*/
	//void handleSpike(const SpikeChannel* spikeInfo, const MidiMessage& event, int samplePosition) override;

	/** The method that standard controls on the editor will call.
	It is recommended that any variables used by the "process" function
	are modified only through this method while data acquisition is active. */
	//void setParameter(int parameterIndex, float newValue) override;

	/** Saving custom settings to XML. */
	//void saveCustomParametersToXml(XmlElement* parentElement) override;

	/** Load custom settings from XML*/
	//void loadCustomParametersFromXml() override;

	/** Optional method called every time the signal chain is refreshed or changed in any way.

	Allows the processor to handle variations in the channel configuration or any other parameter
	passed down the signal chain. The processor can also modify here the settings structure, which contains
	information regarding the input and output channels as well as other signal related parameters. Said
	structure shouldn't be manipulated outside of this method.

	*/
	void updateSettings() override;
	int getNumSubprocessorChannels();
	float getSubprocessorSampleRate(uint32 subprocId);
	uint32 getDataSubprocId(int chan) const;
	void setSubprocessor(uint32 sp);
	uint32 getSubprocessor() const;
	void resetBuffer();
	void setBufferLength(double bufferLength);
	void setApplyWhitening(bool isApply) { isApplyWhitening = isApply; };


private:
	// use to calculating the whitening matrixm, it is a vector of vector, each channel has its own 
	std::shared_ptr<AudioSampleBuffer> whiteningBuffers; 
	std::shared_ptr<AudioSampleBuffer> displayBuffers; //a smart pointer to manage the buffer object

	AbstractFifo abstractFifo;
	float bufferLength=10; // in second

	bool resizeBuffer();

	int numSubprocessors;
	uint32 subprocessorToDraw;
	SortedSet<uint32> allSubprocessors;
	std::map<uint32, int> numChannelsInSubprocessor;
	std::map<uint32, float> subprocessorSampleRate;
	float samplingRate;
	int numChannels;

	std::vector<int> displayBufferIndices; //index for each channel
	Array<int> channelIndices;
	Array<uint32> eventSourceNodes;
	static uint32 getChannelSourceId(const InfoObjectCommon* chan);
	CriticalSection displayMutex;

	bool isBufferReady = false; //whether the buffer is ready to calculate whitening matrix
	int readyChannel = 0; //how many channel has is full in the buffer

	MatrixXf m_W;
	bool m_whiteningMatrixReady = false; //whether whitening matrix is ready
	void calculateWhiteningMatrix();
	void applyWhitening(AudioSampleBuffer& buffer);
	bool isApplyWhitening = true; //whether to apply whitening online
	bool isNeedWhiteningUpdate = true; //whether to recalculate the whitening matrix
	int numDataChannel = 0;

};

#endif