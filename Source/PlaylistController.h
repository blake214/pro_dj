/**Personal Notes
 * Commenting seems fine
 * Structure seems fine
 * File is complete
*/
#pragma once
#include <JuceHeader.h>
#include "DeckGUI.h"
#include "DecksContainer.h"
#include "PlayListEntry.h"

//==============================================================================  
/**PlaylistController
 * This is a component class for the playlist
 * This component loads and adds tracks to decks
 * This component loads and saves the playlist to a .txt file
*/
class PlaylistController  : public juce::Component,
                            public juce::TableListBoxModel,
                            public juce::FileDragAndDropTarget,
                            public juce::Button::Listener,
                            public juce::TextEditor::Listener
{
public:
    /** Component functions */
    /**Here we take arguments in a constructor
     * A pointer to the decks container which has all the decks in
     * A reference to the format manager
    */
    PlaylistController(DecksContainer* _decksContainer, juce::AudioFormatManager& _formatManager);
    ~PlaylistController() override;
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    /** Component functions */
    // This method uses the DJAudio player class to load a track and retrive the track length
    double getTrackLength(juce::URL url); // ------- should have the format manager as an argument
    // This method inherited from textEditor listener checks for changes in the search box
    void textEditorTextChanged(juce::TextEditor& _searchBar)override;
    // This method searches and deletes a track from the playlist vector
    void deleteTrack(juce::String s, std::vector<PlayListEntry>* list);
    // This method searches and returns true if a track is in the playlist vector
    bool trackExists(juce::String s, std::vector<PlayListEntry>* list);
    // This method overwrites the local list.txt file using a vector of string URLs with each track
    void writePlaylistToFile(std::vector<juce::String>& _tracks, juce::String& _file); 
    // This method reads the list.txt file and returns a vector of strings with each track url
    std::vector<juce::String> readPlaylistFromFile(juce::String& _file); 
    // This method creates a vector of track urls from the playlist vector and calls write to playlist
    void saveToPlaylist(juce::String& _file, std::vector<PlayListEntry>* _trackTitles); 
    // This method populates the playlist vector using the vector of track urls
    void loadStringsToEntrys(std::vector<juce::String>& _currentPlaylist, std::vector<PlayListEntry>* _trackTitles); 
    // This method creates the alert window popup for when the user loads the app to select a new playlist or the previous one
    void alertWindowLoadPlayList();
    // This method runs after the alertWindowLoadPlaylist closes, and performs the load frome the list.txt file
    static void asyncLoadPlayList(int result, PlaylistController* _this);   
    // This method creates the alert window popup for when the user opts to add a track to a deck
    void alertWindowSelectDeck(DecksContainer* _decksContainer);
    // This method runs after the alertWindowSelectDeck closes, and performs the load to deck according to the users choice
    static void asyncDeckChooserAlertWindow (int result, PlaylistController* _this);
    // This method loads a url to the decks containers deck at the id selected by the user
    void loadTrack(int deck, PlayListEntry* _playListEntry);
    // This method opens the file browser and adds the selected file to trackTitles
    void fileBrowser();

    // The below methods inherited from table list model
    void buttonClicked(juce::Button* button) override;
    int getNumRows()override;
    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent &)override;
    juce::Component* refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected, Component *existingComponentToUpdate);

    // The below methods inherited from FileDragAndDropTarget
    bool isInterestedInFileDrag (const juce::StringArray &files)override;
    void filesDropped (const juce::StringArray &files, int x, int y)override;

    /** Component variables */
    // The below variables hold the column sizes for the table
    const int columnDuration, columnLoad, columnDelete;
    // The below variable holds the search text bar component
    juce::TextEditor searchBar;
    // The below variable holds the search results from the searchbar
    std::vector<PlayListEntry> trackTitlesSearch{};
    // The below variable holds a vector of strings of the tracks URLs  in the playlist
    std::vector<juce::String> currentPlaylist;
    // The below is a reference to the parsed format manager
    juce::AudioFormatManager& formatManager;
    // The below is a pointer to the file chooser
    std::unique_ptr<juce::FileChooser> chooser;
    // The below holds a pointer to the parsed deck container
    DecksContainer* decksContainer;
    // The below variable holds the full list of loaded tracks
    std::vector<PlayListEntry> trackTitles;
    // This is a pointer that will hold the alert window popup
    std::unique_ptr<juce::AlertWindow> asyncAlertWindow;
    // This variable will hold the url that was selected from the playlist
    PlayListEntry* selectedPlayListEntryTemp{nullptr};
    // This variable holds a mapping of the decks Ids in a vector, purely for the deck selection when loading
    std::vector<int> deckIdMappings;
    // This variable holds the file the playlist is saved too
    juce::String playListLocalFile{"list.txt"};

    juce::TextButton addTrack;
    juce:: TableListBox tableComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistController)
};
//==============================================================================