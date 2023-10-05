/**Personal Notes
 * Commenting seems fine
 * Structure seems fine
 * Mp3 not included, if want to try again, got this from a reference 
 * -> #include <juce_audio_formats/codecs/juce_MP3AudioFormat.h>
*/
#pragma once
#include <JuceHeader.h>
#include "AudioController.h"
#include "DecksContainer.h"
#include "PlaylistController.h"
#include "DJAudioPlayer.h"

//==============================================================================  
/**MainComponent
 * This is our main component of our GUI
*/
class MainComponent  :  public juce::AudioAppComponent, 
                        public juce::ChangeListener
{
public:
    /** Component functions */
    MainComponent();
    ~MainComponent() override;
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void paint (juce::Graphics& g) override;
    void resized() override;
private:
    /** Component functions */
    void changeListenerCallback (juce::ChangeBroadcaster *source) override;

    /** Component variables */
    // Below we creating a broadcaster to listen for instructions to call resize
    juce::ChangeBroadcaster resizeBroadcastListener;
    // Below we creating a broadcaster to listen for instructions to clear the buffer
    juce::ChangeBroadcaster clearBufferInputsBroadcastListener;
    /**Sample references
     * Here we storing the sample data
     * These are different from computer to computer and this seemed the best way to record them
    */
    int original_samplesPerBlockExpected;
    double original_sampleRate;
    // This variable holds format manager for the audio tools
    juce::AudioFormatManager formatManager;
    // This variable holds the audio mixer which we will play from
    juce::MixerAudioSource mixerSource;
    // This variable holds the container that will hold all the decks
    DecksContainer decksContainer{&resizeBroadcastListener, formatManager, &clearBufferInputsBroadcastListener};
    // This variable holds the audio controller, that is used to fade the audio players
    AudioController audioController{&decksContainer,&resizeBroadcastListener};
    // This variable holds the playlist component
    PlaylistController playlistController{&decksContainer, formatManager};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
//==============================================================================