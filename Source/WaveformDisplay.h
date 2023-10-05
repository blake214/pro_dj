/**Personal Notes
 * Commenting seems fine
 * Structure seems fine
 * File is complete
*/
#pragma once
#include <JuceHeader.h>
#include "DJAudioPlayer.h"

//==============================================================================  
/**WaveformDisplay
 * This is a component class for the audioPlayers waveform in the deckGUI
 * This component holds the position controll for the audioplayer
*/
class WaveformDisplay  :    public juce::Component,
                            public juce::ChangeListener,
                            public juce::Slider::Listener
{
public:
    /** Component functions */
    /**Here we take arguments in a constructor
     * A reference to the format manager
     * A reference to the cache
     * A colour
     * A pointer to the audio player
    */
    WaveformDisplay(
        juce::AudioFormatManager& formatManagerToUse, 
        juce::AudioThumbnailCache& cacheToUse, 
        juce::Colour _colour,
        std::shared_ptr<DJAudioPlayer> _audioPlayer);
    ~WaveformDisplay() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    // This method loads the url to the audio thumbnail
    void loadURL(juce::URL audioUrl);
    // This method changes the position slider to the current position the track is in
    void setPositionRelative(double position);
private:
    /** Component functions */
    void sliderValueChanged (juce::Slider *slider)override;
    void changeListenerCallback (juce::ChangeBroadcaster *source)override;

    /** Component variables */
    // This variable holds the position slider that controls the position the track is in as well as shows its position
    juce::Slider positionSlider{juce::Slider::SliderStyle::LinearBar, juce::Slider::TextEntryBoxPosition::NoTextBox};
    std::shared_ptr<DJAudioPlayer> audioPlayer;
    juce::Colour deckColour;
    // This audio thumbnail gets created from the parsed cache and formatmanager
    juce::AudioThumbnail audioThumb;
    bool fileLoaded;
    // This variable holds the current position the track is in
    double position;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
//==============================================================================