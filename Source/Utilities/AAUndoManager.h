//
// Created by Aaron Petrini on 18/10/2026.
//

#pragma once
#include "juce_audio_processors/juce_audio_processors.h"
#include "ParamNames.h"

class AAUndoManager : public juce::UndoManager,
                      private juce::AudioProcessorParameter::Listener
{
public:
    explicit AAUndoManager (const int maxNumberOfUnitsToKeep = 100000, const int minimumTransactionsToKeep = 100)
        : juce::UndoManager (maxNumberOfUnitsToKeep, minimumTransactionsToKeep)
    {
    }

    void trackParameters(const juce::AudioProcessor& processor)
    {
        trackedParameters = processor.getParameters();
        for (auto* parameter : trackedParameters)
        {
            const auto parameterID = dynamic_cast<juce::AudioProcessorParameterWithID*> (parameter)->getParameterID();
            if (parameterID != ParamNames::getGeneralBypassName())
                parameter->addListener(this);
        }
    }

    void parameterValueChanged (int, float) override {}

    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override
    {
        auto* parameter = trackedParameters[parameterIndex];

        if (gestureIsStarting)
            gestureStartValue = parameter->getValue();
        else if (! juce::approximatelyEqual (parameter->getValue(), gestureStartValue))
        {
            beginNewTransaction();
            perform (new ParameterChangeAction (*parameter, gestureStartValue, parameter->getValue()));
        }
    }

private:
    class ParameterChangeAction : public juce::UndoableAction
    {
    public:
        ParameterChangeAction (juce::AudioProcessorParameter& parameterToChange, float oldNormalisedValue, float newNormalisedValue)
            : parameter (parameterToChange), oldValue (oldNormalisedValue), newValue (newNormalisedValue)
        {
        }

        bool perform() override { parameter.setValueNotifyingHost (newValue); return true; }
        bool undo() override    { parameter.setValueNotifyingHost (oldValue); return true; }

    private:
        juce::AudioProcessorParameter& parameter;
        float oldValue, newValue;
    };

    juce::Array<juce::AudioProcessorParameter*> trackedParameters;
    float gestureStartValue {0.f};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AAUndoManager)
};
