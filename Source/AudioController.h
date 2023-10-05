/**Personal Notes
 * Commenting seems fine
 * Structure seems fine
 * This file is complete
*/
#pragma once
#include <JuceHeader.h>
#include "DecksContainer.h"
#include <cmath>

//==============================================================================  
/**ControllerBallComponent
 * This is the component for the interactive controller used to adjust the 'fade' of each of the tracks
 * This is a struct with the implementations imbedded as is still part of the AudioController
 * I had referenced a demo juce project for this implementation
*/
struct ControllerBallComponent  :   public juce::Component,
                                    public juce::Timer 
{                   
    /** Component functions */
    ControllerBallComponent(){
        // Here we starting our thread
        startTimer(50);
    }; 

    void paint (juce::Graphics& g) override {
        auto area = getLocalBounds();
        g.setColour (juce::Colour(0.0f,0.0f,0.0f,0.5f)); //-- Here we just adding some alpha
        g.fillEllipse(area.toFloat());
        g.drawEllipse(area.toFloat(), 0.0f);

        // Here we transforming to the center of out component
        g.addTransform(juce::AffineTransform::translation(area.getCentre()));
        // Here we applying our rotation
        g.addTransform(juce::AffineTransform::rotation(rotationRadians));
        g.setColour (juce::Colours::white); 
        g.drawLine(0,0,area.getWidth()/3,area.getHeight()/3);
    }
    void resized() override {
        // Here we set the limits so our ball stays within the constrainer
        constrainer.setMinimumOnscreenAmounts (getHeight(), getWidth(),getHeight(), getWidth());
    }
    void mouseDown (const juce::MouseEvent& e) override {
        // Starts the dragging when mouse event heard
        dragger.startDraggingComponent (this, e);
    }
    void mouseDrag (const juce::MouseEvent& e) override {
        // Applies the drag to the component as well as applies the constrainer
        dragger.dragComponent (this, e, &constrainer);
    }
    void timerCallback()override{
        // Here we are incrementing our ratation value then resetting
        rotationRadians += 0.1;
        if (rotationRadians > 6.2) rotationRadians = 0.0f;
        repaint();
    }
    
    /** Component variables */
    juce::ComponentBoundsConstrainer constrainer;
    juce::ComponentDragger dragger;
    double rotationRadians = 0.0f;
};
//==============================================================================  

//==============================================================================  
/**DeckBallComponent
 * This is the component individual deck ball indicators
 * Each deck gets one of these balls made for it
 * This is a struct with the implementations imbedded as is still part of the AudioController
*/
struct DeckBallComponent    :   public juce::Component {
   /** Component functions */
   /**Here we take arguments in a constructor
     * A juce::Point to represent the coordinates of the ball
     * The ball colour
    */
    DeckBallComponent(juce::Point<int>* _coOrdinates, juce::Colour* _ballColour) 
        :coOrdinates(*_coOrdinates),
        ballColour(_ballColour),
        distance(0.0f)
    {}
    void paint (juce::Graphics& g) override {
        auto area = getLocalBounds();
        g.setColour (*ballColour);
        g.fillEllipse(area.toFloat());
        g.drawEllipse(area.toFloat(), 0.0f);
    }
    void resized() override {}

    /** Component variables */
    juce::Point<int> coOrdinates;
    juce::Colour* ballColour;
    double distance;
};
//============================================================================== 

//==============================================================================  
/**BallBounds
 * This is a component the controllerBall gets added too
 * We created this component to set bounds for the controllerBall 
 * The calling class can dynamically adjust the bounds for this component
 * This is a struct with the implementations imbedded as is still part of the AudioController
*/

struct BallBounds           :   public juce::Component {
    /** Component functions */
    /**Here we take arguments in a constructor
     * The ball size, so we know how to manipulate the constraints
     * A broadcaster, to let know that the ball has moved
    */
    BallBounds(int _ballSize, juce::ChangeBroadcaster* _ballMoved)
        :ballSize(_ballSize),
        ballMoved(_ballMoved),
        posX(0),
        posY(0)
    {
        addAndMakeVisible(controllerBallComponent);
    }
    void paint (juce::Graphics& g) override {
        // Here we update our variables with the new coordinates
        posX = controllerBallComponent.getPosition().x;
        posY = controllerBallComponent.getPosition().y;
        // Once we have our new coordinates we send our broadcast
        ballMoved->sendChangeMessage();
    }
    void resized() override {
        controllerBallComponent.setBounds(getWidth()/2 - ballSize/2,getHeight()/2 - ballSize/2,ballSize,ballSize);
    }

    /** Component variables */
    int ballSize, posX, posY;
    juce::ChangeBroadcaster* ballMoved;
    ControllerBallComponent controllerBallComponent;
};
//==============================================================================  

//==============================================================================  
/**AudioController
 * This component handles the the fade of the tracks
 * This component creates the deck balls, and the controllerBall
 * This is the main class and the methods implementation is in the cpp file
*/
class AudioController       :   public juce::Component,
                                public juce::ChangeListener
{
public:
    /** Component functions */
    /**Here we take arguments in a constructor
     * The decksContainer, which includes the audio players
     * A broadcaster, to let know that the component has changes its size
    */
    AudioController(DecksContainer* _decksContainer, juce::ChangeBroadcaster *_decksChanged);
    ~AudioController() override;
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    /** Component functions */
    void changeListenerCallback (juce::ChangeBroadcaster *source) override;
     // This function constructs the coordinates for the model and bounds depending on decks amount
    void updateBoundsAndModel(int deckAmount, std::vector<juce::Point<int>>* model, std::vector<int>* bounds);
    // This function gets called when there is a broadcast in the parsed broadcaster
    void updateDecks(
        std::map<int, std::unique_ptr<DeckBallComponent>>* _deckMap, 
        std::map<int, std::shared_ptr<DeckGUI>>* _decksGUI, 
        std::vector<juce::Point<int>>* modelToUse);
    // This function resets the pointers in the deck balls
    void clearDecksMapping(std::map<int, std::unique_ptr<DeckBallComponent>>* _map);
    // This function calculates the distance between two coordinates
    double distanceBetweenPoints (juce::Point<int>, juce::Point<int>);
    // This function updates the balls coordinates, gets called when hears on the broadcaster
    void updateRelativePositionBall(int* _posX, int* _posY, std::vector<int>* _bounds, BallBounds* _ballContainer);
    // This function updates each balls distance from the controller, gets called when hears on the broadcast
    void updateDeckDistances(std::map<int, std::unique_ptr<DeckBallComponent>>* _decks, int _windowPosX, int _windowPosY, double maxDist);
    // This function updates parsed audioplayers fade values
    void updateAudioPlayerFade();

    /** Component variables */
    DecksContainer* decksContainer;
    juce::ChangeBroadcaster *decksChanged;
    juce::ChangeBroadcaster ballMoved;
    // Below are the variables that hold the coordinates of the controllerBall on the window
    int ballWindowPosX, ballWindowPosY;
    // Below is the variable which holds the size of the deckBalls size
    int deckSize;
    // Below is a mapping that holds all the decks components
    std::map<int, std::unique_ptr<DeckBallComponent>> decks;
    // Below is the component for the controllerBall and the contraining container
    BallBounds ballBounds;
    // Below is the variable that hold the position coordinate the deck balls should use
    std::vector<juce::Point<int>> modelToUse;
    // Below are variables that hold the sizes of the constraining ballContollers container
    std::vector<int> boundsToUse;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioController)
};
//==============================================================================