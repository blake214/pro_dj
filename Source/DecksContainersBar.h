/**Personal Notes
 * Commenting seems fine
 * Structure seems fine
 * This file is complete
*/
#pragma once
#include <JuceHeader.h>

//==============================================================================  
/**DecksContainersBar
 * This is a component class for controlling each deck
 * This component controls deleting and minimizing deckGUIs
*/
class DecksContainersBar  : public juce::Component
{
public:
    /** Component functions */
    DecksContainersBar(std::pair<juce::String, juce::Colour> _colour);
    ~DecksContainersBar() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    // This method updates the track name displayed on the bar
    void updateTrackName(juce::String s);
    
    /** Component variables */
    // Below buttons are public for parents to add listeners to
    juce::TextButton buttonDeleteDeck, buttonToggle;
private:
    /** Component variables */
    juce::String trackName;
    juce::Colour deckColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DecksContainersBar)
};
//==============================================================================