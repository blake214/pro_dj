#include <JuceHeader.h>
#include "DeckGUI.h"

DeckGUI::DeckGUI(
    std::shared_ptr<DJAudioPlayer> _audioPlayer, 
    juce::AudioFormatManager& _formatManager,
    juce::AudioThumbnailCache& _cache,
    std::pair<juce::String, juce::Colour> _colour)

    :audioPlayer(_audioPlayer),
    deckColour(_colour.second),
    deckName(_colour.first),
    waveformDisplay(_formatManager, _cache, deckColour, audioPlayer),
    buttonBarHeight(30),
    waveformHeight(60),
    deckSlidersHeight(100),
    deckHeight(buttonBarHeight + waveformHeight + deckSlidersHeight)
{
    addAndMakeVisible(&deckSliders);
    addAndMakeVisible(&buttonBar);
    addAndMakeVisible(&waveformDisplay);

    // Here we need to start our thread
    startTimer(200);
}
 
DeckGUI::~DeckGUI(){
    audioPlayer.reset();
}

void DeckGUI::paint (juce::Graphics& g){
    g.fillAll (juce::Colours::black);
    g.drawRect (getLocalBounds(), 0.0f);
}

void DeckGUI::resized(){
    double deckPosY{0.0f};
    deckSliders.setBounds(deckPosY,0,getWidth(),deckSlidersHeight);
    deckPosY += deckSlidersHeight;
    waveformDisplay.setBounds(0,deckPosY,getWidth(), waveformHeight);
    deckPosY += waveformHeight;
    buttonBar.setBounds(0,deckPosY,getWidth(), buttonBarHeight);
}

void DeckGUI::timerCallback(){
    waveformDisplay.setPositionRelative(audioPlayer->getPositionRelative());
}

void DeckGUI::loadURL(juce::URL url){
    waveformDisplay.loadURL(url);
}

double DeckGUI::getDeckHeight(){
    return deckHeight;
}