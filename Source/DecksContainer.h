/**Personal Notes
 * Commenting seems fine
 * Structure seems fine
 * This file is complete
*/
#pragma once
#include <JuceHeader.h>
#include "DeckGUI.h"
#include "DecksContainersBar.h"
#include "DJAudioPlayer.h"
#include "PlayListEntry.h"

//==============================================================================  
/**DeckGUI
 * This is a component that creates all the elements for the decks
 * This component creates the deck GUI, AudioPlayer, GUIbar
*/
class DecksContainer  : public juce::Component,
                        public juce::Button::Listener
{
public:
    /** Component functions */
    /**Here we take arguments in a constructor
     * A reference to the format manager
     * A broadcaster to listen for resizing
     * A broadcaster to listen for deck deletion
    */
    DecksContainer(
        juce::ChangeBroadcaster *_resizeBroadcastListener, 
        juce::AudioFormatManager& _formatManager, 
        juce::ChangeBroadcaster *_clearBufferInputsBroadcastListener);
    ~DecksContainer() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    // This method returns the current height of the container
    double GetDeckContainerHeight();
    // This method loads a url to all elements of the deck, audioPlayer,deckBars, deckGUIs. for a particular deck ID
    void loadURL(int deck,PlayListEntry* _entry);
    // This method updates the original sample rate for the filters to know
    void updateSampleRate(double _rate);

    /** Component variables */
    // This variable is a mapping of the audio player with a deck ID
    std::map<int, std::shared_ptr<DJAudioPlayer>> audioPlayers;
    // This variable is a mapping of the deckDUIs with a deck ID
    std::map<int, std::shared_ptr<DeckGUI>> decks;
private:
    /** Component functions */
    // This method creates an instance of a deckGUI, audioplayer and deckContainerBar, and pushes these to their mappings
    void AddDeck(int deckKey);
    // This method removes all instances of a deck, the deckGUI, audioPlayer and deckCOntainerBar from the mappings
    void RemoveDeck(int _deckKey);
    void buttonClicked(juce::Button * button)override;
    // This method sets the deckContainerHeight variable
    void setDeckContainerHeight(double h);

    /** Component variables */
    // This variable is a mapping of the deckContainerBars with a deck ID
    std::map<int , std::unique_ptr<DecksContainersBar>> deckBars;
    // The below variable holds the thumbCache that the waveform would use
    juce::AudioThumbnailCache thumbCache;
    // This is a reference to the parsed format manager
    juce::AudioFormatManager& formatManager;
    // The below are the parsed broadcasters
    juce::ChangeBroadcaster *resizeBroadcastListener;
    juce::ChangeBroadcaster *clearBufferInputsBroadcastListener;
    // The below variable holds the current height of the component
    double deckContainerHeight;
    // The below variable holds the max amount of decks we will be using
    int maxDecks;
    // The below variable which holds the colour mappings for each ID
    std::vector<std::pair<juce::String, juce::Colour>> deckColours;
    juce::TextButton buttonAddDeck;
    /** This variable holds the original sample rate which gets updated from the main component 
     * We are storing this to parse to the DJAudioPlayer
     * We parsing this for the filters to know the sample rate
    */
    double original_sampleRate;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DecksContainer)
};
//==============================================================================  