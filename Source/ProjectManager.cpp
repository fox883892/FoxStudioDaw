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
    return session;
}

juce::ValueTree ProjectManager::createTrackTree(const juce::String& trackName)
{
    juce::ValueTree track("track");
    track.setProperty("name", trackName, nullptr);
    return track;
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
    }

    return session;
}
