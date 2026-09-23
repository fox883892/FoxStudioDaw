#include "ProjectManager.h"

ProjectSession ProjectManager::createDefaultSession() const
{
    ProjectSession session;
    session.name = "fox-daw-session";
    session.tempo = 120;
    session.loop = false;
    session.metronome = true;
    session.sampleRate = 44100.0;
    session.tracks = { "Drums", "Bass", "Lead" };

    ProjectTrack lead;
    lead.name = "Lead";
    lead.type = "midi";
    lead.gainDb = 0.0;
    lead.pan = 0.0;
    lead.notes = {
        { 60, 0.0, 1.0 },
        { 64, 1.0, 1.0 },
        { 67, 2.0, 1.0 },
        { 72, 3.0, 2.0 }
    };
    session.trackData.push_back(lead);

    return session;
}

juce::ValueTree ProjectManager::createTrackTree(const juce::String& trackName)
{
    juce::ValueTree track("track");
    track.setProperty("name", trackName, nullptr);
    return track;
}

juce::ValueTree ProjectManager::createNoteTree(const MidiNoteData& note)
{
    juce::ValueTree tree("note");
    tree.setProperty("midiNote", note.midiNote, nullptr);
    tree.setProperty("startBeat", note.startBeat, nullptr);
    tree.setProperty("lengthBeats", note.lengthBeats, nullptr);
    return tree;
}

MidiNoteData ProjectManager::readNoteTree(const juce::ValueTree& tree)
{
    MidiNoteData note;
    note.midiNote = tree.getProperty("midiNote");
    note.startBeat = tree.getProperty("startBeat");
    note.lengthBeats = tree.getProperty("lengthBeats");
    return note;
}

bool ProjectManager::saveToFile(const juce::File& file, const ProjectSession& session)
{
    juce::ValueTree root("foxstudio_project");
    root.setProperty("name", session.name, nullptr);
    root.setProperty("tempo", session.tempo, nullptr);
    root.setProperty("loop", session.loop, nullptr);
    root.setProperty("metronome", session.metronome, nullptr);
    root.setProperty("autosave", session.autoSave, nullptr);
    root.setProperty("sampleRate", session.sampleRate, nullptr);

    juce::ValueTree tracks("tracks");
    for (const auto& track : session.tracks)
        tracks.addChild(createTrackTree(track), -1, nullptr);
    root.addChild(tracks, -1, nullptr);

    juce::ValueTree trackData("trackData");
    for (const auto& track : session.trackData)
    {
        juce::ValueTree tree("track");
        tree.setProperty("name", track.name, nullptr);
        tree.setProperty("type", track.type, nullptr);
        tree.setProperty("gainDb", track.gainDb, nullptr);
        tree.setProperty("pan", track.pan, nullptr);

        juce::ValueTree notes("notes");
        for (const auto& note : track.notes)
            notes.addChild(createNoteTree(note), -1, nullptr);
        tree.addChild(notes, -1, nullptr);
        trackData.addChild(tree, -1, nullptr);
    }
    root.addChild(trackData, -1, nullptr);

    const auto xml = juce::ValueTree::toXml(root);
    return file.replaceWithText(xml);
}

ProjectSession ProjectManager::loadFromFile(const juce::File& file)
{
    ProjectSession session = createDefaultSession();
    if (!file.existsAsFile())
        return session;

    const auto xml = file.loadFileAsString();
    const auto tree = juce::ValueTree::fromXml(xml);
    if (tree.isValid())
    {
        session.name = tree.getProperty("name").toString();
        session.tempo = tree.getProperty("tempo");
        session.loop = tree.getProperty("loop");
        session.metronome = tree.getProperty("metronome");
        session.autoSave = tree.getProperty("autosave");
        session.sampleRate = tree.getProperty("sampleRate");

        session.tracks.clear();
        const auto trackTree = tree.getChildWithName("tracks");
        for (auto child : trackTree)
            session.tracks.push_back(child.getProperty("name").toString());

        session.trackData.clear();
        const auto trackDataTree = tree.getChildWithName("trackData");
        for (auto child : trackDataTree)
        {
            ProjectTrack track;
            track.name = child.getProperty("name").toString();
            track.type = child.getProperty("type").toString();
            track.gainDb = child.getProperty("gainDb");
            track.pan = child.getProperty("pan");

            const auto notesTree = child.getChildWithName("notes");
            for (auto noteTree : notesTree)
                track.notes.push_back(readNoteTree(noteTree));
            session.trackData.push_back(track);
        }
    }

    return session;
}
