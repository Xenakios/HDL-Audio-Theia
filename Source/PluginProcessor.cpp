#include "PluginProcessor.h"
#include "PluginEditor.h"

HdlAudioTheiaAudioProcessor::HdlAudioTheiaAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                       .withInput("Sidechain", AudioChannelSet::stereo(), false)
                       ), parameters(*this, nullptr, Identifier("params"),
                           {
                               std::make_unique<AudioParameterBool>("bypass",
                                                                      "Bypass",
                                                                      false),
                               std::make_unique<AudioParameterFloat>("drive",
                                                                   "Drive",
                                                                   0.f,
                                                                   1.f,
                                                                   .5f),
                               std::make_unique<AudioParameterFloat>("mix",
                                                                   "Mix",
                                                                   0.f,
                                                                   1.f,
                                                                   0.f)
                           })
#endif
{
    bypassParam = parameters.getRawParameterValue("bypass");
    driveParam = parameters.getRawParameterValue("drive");
    mixParam = parameters.getRawParameterValue("mix");
}

HdlAudioTheiaAudioProcessor::~HdlAudioTheiaAudioProcessor(){ }

const String HdlAudioTheiaAudioProcessor::getName() const{ return JucePlugin_Name; }

bool HdlAudioTheiaAudioProcessor::acceptsMidi() const{ return false; }

bool HdlAudioTheiaAudioProcessor::producesMidi() const{ return false; }

bool HdlAudioTheiaAudioProcessor::isMidiEffect() const{ return false; }

double HdlAudioTheiaAudioProcessor::getTailLengthSeconds() const{ return 0.0; }

int HdlAudioTheiaAudioProcessor::getNumPrograms(){ return 1; }

int HdlAudioTheiaAudioProcessor::getCurrentProgram(){ return 0; }

void HdlAudioTheiaAudioProcessor::setCurrentProgram (int index){}

const String HdlAudioTheiaAudioProcessor::getProgramName(int index) { return {}; }

void HdlAudioTheiaAudioProcessor::changeProgramName (int index, const String& newName){}

void HdlAudioTheiaAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock){}

void HdlAudioTheiaAudioProcessor::releaseResources(){}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HdlAudioTheiaAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const{
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}
#endif

void HdlAudioTheiaAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages){
    ScopedNoDenormals noDenormals;
    
    auto mainBus = getBusBuffer(buffer, true, 0);
    auto scBus = getBusBuffer(buffer, true, 1);

    if (getBus(true, 1)->isEnabled()) {
        dsp.setParameters(*bypassParam, *driveParam, *mixParam);

        for (int s = 0; s < mainBus.getNumSamples(); ++s)
            for (int ch = 0; ch < mainBus.getNumChannels(); ++ch) {
                auto& sample = *mainBus.getWritePointer(ch, s);
                auto& scSample = *scBus.getReadPointer(ch, s);

                dsp.process(sample, scSample);
            }
        }
    }

bool HdlAudioTheiaAudioProcessor::hasEditor() const{ return true; }

AudioProcessorEditor* HdlAudioTheiaAudioProcessor::createEditor(){
    return new HdlAudioTheiaAudioProcessorEditor (*this, parameters);
}

void HdlAudioTheiaAudioProcessor::getStateInformation (MemoryBlock& destData){
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void HdlAudioTheiaAudioProcessor::setStateInformation (const void* data, int sizeInBytes){
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(ValueTree::fromXml(*xmlState));
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter(){ return new HdlAudioTheiaAudioProcessor(); }