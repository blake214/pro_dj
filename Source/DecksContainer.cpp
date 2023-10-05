#include <JuceHeader.h>
#include "DecksContainer.h"

DecksContainer::DecksContainer(
    juce::ChangeBroadcaster *_resizeBroadcastListener, 
    juce::AudioFormatManager& _formatManager, 
    juce::ChangeBroadcaster *_clearBufferInputsBroadcastListener)  

    :deckContainerHeight(30.0f),
    maxDecks(4),
    buttonAddDeck("Add Deck"),
    resizeBroadcastListener(_resizeBroadcastListener),
    clearBufferInputsBroadcastListener(_clearBufferInputsBroadcastListener),
    formatManager(_formatManager),
    thumbCache(100)
{
    addAndMakeVisible(buttonAddDeck);
    buttonAddDeck.addListener(this);

    // Below we initialising our deckColours
    deckColours.push_back(std::make_pair("Blue", juce::Colour(0,150,150)));
    deckColours.push_back(std::make_pair("Green", juce::Colour(0,150,50)));
    deckColours.push_back(std::make_pair("Purple", juce::Colour(150,50,150)));
    deckColours.push_back(std::make_pair("Red", juce::Colour(150,50,50)));
}

DecksContainer::~DecksContainer(){
}

void DecksContainer::paint (juce::Graphics& g){
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void DecksContainer::resized(){
    // Here we initialise the starting position of of dynamic variable
    double deckPosY{0.0f};
    double buttonAddDeckHeight{30.0f};
    // Here we reset our containers height (for when we resize)
    double newHeight = 0.0f;
    
    // Here we iterate over the decks and set bounds for each deck bar, we also update our dynamic variable
    for (const auto& bar : deckBars){
        const int barHeight{30};
        bar.second->setBounds(0,deckPosY,getWidth(),barHeight);
        deckPosY += barHeight;
        // Here we updating our containers total height
        newHeight += barHeight;

        // Here we iterate over the decks and set bounds for each deck, we also update our dynamic variable
        for (const auto& deck : decks){
            // Here we checking if the deck belongs to the deck bar, so the decks go between the bars
            if (bar.first != deck.first) continue;
            // Here we initialise the deck height to 0, this will 'hide' the deck
            double deckHeight = 0;
            // Only if the deckbar has been toggled to true the deck will be set to its height
            if (deckBars[deck.first] -> buttonToggle.getToggleState()){
                deckHeight = deck.second->getDeckHeight();
            }
            deck.second -> setBounds(0,deckPosY,getWidth(),deckHeight);
            deckPosY += deckHeight;
            // Here we updating our containers total height
            newHeight += deckHeight;
        }
    }

    buttonAddDeck.setBounds(0,getHeight()-buttonAddDeckHeight,getWidth(),buttonAddDeckHeight);
    // Here we update our containers height
    newHeight += buttonAddDeckHeight;
    setDeckContainerHeight(newHeight);
}

void DecksContainer::AddDeck(int deckKey){
    // Here we make an audio player to add to the map
    std::shared_ptr<DJAudioPlayer> newPlayer = std::make_shared<DJAudioPlayer>(formatManager, original_sampleRate);
    // Here we making a deck GUI for the audio player
    std::shared_ptr<DeckGUI> newDeck = std::make_shared<DeckGUI>(newPlayer, formatManager, thumbCache, deckColours[deckKey]);
    // Here we making a deck bar for the Deck GUI
    std::unique_ptr<DecksContainersBar> newDeckBar = std::make_unique<DecksContainersBar>(deckColours[deckKey]);

    // Here we adding and making the deck bar and deck visiable
    addAndMakeVisible(*newDeckBar);
    addAndMakeVisible(*newDeck);
    // Here we adding listeners to the deckbar to we can hear from this class, for the delete and toggle button
    newDeckBar-> buttonDeleteDeck.addListener(this);
    newDeckBar-> buttonToggle.addListener(this);

    // Here we inserting the audioplayer pointer into the mapping
    audioPlayers.insert({deckKey,std::move(newPlayer)});
    // Here we inserting the deck GUI pointer into the mapping
    decks.insert({deckKey,std::move(newDeck)});
    // Here we inserting the deck bar pointer into the mapping
    deckBars.insert({deckKey,std::move(newDeckBar)});
}

void DecksContainer::buttonClicked(juce::Button * button){
    if(button == &buttonAddDeck){
        // Here we just return if the max number of decks are present
        if (decks.size() >= maxDecks) return;

        // Here we find the next availiable key to add our deck to
        int deckKey = 0;
        for (const auto& deck: decks){
            if (deck.first == deckKey) deckKey++;
            else {
                deckKey = deckKey;
                break;
            }
        }
        // We add the deck, deck bar, audioplayer to the key in the mappings
        AddDeck(deckKey);
        resized();
        // We broadcast the change, which will tell the parent there is a resize
        resizeBroadcastListener->sendChangeMessage();
    }
    // Here we iterate over each deckContainerBar
    for (const auto& bar : deckBars){
        // If the delete button is pressed
        if (button == &bar.second -> buttonDeleteDeck){
            // Here we broadcasting to clear the mixers inputs before we delete the audio player
            clearBufferInputsBroadcastListener->sendChangeMessage();

            // Here we remove the deck
            RemoveDeck(bar.first);

            // We call resize
            resized();
            // We broadcast the change to the main app to they can call resize
            resizeBroadcastListener->sendChangeMessage();
        }
        // If the toggle button is pressed
        if (button == &bar.second -> buttonToggle){
            // Here we checking within the bar to check what state the button is now
            if (bar.second -> buttonToggle.getToggleState()){
                // If true we display the deck
                addAndMakeVisible(*decks[bar.first]);
            } else {
                // Else we hide it
                decks[bar.first] -> setVisible(false);
            }
            // We call resize
            resized();
            // We broadcast the change, which will tell the main class to resize too
            resizeBroadcastListener->sendChangeMessage();
        }
    }
}

void DecksContainer::RemoveDeck(int _deckKey){
    // We remove the deck bar pointer
    deckBars[_deckKey].reset();
    // We reset the deck gui pointer
    decks[_deckKey].reset();
    // We reset the audio player pointer
    audioPlayers[_deckKey].reset();
    // We remove the mappings
    deckBars.erase(_deckKey);
    decks.erase(_deckKey);
    audioPlayers.erase(_deckKey);
}

double DecksContainer::GetDeckContainerHeight(){
    return deckContainerHeight;
}

void DecksContainer::loadURL(int deck, PlayListEntry* _entry){
    audioPlayers[deck] -> loadURL(_entry ->trackURL);
    deckBars[deck] ->updateTrackName(_entry ->trackName);
    decks[deck] -> loadURL(_entry ->trackURL);
}

void DecksContainer::updateSampleRate(double _rate){
    original_sampleRate = _rate;
}

void DecksContainer::setDeckContainerHeight(double h){
    deckContainerHeight = h;
}