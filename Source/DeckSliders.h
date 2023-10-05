/**Personal Notes
 * Commenting seems fine
 * Structure seems fine
 * File is complete
*/
#pragma once
#include <JuceHeader.h>
#include "DJAudioPlayer.h"

//==============================================================================  
/**DeckSliders
 * This is a component class for the slider controls in the deckGUIs
 * This component holds controls to manipulate the audioPlayer
*/
class DeckSliders  :    public juce::Component, 
                        public juce::Slider::Listener
{
public:
    /** Component functions */
    /**Here we take arguments in a constructor
     * A pointer to the audio player
     * A colour
    */
    DeckSliders(std::shared_ptr<DJAudioPlayer> _audioPlayer, juce::Colour _deckColour);
    ~DeckSliders() override;
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    /** Component functions */
    void sliderValueChanged(juce::Slider*) override;

    /** Component variables */
    // Sliders for volume, speed, lowpass, highpass
    juce::Slider volumeSlider, speedSlider, lpfSlider, hpfSlider;
    std::shared_ptr<DJAudioPlayer> audioPlayer;
    juce::Colour deckColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckSliders)
};
//==============================================================================