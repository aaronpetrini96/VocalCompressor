#include "PresetManager.h"
#include "BinaryData.h"
#include "ParamNames.h"

static const juce::StringArray nonPresetParameterIDs {ParamNames::getGeneralBypassName()};

static const juce::StringArray rootPresetFileNames {"Default.xml"};

PresetManager::PresetManager (juce::AudioProcessorValueTreeState& treeStateToUse, juce::UndoManager& undoManagerToUse)
    : treeState (treeStateToUse), undoManager (undoManagerToUse)
{
    treeState.state.addListener (this);
}

PresetManager::~PresetManager()
{
    treeState.state.removeListener (this);
}

void PresetManager::valueTreePropertyChanged (juce::ValueTree& parameterTree, const juce::Identifier&)
{
    if (! nonPresetParameterIDs.contains (parameterTree.getProperty ("id").toString()) && ! presetModified.exchange (true))
        sendChangeMessage();
}

void PresetManager::savePreset(const juce::String& presetName)
{
    const auto presetFile = getUserPresetFile (presetName);
    const auto legalPresetName = presetFile.getFileNameWithoutExtension();

    if (legalPresetName.isEmpty())
        return;

    presetFile.getParentDirectory().createDirectory();
    auto presetState = treeState.copyState();
    for (const auto& nonPresetParameterID : nonPresetParameterIDs)
        presetState.removeChild (presetState.getChildWithProperty ("id", nonPresetParameterID), nullptr);
    if (auto xml = presetState.createXml())
    {
        xml->setAttribute ("PluginVersion", JucePlugin_VersionString);
        xml->writeTo (presetFile);
    }

    presetModified = false;
    setCurrentPresetName (legalPresetName);
}

bool PresetManager::deletePreset(const juce::File& presetFile)
{
    if (! presetFile.existsAsFile() || ! presetFile.deleteFile())
        return false;

    if (getCurrentPresetName() == presetFile.getFileNameWithoutExtension())
        setCurrentPresetName ({});

    return true;
}

void PresetManager::loadPresetFile(const juce::File& presetFile)
{
    const auto xml = juce::XmlDocument::parse (presetFile);
    if (xml == nullptr || ! xml->hasTagName (treeState.state.getType()))
        return;

    auto presetState = juce::ValueTree::fromXml (*xml);
    presetState.removeAllProperties (nullptr);
    const auto currentState = treeState.copyState();

    undoManager.beginNewTransaction();
    undoManager.perform (new PresetChangeAction (*this, currentState, getCurrentPresetName(), isPresetModified(),
                                                 presetState, presetFile.getFileNameWithoutExtension()));
}

void PresetManager::loadDefaultPreset()
{
    loadPresetFile (getPresetFolder().getChildFile ("Default.xml"));
    undoManager.clearUndoHistory();
}

void PresetManager::loadAdjacentPreset(const int indexOffset)
{
    auto presetFiles = getPresetFiles (PresetType::factory);
    presetFiles.addArray (getPresetFiles (PresetType::user));

    const auto numPresets = presetFiles.size();

    if (numPresets == 0)
        return;

    const auto loadedPresetName = getCurrentPresetName();
    auto currentIndex = -1;

    for (int fileIndex = 0; fileIndex < numPresets; ++fileIndex)
    {
        if (presetFiles[fileIndex].getFileNameWithoutExtension() == loadedPresetName)
        {
            currentIndex = fileIndex;
            break;
        }
    }

    const auto adjacentIndex = currentIndex < 0 ? (indexOffset > 0 ? 0 : numPresets - 1)
                                                : (currentIndex + indexOffset + numPresets) % numPresets;
    loadPresetFile (presetFiles[adjacentIndex]);
}

juce::Array<juce::File> PresetManager::getPresetFiles(const PresetType presetType) const
{
    juce::Array<juce::File> files;
    const auto presetFolder = getPresetFolder();

    if (presetType == PresetType::user)
    {
        files = presetFolder.getChildFile ("User Presets").findChildFiles (juce::File::findFiles, false, "*.xml");
        files.sort();
        return files;
    }

    for (const auto& rootPresetFileName : rootPresetFileNames)
        if (const auto rootPresetFile = presetFolder.getChildFile (rootPresetFileName); rootPresetFile.existsAsFile())
            files.add (rootPresetFile);

    auto factoryPresetFiles = presetFolder.getChildFile ("Factory Presets").findChildFiles (juce::File::findFiles, false, "*.xml");
    factoryPresetFiles.sort();
    files.addArray (factoryPresetFiles);

    return files;
}

juce::File PresetManager::getUserPresetFile(const juce::String& presetName) const
{
    return getPresetFolder().getChildFile ("User Presets").getChildFile (juce::File::createLegalFileName (presetName.trim()) + ".xml");
}

bool PresetManager::isFactoryPresetName(const juce::String& presetName) const
{
    const auto legalPresetName = getUserPresetFile (presetName).getFileNameWithoutExtension();

    for (const auto& factoryPresetFile : getPresetFiles (PresetType::factory))
        if (factoryPresetFile.getFileNameWithoutExtension().equalsIgnoreCase (legalPresetName))
            return true;

    return false;
}

juce::String PresetManager::getCurrentPresetName() const
{
    const std::scoped_lock lock (currentPresetNameMutex);
    return currentPresetName;
}

void PresetManager::setCurrentPresetName (const juce::String& presetName)
{
    const std::scoped_lock lock (currentPresetNameMutex);
    currentPresetName = presetName;
    sendChangeMessage();
}

void PresetManager::setPresetModified (const bool isModified)
{
    presetModified = isModified;
    sendChangeMessage();
}

bool PresetManager::isPresetModified() const
{
    return presetModified;
}

void PresetManager::applyPreset (const juce::ValueTree& presetState, const juce::String& presetName, const bool isModified)
{
    auto newState = presetState.createCopy();
    const auto currentState = treeState.copyState();
    for (const auto& nonPresetParameterID : nonPresetParameterIDs)
    {
        newState.removeChild (newState.getChildWithProperty ("id", nonPresetParameterID), nullptr);
        newState.appendChild (currentState.getChildWithProperty ("id", nonPresetParameterID).createCopy(), nullptr);
    }
    treeState.replaceState (newState);
    presetModified = isModified;
    setCurrentPresetName (presetName);
}

juce::File PresetManager::getPresetFolder()
{
    return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
          #if JUCE_MAC
           .getChildFile ("Application Support")
          #endif
           .getChildFile (JucePlugin_Manufacturer)
           .getChildFile (JucePlugin_Name)
           .getChildFile ("Presets");
}

void PresetManager::installFactoryPresets()
{
    const auto presetFolder = getPresetFolder();
    const auto factoryPresetFolder = presetFolder.getChildFile ("Factory Presets");
    factoryPresetFolder.createDirectory();

    for (int resourceIndex = 0; resourceIndex < BinaryData::namedResourceListSize; ++resourceIndex)
    {
        const juce::String originalFilename { BinaryData::originalFilenames[resourceIndex] };

        if (! originalFilename.endsWithIgnoreCase (".xml"))
            continue;

        const auto presetFile = (rootPresetFileNames.contains (originalFilename) ? presetFolder : factoryPresetFolder).getChildFile (originalFilename);

        int presetDataSize = 0;
        const auto* presetData = BinaryData::getNamedResource (BinaryData::namedResourceList[resourceIndex], presetDataSize);

        if (presetData == nullptr)
            continue;

        const juce::MemoryBlock bundledPresetData (presetData, (size_t) presetDataSize);
        juce::MemoryBlock installedPresetData;

        if (presetFile.loadFileAsData (installedPresetData) && installedPresetData == bundledPresetData)
            continue;

        presetFile.replaceWithData (presetData, (size_t) presetDataSize);
    }
}