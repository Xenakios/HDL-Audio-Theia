#include "PluginProcessor.h"
#include "PluginEditor.h"

HdlAudioTheiaAudioProcessorEditor::HdlAudioTheiaAudioProcessorEditor (
    HdlAudioTheiaAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(p), processor(p), valueTreeState(vts){
    
    auto companyName = String(JucePlugin_Manufacturer);
    companyLabel.setText(companyName, NotificationType::dontSendNotification);
    String pluginName = String(JucePlugin_Name).substring(companyName.length() + 1);
    titleLabel.setText(pluginName, NotificationType::dontSendNotification);

    addAndMakeVisible(companyLabel);
    addAndMakeVisible(titleLabel);

    driveParam.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    driveParam.setTextBoxStyle(Slider::TextBoxBelow, false, 0, 0);
    mixParam.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mixParam.setTextBoxStyle(Slider::TextBoxBelow, false, 0, 0);
    
    companyLabel.setLookAndFeel(&labelLookAndFeel);
    titleLabel.setLookAndFeel(&labelLookAndFeel);
    bypassParam.setLookAndFeel(&bypassLookAndFeel);
    driveParam.setLookAndFeel(&driveLookAndFeel);
    mixParam.setLookAndFeel(&mixLookAndFeel);

    addAndMakeVisible(bypassParam);
    addAndMakeVisible(driveParam);
    addAndMakeVisible(mixParam);

    bypassAttachment.reset(new ButtonAttachment(valueTreeState, "bypass", bypassParam));
    driveAttachment.reset(new SliderAttachment(valueTreeState, "drive", driveParam));
    mixAttachment.reset(new SliderAttachment(valueTreeState, "mix", mixParam));

    float x = 1.f;
    float y = .5f;
    float scale = 500.f;
    setSize (int(x * scale), int(y * scale));
}

HdlAudioTheiaAudioProcessorEditor::~HdlAudioTheiaAudioProcessorEditor(){}

void HdlAudioTheiaAudioProcessorEditor::paint (Graphics& g){
    g.fillAll(Colours::black);
    moon.draw(g);
    //grid.draw(g, Colours::darkgrey);
}

void HdlAudioTheiaAudioProcessorEditor::resized(){
    Random random;
    auto mainColour = Colour(0xffff0000).withRotatedHue(random.nextFloat());
    bypassLookAndFeel.setColour(420, mainColour);
    labelLookAndFeel.setColour(420, mainColour);
    driveLookAndFeel.makeWaveshaperImages(mainColour);

    moon.setBounds(getLocalBounds());
    moon.setImage();
    grid.setGrid(16.f, 8.f, getLocalBounds());

    bypassParam.setBounds(grid.getRect(0.f, 0.f, 1.f, 1.f).toNearestInt());
    
    companyLabel.setBounds(grid.getRect(.25f, 1.f, 5.f, 2.f).toNearestInt());
    titleLabel.setBounds(grid.getRect(4.f, 1.f, 6.f, 3.f).toNearestInt());
    
    driveParam.setBounds(grid.getRect(1.5f, 3.5f, 4.f, 4.f).toNearestInt());
    mixParam.setBounds(grid.getRect(7.f, 1.f, 10.f, 10.f).toNearestInt());
}