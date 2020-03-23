#pragma once

#include <JuceHeader.h>
#include "HDLDSP.h"
#include "HDLImagePP.h"
#include "HDLPaintings.h"
#include "HDLGainMeter.h"

class HdlAudioTheiaAudioProcessor  : public AudioProcessor,
                                public ValueTree::Listener{
public:
    HdlAudioTheiaAudioProcessor();
    ~HdlAudioTheiaAudioProcessor();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    HDLGainMeterLight gainMeter, gainMeterSC;

    // valueTreeState
    AudioProcessorValueTreeState valueTreeState;
    float driveInterval, mixInterval;
private:
    HDLDSP hdldsp;
    
    AudioProcessorValueTreeState::ParameterLayout createParameters();
    float bypassParam, driveParam, mixParam;
    void valueTreePropertyChanged(ValueTree& tree, const Identifier& property) override;
    std::atomic<bool> valueTreeShouldUpdate{ false };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HdlAudioTheiaAudioProcessor)
};