#include <JuceHeader.h>
#include "PlaylistController.h"

PlaylistController::PlaylistController(DecksContainer* _decksContainer, juce::AudioFormatManager& _formatManager)
    : decksContainer(_decksContainer),
    formatManager(_formatManager),
    columnDuration(100),
    columnLoad(100),
    columnDelete(100),
    addTrack("Add Track")
{
    tableComponent.getHeader().addColumn("Track Title", 1, 400);
    tableComponent.getHeader().addColumn("Duration", 2, columnDuration);
    tableComponent.getHeader().addColumn("", 3, columnLoad);
    tableComponent.getHeader().addColumn("", 4, columnDelete);
    tableComponent.setModel(this);

    addAndMakeVisible(tableComponent);
    addAndMakeVisible(addTrack);
    addAndMakeVisible(searchBar);
    searchBar.addListener(this);
    addTrack.addListener(this);
    searchBar.setTextToShowWhenEmpty("Search the playlist", juce::Colours::lightgrey);

    // Here we checking if the previous list.txt file is empty or not to prompt to load or not
    currentPlaylist = readPlaylistFromFile(playListLocalFile);
    if(!currentPlaylist.empty()) alertWindowLoadPlayList();
}

PlaylistController::~PlaylistController(){
    saveToPlaylist(playListLocalFile, &trackTitles);
    chooser.reset();
    asyncAlertWindow.reset();
}

void PlaylistController::alertWindowLoadPlayList(){
    asyncAlertWindow = std::make_unique<juce::AlertWindow> (
        "Do you want to load previous playlist?",
        "You can load your previous playlist or overrite it with a new playlist...",
        juce::MessageBoxIconType::QuestionIcon);
    asyncAlertWindow->addButton ("Load Previous Playlist", 1, juce::KeyPress (juce::KeyPress::returnKey, 0, 0));
    asyncAlertWindow->addButton ("Start New Playlist", 0, juce::KeyPress (juce::KeyPress::escapeKey, 0, 0));
    // Here we activating the alert window and call a async function to handle the returned data
    asyncAlertWindow->enterModalState (true, juce::ModalCallbackFunction::create (asyncLoadPlayList,this));
}

void PlaylistController::asyncLoadPlayList (int result, PlaylistController* _this){
    // This closes the alert window (so user can interact back with the main GUI)
    _this->asyncAlertWindow->exitModalState(true);
    // This makes the alert window not visiable
    _this->asyncAlertWindow->setVisible(false);
    // This is for when the Load button is pressed
    if (result != 0) {
        // Here we loading our saved playlist
        _this -> loadStringsToEntrys(_this -> currentPlaylist, &_this -> trackTitles);
        _this -> tableComponent.resized();
    }
    // Here we clear up the memory as we exit
    _this -> asyncAlertWindow.reset();
    _this -> selectedPlayListEntryTemp = nullptr;
}

void PlaylistController::paint (juce::Graphics& g){
}

void PlaylistController::resized(){
    // Here we resize the column width to be dynamic
    int columnTitle = int(getWidth() - columnDuration - columnLoad - columnDelete);
    tableComponent.getHeader().setColumnWidth(1,columnTitle);

    double posY{0.0f};
    double addTrackHeight{30};
    double searchBarHeight{30};
    tableComponent.setBounds(0, posY, getWidth(), getHeight() - addTrackHeight - searchBarHeight);
    searchBar.setBounds(0,getHeight() - addTrackHeight - searchBarHeight, getWidth(), searchBarHeight);
    addTrack.setBounds(0,getHeight() - addTrackHeight, getWidth(), addTrackHeight); 
}

int PlaylistController::getNumRows(){
    //Here if the search results isnt empty we return the results 
    if (trackTitlesSearch.size() != 0) return trackTitlesSearch.size();
    else return trackTitles.size();
}

void PlaylistController::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected){
    if(rowIsSelected) g.fillAll(juce::Colours::white);
    else {
        //Here if the search results isnt empty we draw the elements from the search
        if (trackTitlesSearch.size() != 0) g.fillAll(juce::Colours::lightcyan);
        else g.fillAll(juce::Colours::lightgrey);
    }
}

void PlaylistController::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected){
    std::vector<PlayListEntry> trackTitlesToDraw = trackTitles;
    //Here if the search results isnt empty we draw the elements from the search
    if (trackTitlesSearch.size() != 0) trackTitlesToDraw = trackTitlesSearch;

    // Here we painting the cells
    if (columnId == 1) g.drawText(trackTitlesToDraw[rowNumber].trackName,2,0,width-4,height,juce::Justification::centredLeft,true);
    if (columnId == 2) g.drawText(trackTitlesToDraw[rowNumber].trackLengthString,2,0,width-4,height,juce::Justification::centredLeft,true);
}

juce::Component* PlaylistController::refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected, Component *existingComponentToUpdate){
    // This code original from lectures excersise
    if(columnId == 3){
      if (existingComponentToUpdate == nullptr){
        juce::TextButton* button = new juce::TextButton{"Load to deck"};
        juce::String id{std::to_string(rowNumber)};
        button->setComponentID(id);
        button->addListener(this);
        existingComponentToUpdate = button;
      }
    }
    if(columnId == 4){
      if (existingComponentToUpdate == nullptr){
        juce::TextButton* button = new juce::TextButton{"Delete"};
        juce::String id{std::to_string(rowNumber)};
        button->setComponentID(id);
        button->addListener(this);
        existingComponentToUpdate = button;
      }
    }
    return existingComponentToUpdate;
}

void PlaylistController::buttonClicked(juce::Button* button) {
    /**File chooser
     * I wasnt able to figure out how to get the chooser to select multiple files, did try and failed
     * Here using the juce file browser because i dont know how to close the normal one
     * The file browser gets called again when saving the playlist so this needs to be closed in order to do that
     * I wasnt able to get juce to read mp3 files either, spent multiple hours and failed
    */
    if (button == &addTrack){
        fileBrowser();
        return;
    }

    // Here we load a track to a deck or prompt that there is no deck
    juce::String buttonType = button -> getButtonText();
    if(buttonType == "Load to deck"){
        int id = std::stoi(button->getComponentID().toStdString());
        if (decksContainer->decks.size() >= 1){
            selectedPlayListEntryTemp = &trackTitles[id];
            alertWindowSelectDeck(decksContainer);
        } else {
            juce::AlertWindow::showMessageBoxAsync (
                    juce::MessageBoxIconType::InfoIcon, 
                    "No deck available",
                    "You first need to add a deck to load a track..",
                    "OK");
        }
        return;
    }

    // Here we delete the track from the searched track vector as well as the original
    if (buttonType == "Delete"){
        int id = std::stoi(button->getComponentID().toStdString());
        juce::String trackNameToDelete;
        if (trackTitlesSearch.size() > 0) {
            trackNameToDelete = trackTitlesSearch[id].trackName;
            deleteTrack(trackNameToDelete, &trackTitlesSearch);
        } 
        else trackNameToDelete = trackTitles[id].trackName;
        deleteTrack(trackNameToDelete, &trackTitles);
        tableComponent.resized();
        tableComponent.repaint();
        return;
    }
}

bool PlaylistController::isInterestedInFileDrag (const juce::StringArray &files){
    bool fileExtensionsOk{true};
    for(juce::String filename : files){
        // Here we only interested in .wav files
        if(!filename.contains(juce::StringRef(juce::String(".wav")))) fileExtensionsOk = false;
    }
    return fileExtensionsOk;
}
void PlaylistController::filesDropped (const juce::StringArray &files, int x, int y){
    // Here we just printing the file name that was dropped
    for(juce::String file : files){
        const juce::URL url = juce::URL{juce::File{file}};
        // If the track doesnt already exist in the list
        if(!trackExists(url.getFileName() , &trackTitles)){
            const double trackLength = getTrackLength(url);
            trackTitles.push_back(PlayListEntry(url, trackLength));
        }
        tableComponent.resized();
    }
}

void PlaylistController::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent &){
    if (decksContainer->decks.size() >= 1){
        selectedPlayListEntryTemp = &trackTitles[rowNumber];
        alertWindowSelectDeck(decksContainer);
    } else {
        juce::AlertWindow::showMessageBoxAsync (
                juce::MessageBoxIconType::InfoIcon, 
                "No deck available",
                "You first need to add a deck to load a track..",
                "OK");
    }
}

void PlaylistController::alertWindowSelectDeck(DecksContainer* _decksContainer){
    // Here we creating our alert window
    asyncAlertWindow = std::make_unique<juce::AlertWindow> (
        "Load...",
        "Load the track to one of your decks",
        juce::MessageBoxIconType::QuestionIcon);
    // Here we gathering the availiable decks
    juce::StringArray deckOptions{};
    // Here we reset selectedDeckIds for next time
    deckIdMappings.clear();
    for(const auto& deck : _decksContainer->decks){
        deckOptions.add("Deck: " + deck.second->deckName);
        deckIdMappings.push_back(deck.first);
    }
    // Here we adding components to our alert window
    asyncAlertWindow->addComboBox ("decks", deckOptions, "Pick which deck to add too...");
    asyncAlertWindow->addButton ("Load",     1, juce::KeyPress (juce::KeyPress::returnKey, 0, 0));
    asyncAlertWindow->addButton ("Cancel", 0, juce::KeyPress (juce::KeyPress::escapeKey, 0, 0));
    // Here we activating the alert window
    // we call a async function to handle the returned data
    asyncAlertWindow->enterModalState (true, juce::ModalCallbackFunction::create (asyncDeckChooserAlertWindow,this));
}

void PlaylistController::asyncDeckChooserAlertWindow (int result, PlaylistController* _this){
    // This closes the alert window (so user can interact back with the main GUI)
    _this->asyncAlertWindow->exitModalState(true);
    // This makes the alert window not visiable
    _this->asyncAlertWindow->setVisible(false);
    // This is for when the Load button is pressed
    if (result != 0) {
        // Here we gathering the data from the alert window feilds
        auto selectedDeck = _this->asyncAlertWindow->getComboBoxComponent ("decks") -> getSelectedItemIndex();
        // Here we parse the data to load a track
        _this -> loadTrack(_this -> deckIdMappings[selectedDeck],_this->selectedPlayListEntryTemp);
    }
    // Here we clear up the memory as we exit
    _this -> asyncAlertWindow.reset();
    _this -> selectedPlayListEntryTemp = nullptr;
}

void PlaylistController::loadTrack(int deck, PlayListEntry* _playListEntry){
    decksContainer -> loadURL(deck,_playListEntry);
}

double PlaylistController::getTrackLength(juce::URL url){
    DJAudioPlayer trackLoaded{formatManager};
    trackLoaded.loadURL(url);
    return trackLoaded.length();
}

void PlaylistController::textEditorTextChanged(juce::TextEditor& _searchBar){
    juce::String searchString = _searchBar.getText();
    trackTitlesSearch.clear();
    if (searchString != ""){
        // Here we checking if the string matches a string in the playlist then write the restults to the track title search variable
        for( const PlayListEntry& entry : trackTitles){
            if (entry.trackName.startsWith(searchString)) trackTitlesSearch.push_back(entry);
        }
    }
    tableComponent.resized();
    tableComponent.repaint();
};

void PlaylistController::deleteTrack(juce::String s, std::vector<PlayListEntry>* list){
    int vectorPos = 0;
    for(auto& track : *list){
        if (track.trackName == s){
            list -> erase(list -> begin() + vectorPos);
            break;
        }
        vectorPos += 1;
    }
};

bool PlaylistController::trackExists(juce::String s, std::vector<PlayListEntry>* list){
    for(auto& track : *list){
        if (track.trackName == s) return true;
    }
    return false;
};

void PlaylistController::writePlaylistToFile(std::vector<juce::String>& _tracks, juce::String& _file){
    // Here we loading our list.txt file
    juce::File resourceFile = juce::File::getCurrentWorkingDirectory().getChildFile (_file);
    // Here we create a temp file for our changes to be done to first
    juce::TemporaryFile tempFile (resourceFile);
    juce::FileOutputStream output (tempFile.getFile());

    // If we arent able to open our list.txt file
    if (! output.openedOk()) DBG ("File didnt open");
    // Here we stating our newline character
    output.setNewLineString("\n");
    // Here we iterate over our vector of string urls and writing them to our temp file
    for (const auto& track : _tracks){
        juce::String urlToWrite = track;
        juce::String endLine = "\n";
        urlToWrite+=endLine;
        output.writeText(urlToWrite,true,true,nullptr);
    }
    output.flush(); // (called explicitly to force an fsync on posix) -> this i referenced somewhere
    // If something goes wrong we just console an error
    if (output.getStatus().failed()) DBG ("An error occurred in the stream");
    // Here we save our temp changes to our list.txt
    bool success = tempFile.overwriteTargetFileWithTemporary();
    // Here we just console an error if the write failed
    if (!success) DBG("File failed to overwrite");
}

std::vector<juce::String> PlaylistController::readPlaylistFromFile(juce::String& _file){
    // Here we loading our list.txt file
    juce::File resourceFile = juce::File::getCurrentWorkingDirectory().getChildFile(_file);
    std::vector<juce::String> tracks_loaded{};
    if (resourceFile.existsAsFile()){
        // Here we reading list.txt as a single long string
        auto filetext = resourceFile.loadFileAsString();

        /**Here we tokenise the string
         * This was the easiest way to do this
         * While tokenising we populate the vector of strings with the URLs
        */
        int currentIndex = 0;
        bool ended = false;
        while(!ended){
            int indexOfReturn = filetext.indexOf(currentIndex,"\n");
            if(indexOfReturn != -1){
                juce::String toAdd = filetext.substring(currentIndex, indexOfReturn);
                tracks_loaded.push_back(toAdd);
                currentIndex = indexOfReturn + 3;
            } else {
                ended = true;
                juce::String toAdd = filetext.substring(currentIndex, filetext.length() - 2);
            }
        }
    } 
    return tracks_loaded;
}

void PlaylistController::saveToPlaylist(juce::String& _file, std::vector<PlayListEntry>* _trackTitles){
    std::vector<juce::String> playlistUrls;
    // Here we iterate over the tracktitles and populate the vector of strings or URLs to send to writePlaylist
    for (const auto& track : *_trackTitles){
        playlistUrls.push_back(track.trackURLPath);
    }
    writePlaylistToFile(playlistUrls, _file);
}

void PlaylistController::loadStringsToEntrys(std::vector<juce::String>& _currentPlaylist, std::vector<PlayListEntry>* _trackTitles){
    for (auto path : currentPlaylist){
        // Below we are gathering the url from the vector of strings storing URL and create an absolute path for juce to read
        juce::String storedURL = path;
        juce::String absoluteURL = "file://";
        absoluteURL += storedURL;
        juce::URL url = juce::URL(absoluteURL);

        // Then we create the playlist entry and push to our trackTitles
        PlayListEntry entry = PlayListEntry(url, getTrackLength(url));
        _trackTitles -> push_back(entry);
    }
}

void PlaylistController::fileBrowser(){
    chooser = std::make_unique<juce::FileChooser> ("Select a Wave file to play...", juce::File{}, "*.wav", false); 
    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
    chooser->launchAsync (chooserFlags, [this] (const juce::FileChooser& fc) {
        juce::URL url = fc.getURLResult();
        if (url != juce::URL{}){
            const double trackLength = getTrackLength(url);
            if(!trackExists(url.getFileName() , &trackTitles)){
                trackTitles.push_back(PlayListEntry(url, trackLength));
            }
            // Here we let the boxmodel know the list has changed
            tableComponent.resized();
        }
    });
}