/**Personal Notes
 * Commenting seems fine
 * Structure seems fine
 * This file is complete
*/
#pragma once
#include <JuceHeader.h>

//==============================================================================  
/**PlayListEntry
 * This is a component for each of the tracks loaded
 * This component holds details about the track that get referenced too
 * This is a struct with the implementations imbedded as is still part of the the Playlist Controller
*/
struct PlayListEntry {
    /** Component functions */
    /**Here we take arguments in a constructor
     * A track url and the length of the track
    */
    PlayListEntry(juce::URL _trackURL, double _trackLength);
    ~PlayListEntry();
    /** Component variables */
    // This variable holds the track file name
    juce::String trackName;
    // This variable holds the tracks URL juce format
    juce::URL trackURL;
    // This variable holds the track length as a double
    double trackLength;
    // This variable holds the track length as a string
    juce::String trackLengthString;
    // This variable holds the tracks URL path as a string
    juce::String trackURLPath;
};
//==============================================================================  