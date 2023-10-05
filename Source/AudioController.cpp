#include <JuceHeader.h>
#include "AudioController.h"

AudioController::AudioController(DecksContainer* _decksContainer, juce::ChangeBroadcaster *_decksChanged)
    :decksContainer(_decksContainer),
    decksChanged(_decksChanged),
    ballWindowPosX(0),
    ballWindowPosY(0),
    deckSize(20),
    ballBounds(deckSize, &ballMoved),
    boundsToUse({0,0,0,0})
{
    decksChanged -> addChangeListener(this);
    ballMoved.addChangeListener(this);
    addAndMakeVisible(ballBounds);
}

AudioController::~AudioController(){}

void AudioController::paint (juce::Graphics& g){
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    // Here we checking if the decks map is empty
    if (decks.empty()) {
        g.setColour (juce::Colours::white);
        g.setFont (14.0f);
        g.drawText ("No decks added...", getLocalBounds(),juce::Justification::centred, true); 
    }
}

void AudioController::resized(){
    updateBoundsAndModel(decksContainer -> decks.size(), &modelToUse, &boundsToUse);
    updateDecks(&decks, &decksContainer -> decks, &modelToUse);
    ballBounds.setBounds(boundsToUse[0] - deckSize/2,boundsToUse[1] - deckSize/2,boundsToUse[2],boundsToUse[3]);

    for(const auto& deck : decks){
        deck.second -> setBounds(
            deck.second -> coOrdinates.getX() - deckSize/2, 
            deck.second -> coOrdinates.getY() - deckSize/2, 
            deckSize, deckSize);
    }
}

void AudioController::updateDecks(std::map<int, std::unique_ptr<DeckBallComponent>>* _deckMap, std::map<int, std::shared_ptr<DeckGUI>>* _decksGUI, std::vector<juce::Point<int>>* _modelToUse){
    if(_decksGUI->size() > 4) return; //just in case
    // Here we just clearing the decks for the new mapping to be made
    clearDecksMapping(_deckMap); 

    int iterator{0};
    // Here we creating a new DeckBallComponent for each deck, using a particular model
    for (const auto& deck : *_decksGUI){
        std::unique_ptr<DeckBallComponent> newDeck = std::make_unique<DeckBallComponent>(&_modelToUse->at(iterator), &deck.second->deckColour);
        addAndMakeVisible(*newDeck, 0); // -- 0 is the z-index being back of others
        _deckMap->insert({deck.first,std::move(newDeck)});
        iterator ++;
    }
}

void AudioController::updateBoundsAndModel(int deckAmount, std::vector<juce::Point<int>>* model, std::vector<int>* bounds){
    const int spaceHeight {getHeight() - deckSize * 3/2};
    const int spaceWidth {getHeight() - deckSize * 3/2};
    // Here we are creating a model and bounds depending on how many decks we are working with
    switch (deckAmount){
        case 1:
            *model =    std::vector<juce::Point<int>> {
                            juce::Point<int>{getWidth()/2, getHeight()/2}
                        };
            *bounds =   std::vector<int> {
                            getWidth()/2,
                            getHeight()/2,
                            deckSize,
                            deckSize
                        };
            break;
        case 2:
            *model =    std::vector<juce::Point<int>> {
                            juce::Point<int>{getWidth()/2 - spaceWidth / 2, getHeight()/2},
                            juce::Point<int>{getWidth()/2 + spaceWidth / 2, getHeight()/2}
                        };
            *bounds =   std::vector<int> {
                            getWidth()/2 - spaceWidth/2,
                            getHeight()/2,
                            spaceWidth + deckSize,
                            deckSize
                        };
            break;
        case 3:
            *model =    std::vector<juce::Point<int>> {
                            juce::Point<int>{getWidth()/2 - spaceWidth / 2, getHeight()/2 - spaceHeight / 2},
                            juce::Point<int>{getWidth()/2 + spaceWidth / 2, getHeight()/2 - spaceHeight / 2},
                            juce::Point<int>{getWidth()/2, getHeight()/2 + spaceHeight / 2}
                        };
            *bounds =   std::vector<int> {
                            getWidth()/2 - spaceWidth/2,
                            getHeight()/2 - spaceWidth/2,
                            spaceWidth + deckSize,
                            spaceHeight + deckSize
                        };
            break;
        case 4:
            *model =    std::vector<juce::Point<int>> {
                            juce::Point<int>{getWidth()/2 - spaceWidth / 2, getHeight()/2 - spaceHeight / 2},
                            juce::Point<int>{getWidth()/2 + spaceWidth / 2, getHeight()/2 - spaceHeight / 2},
                            juce::Point<int>{getWidth()/2 - spaceWidth / 2, getHeight()/2 + spaceHeight / 2},
                            juce::Point<int>{getWidth()/2 + spaceWidth / 2, getHeight()/2 + spaceHeight / 2}
                        };
            *bounds =   std::vector<int> {
                            getWidth()/2 - spaceWidth/2,
                            getHeight()/2 - spaceWidth/2,
                            spaceWidth + deckSize,
                            spaceHeight + deckSize
                        };
            break;
        default:
            *model =    std::vector<juce::Point<int>> {
                            juce::Point<int>{getWidth()/2, getHeight()/2}
                        };
            *bounds =   std::vector<int> {0,0,0,0};
            break;
    }
}

void AudioController::changeListenerCallback (juce::ChangeBroadcaster *source){
    if (source == decksChanged) {
        resized();
        repaint();
    }
    if (source == &ballMoved){
        // Everytime the ball is moved we update our values
        updateRelativePositionBall(&ballWindowPosX, &ballWindowPosY, &boundsToUse, &ballBounds);
        updateDeckDistances(&decks, ballWindowPosX, ballWindowPosY, boundsToUse[2] - deckSize);
        updateAudioPlayerFade();
    }
}

void AudioController::clearDecksMapping(std::map<int, std::unique_ptr<DeckBallComponent>>* _map){
    for(auto& key : *_map){
        key.second.reset();
    }
    _map->clear();
}

double AudioController::distanceBetweenPoints (juce::Point<int> a, juce::Point<int> b){
    // Here we getting the x and y
    int x = a.getX() - b.getX();
    int y = a.getY() - b.getY();
    // Here just making positive
    if(x < 0)x *= -1;
    if(y < 0)y *= -1;
    // Here just pythagoras
    return std::sqrt(std::pow(x, 2) + std::pow(y, 2));
}

void AudioController::updateRelativePositionBall(int* _posX, int* _posY, std::vector<int>* _bounds, BallBounds* _ballContainer) {
    *_posX = _bounds->at(0) + _ballContainer->posX;
    *_posY = _bounds->at(1) + _ballContainer->posY;
}

void AudioController::updateDeckDistances(std::map<int, std::unique_ptr<DeckBallComponent>>* _decks, int _windowPosX, int _windowPosY, double maxDist) {
    // Here we updating the deckBallComponent decks distance from the controller
    for (const auto& deck : *_decks){
        double distance = distanceBetweenPoints(deck.second->coOrdinates,juce::Point<int>{_windowPosX,_windowPosY});
        // Below we constraining the distance
        if (distance > maxDist) deck.second->distance = maxDist; 
        else deck.second->distance = distance;
    }
}

void AudioController::updateAudioPlayerFade() {
    for (const auto& audioPlayerDeck : decksContainer->audioPlayers){
        // Below if the distance is at its max then we dont lower the fade, else we do
        if(decks[audioPlayerDeck.first] -> distance <= (boundsToUse[2] - deckSize)/2){
            // audioPlayerDeck.second->volumeFade = 1.0f;
            audioPlayerDeck.second->fadeUpdate(1.0f);
        } else {
            /** Here we getting the value from the range we will need to map, this difficult to explain
             * So a value between 0-120,e.g. if value is 60 will be 0, a value 70 is 10, a value 120 is 60
             * so we will have a value between 0-60
            */
            double a = decks[audioPlayerDeck.first]->distance - ((boundsToUse[2] - deckSize)/2); // so between 0-120, if 70 will be 10
            // Here we mapping the value to a ratio between 0-1
            double b = a / ((boundsToUse[2] - deckSize)/2); // should return ratio between 0-1;
            // Here we inverting the result and updating the player
            audioPlayerDeck.second->fadeUpdate(1 - b);
        }
    }
}