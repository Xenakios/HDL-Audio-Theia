#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "HDLBgImageParameter.h"
#include "HDLWaveshaperSlider.h"
#include "HDLMixSlider.h"
#include "HDLLabelLook.h"
#include "HDLBypassButton.h"
#include "HDLGrid.h"

class HdlAudioTheiaAudioProcessorEditor  : public AudioProcessorEditor, public Timer{
public:
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    HdlAudioTheiaAudioProcessorEditor (HdlAudioTheiaAudioProcessor&, AudioProcessorValueTreeState&);
    ~HdlAudioTheiaAudioProcessorEditor();

    void paint (Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    // paintings
    HDLBgImageParameter bgImage;
    double lastMixValue{ 0. };

    // look and feel, grid:
    HDLGrid grid;
    HDLWaveshaperSlider driveLookAndFeel;
    HDLSpirographSlider mixLookAndFeel;
    HDLLunaLabel labelLookAndFeel;
    HDLBypassButton bypassLookAndFeel;

    // title:
    Label companyLabel;
    Label titleLabel;

    // parameters:
    AudioProcessorValueTreeState& valueTreeState;
    ToggleButton bypassParam;
    std::unique_ptr<ButtonAttachment> bypassAttachment;
    Slider driveParam, mixParam;
    std::unique_ptr<SliderAttachment> driveAttachment, mixAttachment;

    // processor:
    HdlAudioTheiaAudioProcessor& processor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HdlAudioTheiaAudioProcessorEditor)
};
