#include "DJAudioPlayer.h"

DJAudioPlayer::DJAudioPlayer(juce::AudioFormatManager& _formatManager, double _original_sampleRate) 
    :formatManager(_formatManager),
    volume(5.0f),
    lowPassFilter(juce::dsp::IIR::Coefficients<float>::makeLowPass(_original_sampleRate,10000.0f, 0.1f)),
    highPassFilter(juce::dsp::IIR::Coefficients<float>::makeHighPass(_original_sampleRate,10000.0f, 0.1f)),
    volumeFade(1.0f)
{
    transport.setGain(volume);
}

DJAudioPlayer::DJAudioPlayer(juce::AudioFormatManager& _formatManager) 
    :formatManager(_formatManager)
{}

DJAudioPlayer::~DJAudioPlayer(){}

void DJAudioPlayer::prepareToPlay (int samplesPerBlockExpected, double sampleRate){
    // Here we storing the input paramiters for when we recall prepareToPlay
    original_samplesPerBlockExpected = samplesPerBlockExpected;
    original_sampleRate = sampleRate;

    // Below we setting up our process spec
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = original_sampleRate;
    spec.maximumBlockSize = samplesPerBlockExpected;
    spec.numChannels = 2;

    // Below we resetting our filters
    lowPassFilter.reset();
    highPassFilter.reset();

    // Below we preparing our filters to play
    lowPassFilter.prepare(spec);
    highPassFilter.prepare(spec);

    // Below we preparing our resample to play
    resampleSource.prepareToPlay(samplesPerBlockExpected,sampleRate);
}

void DJAudioPlayer::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill){
    if(isLooping)performLoop();

    // Below we adding our sesample to the buffer
    resampleSource.getNextAudioBlock(bufferToFill);

    // Below we applying our filters to the buffer
    juce::dsp::AudioBlock<float> block (*bufferToFill.buffer);
    applyFilters();
    lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
    highPassFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
}

void DJAudioPlayer::releaseResources(){
    resampleSource.releaseResources();
}

void DJAudioPlayer::loadURL(juce::URL url){
    juce::File file = url.getLocalFile();
    juce::AudioFormatReader* reader = formatManager.createReaderFor(file);
    std::unique_ptr<juce::AudioFormatReaderSource> tempSource(new juce::AudioFormatReaderSource(reader,true));
    transport.setSource(tempSource.get());
    playSource.reset(tempSource.release());
}

void DJAudioPlayer::setVolume(double _volume){
    if (_volume > 0 || _volume < 10){
        volume = _volume;
        // Here we multiply the volume by the fade
        transport.setGain(volume * volumeFade);
    }
}

void DJAudioPlayer::setSpeed(double ratio){
    // If we resample with '0' we get an assertion error, so we making it 0.1
    if (ratio > 0.1 || ratio < 10) resampleSource.setResamplingRatio(ratio);
}

void DJAudioPlayer::setPositionRelative(double pos){
    if (pos > 0 || pos < 10){
        double posInSecs = (pos/10) * transport.getLengthInSeconds();
        transport.setPosition(posInSecs);
    }
}

void DJAudioPlayer::start(){
    transport.start();
}

void DJAudioPlayer::stop(){
    transport.stop();
    transport.setPosition(0.0f);
}

void DJAudioPlayer::pause(){
    transport.stop();
}

void DJAudioPlayer::looping(bool b){
    isLooping = b;
}

void DJAudioPlayer::performLoop(){
    // Here we checking if we at the end of the track, then restart
    if(transport.getCurrentPosition() >= transport.getLengthInSeconds() - 0.0005){
        transport.stop();
        transport.setPosition(0.0f);
        transport.start();
    }
}

double DJAudioPlayer::getPositionRelative(){
    double length = transport.getLengthInSeconds();
    double position = transport.getCurrentPosition();
    if (length == 0) return 0.0f;
    return position/length;
}

double DJAudioPlayer::getVolume(){
    return volume;
}

void DJAudioPlayer::fadeUpdate(double d){
    if (d < 0 || d > 1) return;
    volumeFade = d;
    transport.setGain(volume * volumeFade);
}

double DJAudioPlayer::length(){
    return transport.getLengthInSeconds();
}

void DJAudioPlayer::setLowPassFreqRelative(float v){
    if (v < 0 || v > 10) return;
    const float lPassFreqH {10000.0f};
    const float lPassFreqL {100.0f};
    float test = (v/10) * (lPassFreqH - lPassFreqL);
    lowPassFreq = test + lPassFreqL;
}

void DJAudioPlayer::setHighPassFreqRelative(float v){
    if (v < 0 || v > 10) return;
    const float hPassFreqH {10000.0f};
    const float hPassFreqL {20.0f};
    float test = (v/10) * (hPassFreqH - hPassFreqL);
    highPassFreq = test + hPassFreqL;
}

void DJAudioPlayer::applyFilters(){
    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(original_sampleRate,lowPassFreq, res);
    *highPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(original_sampleRate,highPassFreq, res);
}