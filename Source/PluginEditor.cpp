#include "PluginProcessor.h"
#include "PluginEditor.h"

HdlAudioTheiaAudioProcessorEditor::HdlAudioTheiaAudioProcessorEditor (
    HdlAudioTheiaAudioProcessor& p)
    : AudioProcessorEditor(p), processor(p){
    
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

    bypassAttachment = std::make_unique<ButtonAttachment>(processor.valueTreeState, "bypass", bypassParam);
    driveAttachment = std::make_unique<SliderAttachment>(processor.valueTreeState, "drive", driveParam);
    mixAttachment = std::make_unique<SliderAttachment>(processor.valueTreeState, "mix", mixParam);

    bgImage.resize(int(1.f / processor.mixInterval));

    float x = 1.f;
    float y = .47f;
    auto screenArea = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
    float scale = float(screenArea.getWidth()) * .3f;
    float fps = 18;

    processor.gainMeter.setImagesCount(12);
    processor.gainMeter.setRelease(.2f);
    processor.gainMeter.setFPS(24);
    processor.gainMeterSC.setImagesCount(12);
    processor.gainMeterSC.setRelease(.2f);
    processor.gainMeterSC.setFPS(24);

    setSize(int(x * scale), int(y * scale));
    startTimer(1000.f / float(fps));
}

HdlAudioTheiaAudioProcessorEditor::~HdlAudioTheiaAudioProcessorEditor(){}

void HdlAudioTheiaAudioProcessorEditor::timerCallback() {
    if (mixParam.getValue() != lastMixValue) {
        lastMixValue = mixParam.getValue();
        bgImage.setIndex(float(mixParam.getValue()));
        repaint();
    }
    else if (processor.gainMeter.shouldRepaint() || processor.gainMeterSC.shouldRepaint())
        repaint();
}

void HdlAudioTheiaAudioProcessorEditor::paint (Graphics& g){
    g.fillAll(Colours::black);
    g.setImageResamplingQuality(Graphics::lowResamplingQuality);
    bgImage.draw(g, getLocalBounds().toFloat());
    processor.gainMeter.draw(g, grid.getRect(1.f, 0.f, 1.f, 1.f));
    processor.gainMeterSC.draw(g, grid.getRect(2.f, 0.f, 1.f, 1.f));
    //grid.draw(g, Colours::darkgrey);
}

void HdlAudioTheiaAudioProcessorEditor::resized(){
    Random random;
    auto mainColour = Colour(0xffff0000).withRotatedHue(random.nextFloat());
    bypassLookAndFeel.setColour(420, mainColour);
    labelLookAndFeel.setColour(420, mainColour);

    driveLookAndFeel.makeWaveshaperImages(mainColour, int(1.f / processor.driveInterval));
    
    processor.gainMeter.setImageBounds(16);
    processor.gainMeterSC.setImageBounds(16);
    processor.gainMeter.setImages(.75f, 1.f, mainColour, 0.f, 1.f, 0.f, .9f);
    processor.gainMeterSC.setImages(.75f, 1.f, mainColour, 0.f, 1.f, 0.f, .9f);

    auto smallerBounds = Rectangle<int>(getX(), getY(), getWidth() / 4 * 3, getHeight() / 4 * 3);

    bgImage.setBounds(getLocalBounds());
    bgImage.setImage(128, 3.f, 7, mainColour);

    grid.setGrid(16.f, 8.f, getLocalBounds());

    bypassParam.setBounds(grid.getRect(0.f, 0.f, 1.f, 1.f).toNearestInt());
    companyLabel.setBounds(grid.getRect(.25f, 1.f, 5.f, 2.f).toNearestInt());
    titleLabel.setBounds(grid.getRect(4.f, 1.f, 6.f, 3.f).toNearestInt());
    
    driveParam.setBounds(grid.getRect(1.5f, 3.5f, 4.f, 4.f).toNearestInt());
    mixParam.setBounds(grid.getRect(7.f, 1.f, 10.f, 10.f).toNearestInt());
}