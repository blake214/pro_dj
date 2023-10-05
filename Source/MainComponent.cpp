#include "MainComponent.h"

MainComponent::MainComponent()
    :original_samplesPerBlockExpected(0),
    original_sampleRate(0.0f)
{
    setSize (1200, 1000);
    //==============================================================================
    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio) && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio)){
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio, [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else setAudioChannels (0, 2);
    //==============================================================================
    // Here we adding components and setting visiable
    addAndMakeVisible(&audioController);
    addAndMakeVisible(&decksContainer);
    addAndMakeVisible(&playlistController);
    // Here we adding listeners on your broadcasters
    resizeBroadcastListener.addChangeListener(this);
    clearBufferInputsBroadcastListener.addChangeListener(this);
    // Setting up main components formatManager
    formatManager.registerBasicFormats();
}

MainComponent::~MainComponent(){
    shutdownAudio();
}

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate){
    // Here we storing the input paramiters for when we recall prepareToPlay
    original_samplesPerBlockExpected = samplesPerBlockExpected;
    original_sampleRate = sampleRate;
    // Here we updating our decks container with the original sample rate
    decksContainer.updateSampleRate(sampleRate);
    // Here we creating our mixer audio source
    mixerSource.prepareToPlay(samplesPerBlockExpected,sampleRate);
    // Here we populate our mixer with the audio players of our decks
    for (const auto& audioPlayer : decksContainer.audioPlayers){
        audioPlayer.second -> prepareToPlay(samplesPerBlockExpected, sampleRate);
        mixerSource.addInputSource(&*audioPlayer.second,false);
    }
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill){
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources(){
    // Here we release resources in each of our audioplayers
    for (const auto& audioPlayer : decksContainer.audioPlayers){
        audioPlayer.second -> releaseResources();
    }
    mixerSource.releaseResources();
}

void MainComponent::paint (juce::Graphics& g){
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized(){
    double GUIposY{0.0f};
    const double audioControllerHeight{150};
    const double decksContainerHeight{decksContainer.GetDeckContainerHeight()};
    // Here we set bounds and adjust our dynamic variables
    audioController.setBounds(0,GUIposY,getWidth(),audioControllerHeight);
    GUIposY += audioControllerHeight;
    decksContainer.setBounds(0,GUIposY,getWidth(),decksContainerHeight);
    GUIposY += decksContainerHeight;
    playlistController.setBounds(0,GUIposY,getWidth(),getHeight()-GUIposY);
}

void MainComponent::changeListenerCallback (juce::ChangeBroadcaster *source){
    // Here if there has been a change in the broadcasts we do stuff
    if(source == &resizeBroadcastListener) {
        resized();
    }
    if(source == &clearBufferInputsBroadcastListener) {
        mixerSource.removeAllInputs();
        releaseResources();
    }
    // Here we recall prepare to play which would reference the new changes
    prepareToPlay(original_samplesPerBlockExpected, original_sampleRate);
}