/**Personal Notes
 * Commenting seems fine
 * Structure seems fine
 * This file is complete
*/
#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================  
/**DJAudioPlayer
 * This component handles the audio buffer
 * This component controls the fade, volume, speed ...etc of the buffer
*/
class DJAudioPlayer : public juce::AudioSource {
public:
    /** Component functions */
    /**Here we take arguments in a constructor
     * A reference to the format manager
     * The original sample rate, this is for the filters
     * This is the main constructor for the audio players
    */
    DJAudioPlayer(juce::AudioFormatManager& _formatManager, double _original_sampleRate);
    /**Here we take arguments in a constructor
     * A reference to the format manager
     * This constructor is for an audio player that wont be getting played 
     *      (when we just getting details from the audio files)
    */
    DJAudioPlayer(juce::AudioFormatManager& _formatManager);
    ~DJAudioPlayer();
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    // This method loads a url to the audioplayer reader source
    void loadURL(juce::URL url);
    // This method sets the volume in the transport source
    void setVolume(double volume);
    // This method sets the speed in the transport source
    void setSpeed(double ratio);
    // This method sets the position in the transport source
    void setPositionRelative(double pos);
    // This method updates the fade value
    void fadeUpdate(double d);
    // This method updates the looping state
    void looping(bool);
    // This method sets the low pass freqency
    void setLowPassFreqRelative(float v);
    // This method sets the high pass frequency
    void setHighPassFreqRelative(float v);
    // These methods are for standard functionality
    void start();
    void stop();
    void pause();
    // These methods are to get details of the audioPlayer
    double getPositionRelative();
    double getVolume();
    double length();        
private:
    /** Component functions */
    // This method applys the filter values to each filter
    void applyFilters();
    // This method performs the loop / restart
    void performLoop();

    /** Component variables */
    // This variable holds the state of looping
    bool isLooping{false};
    // This variable holds the true volume
    double volume;
    // This variable holds the fade value which gets multiplied by the volume
    double volumeFade;
    // This variable holds the parsed format manager
    juce::AudioFormatManager& formatManager;
    // This variable holds a pointer to the format reader source
    std::unique_ptr<juce::AudioFormatReaderSource> playSource;
    // This variable holds the transport source
    juce::AudioTransportSource transport;
    // This variable holds the resampling audio source
    juce::ResamplingAudioSource resampleSource{&transport, false, 2};

    // This variable holds the original sample rate
    double original_sampleRate;
    // This variable holds the original samplesPerBlockExpected
    int original_samplesPerBlockExpected;

    // The below variables hold the values for lowpass and highpass filters, initialised here for standard
    float lowPassFreq {10000.0f};
    float highPassFreq {20.0f};
    float res {4.0f};

    /** Component filter variables */
    // Below IIR filter for high pass
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients<float>> highPassFilter;
    // Below IIR filter for low pass
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients<float>> lowPassFilter;
};
//==============================================================================