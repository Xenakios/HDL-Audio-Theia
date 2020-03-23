#include "PluginProcessor.h"
#include "PluginEditor.h"

HdlAudioTheiaAudioProcessor::HdlAudioTheiaAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
        .withInput("Input", AudioChannelSet::stereo(), true)
        .withOutput("Output", AudioChannelSet::stereo(), true)
        .withInput("Sidechain", AudioChannelSet::stereo(), false)
    ), valueTreeState(*this, nullptr, "parameters", createParameters())
#endif
{
    valueTreeState.state.addListener(this);
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

void HdlAudioTheiaAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock){
    gainMeter.setSampleRate(sampleRate);
    gainMeterSC.setSampleRate(sampleRate);
}

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

    if (valueTreeShouldUpdate) {
        bypassParam =
            valueTreeState.getRawParameterValue("bypass")->load();
        driveParam =
            valueTreeState.getRawParameterValue("drive")->load();
        mixParam =
            valueTreeState.getRawParameterValue("mix")->load();
    }

    if (getBus(true, 1)->isEnabled()) {
        hdldsp.setParameters(bypassParam, driveParam, mixParam);

        for (int s = 0; s < mainBus.getNumSamples(); ++s)
            for (int ch = 0; ch < mainBus.getNumChannels(); ++ch) {
                auto& sample = *mainBus.getWritePointer(ch, s);
                auto& scSample = *scBus.getReadPointer(ch, s);

                gainMeter.setSample(sample);
                gainMeterSC.setSample(scSample);

                hdldsp.process(sample, scSample);
            }
    } else
        for (int s = 0; s < mainBus.getNumSamples(); ++s)
            for (int ch = 0; ch < mainBus.getNumChannels(); ++ch) {
                auto& sample = *mainBus.getWritePointer(ch, s);
                gainMeter.setSample(sample);
                gainMeter.setSample(0.f);
            }
}

bool HdlAudioTheiaAudioProcessor::hasEditor() const{ return true; }

AudioProcessorEditor* HdlAudioTheiaAudioProcessor::createEditor(){
    return new HdlAudioTheiaAudioProcessorEditor (*this);
}

void HdlAudioTheiaAudioProcessor::getStateInformation (MemoryBlock& destData){
    auto state = valueTreeState.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void HdlAudioTheiaAudioProcessor::setStateInformation (const void* data, int sizeInBytes){
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(valueTreeState.state.getType()))
            valueTreeState.replaceState(ValueTree::fromXml(*xmlState));
}

AudioProcessorValueTreeState::ParameterLayout HdlAudioTheiaAudioProcessor::createParameters() {
    std::vector<std::unique_ptr<RangedAudioParameter>> parameters;

    driveInterval = .1f;
    mixInterval = .05f;

    parameters.push_back(std::make_unique<AudioParameterBool>(
        "bypass", "Bypass", 0
        ));
    parameters.push_back(std::make_unique<AudioParameterFloat>(
        "drive", "Drive",
        NormalisableRange<float>(0.f, 1.f, driveInterval),
        .5f
        ));
    parameters.push_back(std::make_unique<AudioParameterFloat>(
        "mix", "Mix",
        NormalisableRange<float>(0.f, 1.f, mixInterval),
        .5f
        ));
    return{ parameters.begin(), parameters.end() };
}

void HdlAudioTheiaAudioProcessor::valueTreePropertyChanged(ValueTree& tree, const Identifier& property) {
    valueTreeShouldUpdate.store(true);
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter(){ return new HdlAudioTheiaAudioProcessor(); }