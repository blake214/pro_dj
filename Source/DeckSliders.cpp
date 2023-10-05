#include <JuceHeader.h>
#include "DeckSliders.h"

DeckSliders::DeckSliders(std::shared_ptr<DJAudioPlayer> _audioPlayer, juce::Colour _deckColour)
    :audioPlayer(_audioPlayer),
    deckColour(_deckColour),
    volumeSlider(juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextEntryBoxPosition::NoTextBox),
    speedSlider(juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextEntryBoxPosition::NoTextBox),
    lpfSlider(juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextEntryBoxPosition::NoTextBox),
    hpfSlider(juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextEntryBoxPosition::NoTextBox)
{
    addAndMakeVisible(&volumeSlider);
    addAndMakeVisible(&speedSlider);
    addAndMakeVisible(&lpfSlider);
    addAndMakeVisible(&hpfSlider);

    /**Here we setting up the sliders
     * Creating popup texts
     * Setting the default values
     * Changing the audioPlayer to be synced with the sliders default values
     * Adding listeners
    */
    volumeSlider.setPopupDisplayEnabled (true, true, this);
    volumeSlider.setTextValueSuffix (" Volume");
    volumeSlider.addListener(this);
    volumeSlider.setValue(5.0f);
    audioPlayer->setVolume(volumeSlider.getValue());

    speedSlider.setPopupDisplayEnabled (true, true, this);
    speedSlider.setTextValueSuffix (" Speed");
    speedSlider.addListener(this);
    speedSlider.setValue(1.0f);
    audioPlayer->setSpeed(speedSlider.getValue());

    lpfSlider.setPopupDisplayEnabled (true, true, this);
    lpfSlider.setTextValueSuffix (" Lows");
    lpfSlider.addListener(this);
    lpfSlider.setValue(10.0f);
    audioPlayer->setLowPassFreqRelative(lpfSlider.getValue());

    hpfSlider.setPopupDisplayEnabled (true, true, this);
    hpfSlider.setTextValueSuffix (" Highs");
    hpfSlider.addListener(this);
    hpfSlider.setValue(0.0f);
    audioPlayer->setHighPassFreqRelative(hpfSlider.getValue());
}

DeckSliders::~DeckSliders(){
    audioPlayer.reset();
}

void DeckSliders::paint (juce::Graphics& g){
    g.fillAll (deckColour.withAlpha(0.8f)); 
}

void DeckSliders::resized(){
    const int sliderCount{4};
    double xPos{0.0f};
    const double xIntervals = getWidth() / (sliderCount + 1);
    const int sliderWidth = 20;
    const int space{10};
    const double sliderHeight = getHeight() - (space * 2);

    volumeSlider.setBounds(xPos + xIntervals - (sliderWidth/2), space, sliderWidth, sliderHeight);
    xPos += xIntervals;
    speedSlider.setBounds(xPos + xIntervals - (sliderWidth/2), space, sliderWidth, sliderHeight);
    xPos += xIntervals;
    lpfSlider.setBounds(xPos + xIntervals - (sliderWidth/2), space, sliderWidth, sliderHeight);
    xPos += xIntervals;
    hpfSlider.setBounds(xPos + xIntervals - (sliderWidth/2), space, sliderWidth, sliderHeight);
}

void DeckSliders::sliderValueChanged(juce::Slider* slider) {
    if (slider == &volumeSlider) return audioPlayer->setVolume(slider->getValue());
    if (slider == &speedSlider) return audioPlayer->setSpeed(slider->getValue());
    if (slider == &lpfSlider) return audioPlayer->setLowPassFreqRelative(slider->getValue());
    if (slider == &hpfSlider) return audioPlayer->setHighPassFreqRelative(slider->getValue());
}