/**Personal Notes
 * Commenting seems fine
 * Structure seems fine
 * File is complete
*/
#pragma once
#include <JuceHeader.h>
#include "DJAudioPlayer.h"

//==============================================================================  
/**DeckGUIButtonBar
 * This is the component for basic controls on the audio player
*/
class DeckGUIButtonBar  :   public juce::Component,
                            public juce::Button::Listener
{
public:
    /** Component functions */
    /**Here we take arguments in a constructor
     * A pointer to the audio player
     * A colour
    */
    DeckGUIButtonBar(std::shared_ptr<DJAudioPlayer> _audioPlayer, juce::Colour _deckColour);
    ~DeckGUIButtonBar() override;
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    /** Component functions */
    void buttonClicked(juce::Button * button)override;
    // This method updates the volume variable
    void setVolume(double v);
    /** Component variables */
    juce::Colour deckColour;
    std::shared_ptr<DJAudioPlayer> audioPlayer;
    // This variable is to store the volume before muting, so when unmute it gains the original volume
    double volume;
    juce::TextButton playButton, pauseButton, stopButton, muteToggle, loopToggle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUIButtonBar)
};
//==============================================================================