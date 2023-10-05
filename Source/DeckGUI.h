/**Personal Notes
 * Commenting seems fine
 * Structure seems fine
 * File is complete
*/
#pragma once
#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "DeckSliders.h"
#include "WaveformDisplay.h"
#include "DeckGUIButtonBar.h"

//==============================================================================  
/**DeckGUI
 * This is a component class for the deck GUIs for each deck
 * This component holds all the controls for the audio player
*/
class DeckGUI  :    public juce::Component,
                    public juce::Timer
{
public:
    /** Component functions */
    /**Here we take arguments in a constructor
     * A pointer to the audio player
     * A reference to the format managet
     * A reference to the cache
     * A colour name and colour
    */
    DeckGUI(
        std::shared_ptr<DJAudioPlayer> _audioPlayer, 
        juce::AudioFormatManager& _formatManager,
        juce::AudioThumbnailCache& _cache,
        std::pair<juce::String, juce::Colour> _colour);
    ~DeckGUI() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    // This function loads the url to the waveform display
    void loadURL(juce::URL url);
    // This function returns the deck height which is assigned in this class
    double getDeckHeight();

    /** Component variables */
    juce::Colour deckColour;
    juce::String deckName;
private:
    /** Component functions */
    void timerCallback()override;
    /** Component variables */
    std::shared_ptr<DJAudioPlayer> audioPlayer;
    // This creates an instance of a deck slider
    DeckSliders deckSliders{audioPlayer, deckColour};
    // This creates an instance of the deck control bar
    DeckGUIButtonBar buttonBar{audioPlayer,deckColour};
    // This creates an instance of a waveform sidplay
    WaveformDisplay waveformDisplay;
    // Below variables hold the sizes of the components
    const int buttonBarHeight, waveformHeight, deckSlidersHeight;
    double deckHeight;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};
//==============================================================================  
