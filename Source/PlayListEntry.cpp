#include <JuceHeader.h>
#include "PlayListEntry.h"

PlayListEntry::PlayListEntry(juce::URL _trackURL, double _trackLength)
    :trackName(_trackURL.getFileName()),
    trackURL(_trackURL),
    trackLength(_trackLength)
{
    // Here we rounding off the length double to two decimal places, and writing to a string
    std::string roundedStringAll = std::to_string(std::ceil(trackLength * 100.0) / 100.0);
    // Below we obtaining the strings for minutes and seconds
    std::string minutes = std::to_string(int(trackLength));
    std::string seconds = roundedStringAll.substr(roundedStringAll.find(".") + 1, 2);
    trackLengthString = minutes + " : " + seconds;
    // Here we obtaining the path of the trackURL in a string
    trackURLPath = trackURL.getLocalFile().getFullPathName();
}

PlayListEntry::~PlayListEntry(){
}