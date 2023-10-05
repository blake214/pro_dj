#include <JuceHeader.h>
#include "DeckGUIButtonBar.h"

DeckGUIButtonBar::DeckGUIButtonBar(std::shared_ptr<DJAudioPlayer> _audioPlayer, juce::Colour _deckColour)
    :audioPlayer(_audioPlayer),
    deckColour(_deckColour),
    volume(0.0f),
    playButton("PLAY"),
    pauseButton("PAUSE"),
    stopButton("STOP"),
    muteToggle("Mute"),
    loopToggle("Looping off")
{
    addAndMakeVisible(&playButton);
    addAndMakeVisible(&pauseButton);
    addAndMakeVisible(&stopButton);
    addAndMakeVisible(&muteToggle);
    addAndMakeVisible(&loopToggle);

    // Here we styling the buttons colours
    playButton.setColour(0x1000100,deckColour);
    pauseButton.setColour(0x1000100,deckColour);
    stopButton.setColour(0x1000100,deckColour);
    muteToggle.setColour(0x1000100,deckColour);
    muteToggle.setColour(0x1000101,deckColour.darker(1.0f));
    loopToggle.setColour(0x1000100,deckColour);
    loopToggle.setColour(0x1000101,deckColour.darker(1.0f));

    playButton.addListener(this);
    stopButton.addListener(this);
    pauseButton.addListener(this);
    muteToggle.addListener(this);
    loopToggle.addListener(this);

    // Assigning button states
    muteToggle.setClickingTogglesState(true);
    loopToggle.setClickingTogglesState(true);
}

DeckGUIButtonBar::~DeckGUIButtonBar(){
    audioPlayer.reset();
}

void DeckGUIButtonBar::paint (juce::Graphics& g){
    g.fillAll (deckColour.withAlpha(0.6f)); 
}

void DeckGUIButtonBar::resized(){
    const int buttonsWidth = 80;
    const int togglesWidth = 80;

    playButton.setBounds(getWidth()/2 - buttonsWidth - buttonsWidth/2,0,buttonsWidth,getHeight());
    pauseButton.setBounds(getWidth()/2 - buttonsWidth/2,0,buttonsWidth,getHeight());
    stopButton.setBounds(getWidth()/2 + buttonsWidth/2,0,buttonsWidth,getHeight());
    muteToggle.setBounds(getWidth() - togglesWidth, 0, togglesWidth, getHeight());
    loopToggle.setBounds(getWidth() - togglesWidth*2, 0, togglesWidth, getHeight());
}

void DeckGUIButtonBar::buttonClicked(juce::Button * button){
    if (button == &playButton) return audioPlayer->start();
    if (button == &stopButton) return audioPlayer->stop();
    if (button == &pauseButton) return audioPlayer->pause();
    if (button == &muteToggle){
        // Here we storing the volume to set back when we unmute
        if(muteToggle.getToggleState()){
            setVolume(audioPlayer -> getVolume());
            audioPlayer->setVolume(0.0f);
            muteToggle.setButtonText("Unmute");
        } else {
            audioPlayer->setVolume(volume);
            muteToggle.setButtonText("Mute");
        }
        return;
    }
    if (button == &loopToggle){
        audioPlayer ->  looping(loopToggle.getToggleState());
        if(loopToggle.getToggleState()){
            loopToggle.setButtonText("Looping");
        } else {
            loopToggle.setButtonText("Looping off");
        }
    }
}

void DeckGUIButtonBar::setVolume(double v){
    volume = v;
}