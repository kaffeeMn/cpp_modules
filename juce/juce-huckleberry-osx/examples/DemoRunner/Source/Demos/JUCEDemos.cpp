/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2017 - ROLI Ltd.

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 5 End-User License
   Agreement and JUCE 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.juce.com/juce-5-licence
   Privacy Policy: www.juce.com/juce-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../../Assets/DemoUtilities.h"
#include "JUCEDemos.h"

//==============================================================================
std::vector<JUCEDemos::DemoCategory>& JUCEDemos::getCategories()
{
    static std::vector<DemoCategory> categories;
    return categories;
}

JUCEDemos::DemoCategory& JUCEDemos::getCategory (const String& name)
{
    auto& categories = getCategories();

    for (auto& c : categories)
        if (c.name == name)
            return c;

    std::vector<FileAndCallback> fc;
    categories.push_back ({ name, fc });

    return categories.back();
}

void JUCEDemos::registerDemo (std::function<Component*()> constructorCallback, const String& filePath, const String& category, bool isHeavyweight)
{
    auto f = findExamplesDirectoryFromExecutable (File::getSpecialLocation (File::SpecialLocationType::currentApplicationFile));

    #if ! (JUCE_ANDROID || JUCE_IOS)
    if (f == File())
    {
        jassertfalse;
        return;
    }
    #endif

    getCategory (category).demos.push_back ({ f.getChildFile (filePath), constructorCallback, isHeavyweight });
}

File JUCEDemos::findExamplesDirectoryFromExecutable (File exec)
{
    int numTries = 15;
    auto exampleDir = exec.getParentDirectory().getChildFile ("examples");

    if (exampleDir.exists())
        return exampleDir;

    while (exec.getFileName() != "examples" && numTries-- > 0)
        exec = exec.getParentDirectory();
    if (exec.getFileName() == "examples")
        return exec;
    return {};
}

//==============================================================================
ScopedPointer<AudioDeviceManager> sharedAudioDeviceManager;

static String getCurrentDefaultAudioDeviceName (AudioDeviceManager& deviceManager, bool isInput)
{
    auto* deviceType = deviceManager.getCurrentDeviceTypeObject();
    jassert (deviceType != nullptr);

    if (deviceType != nullptr)
    {
        auto deviceNames = deviceType->getDeviceNames();
        return deviceNames [deviceType->getDefaultDeviceIndex (isInput)];
    }

    return {};
}

// (returns a shared AudioDeviceManager object that all the demos can use)
AudioDeviceManager& getSharedAudioDeviceManager (int numInputChannels, int numOutputChannels)
{
    if (sharedAudioDeviceManager == nullptr)
        sharedAudioDeviceManager = new AudioDeviceManager();

    auto* currentDevice = sharedAudioDeviceManager->getCurrentAudioDevice();

    if (numInputChannels < 0)
        numInputChannels = (currentDevice != nullptr ? currentDevice->getActiveInputChannels().countNumberOfSetBits() : 1);

    if (numOutputChannels < 0)
        numOutputChannels = (currentDevice != nullptr ? currentDevice->getActiveOutputChannels().countNumberOfSetBits() : 2);

    if (numInputChannels > 0 && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [numInputChannels, numOutputChannels] (bool granted)
                                     {
                                         if (granted)
                                             getSharedAudioDeviceManager (numInputChannels, numOutputChannels);
                                     });

        numInputChannels = 0;
    }

    if (sharedAudioDeviceManager->getCurrentAudioDevice() != nullptr)
    {
        AudioDeviceManager::AudioDeviceSetup setup;
        sharedAudioDeviceManager->getAudioDeviceSetup (setup);

        auto numInputs  = jmax (numInputChannels,  setup.inputChannels.countNumberOfSetBits());
        auto numOutputs = jmax (numOutputChannels, setup.outputChannels.countNumberOfSetBits());

        auto oldInputs  = setup.inputChannels.countNumberOfSetBits();
        auto oldOutputs = setup.outputChannels.countNumberOfSetBits();

        if (oldInputs != numInputs || oldOutputs != numOutputs)
        {
            if (oldInputs == 0 && oldOutputs == 0)
            {
                sharedAudioDeviceManager->initialise (numInputChannels, numOutputChannels, nullptr, true, {}, nullptr);
            }
            else
            {
                setup.useDefaultInputChannels = setup.useDefaultOutputChannels = false;

                setup.inputChannels.clear();
                setup.outputChannels.clear();

                setup.inputChannels.setRange (0, numInputs, true);
                setup.outputChannels.setRange (0, numOutputs, true);

                if (oldInputs == 0 && numInputs > 0 && setup.inputDeviceName.isEmpty())
                    setup.inputDeviceName = getCurrentDefaultAudioDeviceName (*sharedAudioDeviceManager, true);

                if (oldOutputs == 0 && numOutputs > 0 && setup.outputDeviceName.isEmpty())
                    setup.outputDeviceName = getCurrentDefaultAudioDeviceName (*sharedAudioDeviceManager, false);

                sharedAudioDeviceManager->setAudioDeviceSetup (setup, false);
            }
        }
    }
    else
    {
        sharedAudioDeviceManager->initialise (numInputChannels, numOutputChannels, nullptr, true, {}, nullptr);
    }

    return *sharedAudioDeviceManager;
}

//==============================================================================
// need to split this into two files otherwise VS will fall over
void registerDemos_One() noexcept;
void registerDemos_Two() noexcept;

void registerAllDemos() noexcept
{
    registerDemos_One();
    registerDemos_Two();
}
