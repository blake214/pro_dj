#include <JuceHeader.h>
#include "WaveformDisplay.h"

WaveformDisplay::WaveformDisplay(
    juce::AudioFormatManager& formatManagerToUse, 
    juce::AudioThumbnailCache& cacheToUse, 
    juce::Colour _colour,
    std::shared_ptr<DJAudioPlayer> _audioPlayer)

    : audioThumb(1000, formatManagerToUse, cacheToUse),
    fileLoaded(false),
    position(0),
    deckColour(_colour),
    audioPlayer(_audioPlayer)
{
    audioThumb.addChangeListener(this);
    addAndMakeVisible(positionSlider);

    // Here we just adding style to our position slider
    positionSlider.setColour(0x1001700,juce::Colours::transparentWhite); //boarder
    positionSlider.setColour(0x1001310,deckColour.brighter(1.5f).withAlpha(0.5f)); //track
    positionSlider.addListener(this);
}

WaveformDisplay::~WaveformDisplay(){
    audioPlayer.reset();
}

void WaveformDisplay::paint (juce::Graphics& g){
    //Drawing the wave
    if(fileLoaded){
      g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
      g.setColour(deckColour);
      audioThumb.drawChannel(g, getLocalBounds(), 0, audioThumb.getTotalLength(), 0, 1.0f);
    } else {
      g.fillAll (deckColour.withAlpha(0.8f));
    }
}

void WaveformDisplay::resized(){
  positionSlider.setBounds(0,0,getWidth(),getHeight());
}

void WaveformDisplay::loadURL(juce::URL audioUrl){
    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new juce::URLInputSource(audioUrl));
}

void WaveformDisplay::changeListenerCallback (juce::ChangeBroadcaster *source){
    repaint();
}

void WaveformDisplay::setPositionRelative(double _position){
    if (position != _position){
      position = _position;
      // Here we setting the sliders position to the parsed position
      positionSlider.setValue(position * 10);
      repaint();
    }
}

void WaveformDisplay::sliderValueChanged (juce::Slider *slider){
    // Here we test the audioPlayers position according the the position of the slider
    if (slider == &positionSlider) audioPlayer->setPositionRelative(positionSlider.getValue());
}