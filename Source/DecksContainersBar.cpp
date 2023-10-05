#include <JuceHeader.h>
#include "DecksContainersBar.h"

DecksContainersBar::DecksContainersBar(std::pair<juce::String, juce::Colour> _deckColour)
    :deckColour(_deckColour.second),
    trackName("No track loaded..."),
    buttonDeleteDeck("Delete Deck")
{
    addAndMakeVisible(buttonToggle);
    addAndMakeVisible(buttonDeleteDeck);

    // Below we adding style to the components
    buttonDeleteDeck.setColour(0x1000100,deckColour.darker(1.0f));
    buttonToggle.setColour(0x1000100,deckColour.darker(1.0f));
    buttonToggle.setColour(0x1000101,deckColour.darker(1.0f));

    // Below we seting up how the toggle button works 
    buttonToggle.setButtonText("-");
    buttonToggle.setClickingTogglesState(true);
    buttonToggle.setToggleState(true,juce::NotificationType::dontSendNotification);
    buttonToggle.onClick = [this] {
        if (buttonToggle.getToggleState() == true) buttonToggle.setButtonText("-");
        else buttonToggle.setButtonText("+");
    };
}

DecksContainersBar::~DecksContainersBar(){
}

void DecksContainersBar::paint (juce::Graphics& g){
    g.fillAll (deckColour);
    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText (trackName, getLocalBounds(),juce::Justification::centred, true);
}

void DecksContainersBar::resized(){
    const int bar_size{getHeight()};
    buttonToggle.setBounds(getWidth() - bar_size, 0, bar_size, bar_size);
    buttonDeleteDeck.setBounds(0, 0, 100, bar_size);
}

void DecksContainersBar::updateTrackName(juce::String s){
    trackName = s;
    repaint();
}
