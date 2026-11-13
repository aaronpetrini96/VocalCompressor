//
// Created by Aaron Petrini on 18/10/2026.
//

#pragma once
#include "juce_audio_processors/juce_audio_processors.h"
#include "ParamNames.h"

class AAUndoManager : public juce::UndoManager,
                      private juce::AudioProcessorParameter::Listener,
                      private juce::Timer
{
public:
    explicit AAUndoManager (const int maxNumberOfUnitsToKeep = 100000, const int minimumTransactionsToKeep = 100)
        : juce::UndoManager (maxNumberOfUnitsToKeep, minimumTransactionsToKeep)
    {
    }

    void trackParameters(const juce::AudioProcessor& processor)
    {
        trackedParameters = processor.getParameters();
        gestures = std::vector<Gesture> (static_cast<size_t> (trackedParameters.size()));
        for (auto* parameter : trackedParameters)
        {
            const auto parameterID = dynamic_cast<juce::AudioProcessorParameterWithID*> (parameter)->getParameterID();
            if (parameterID != ParamNames::getGeneralBypassName())
                parameter->addListener(this);
        }
        startTimerHz (30);
    }

    void parameterValueChanged (int, float) override {}

    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override
    {
        auto& gesture = gestures[static_cast<size_t> (parameterIndex)];
        const auto currentValue = trackedParameters[parameterIndex]->getValue();

        if (gestureIsStarting)
        {
            if (! gesture.hasFinished.load())
                gesture.startValue.store (currentValue);
            return;
        }

        gesture.endValue.store (currentValue);
        gesture.hasFinished.store (true);
        anyGestureFinished.store (true);
    }

private:
    void timerCallback() override
    {
        if (! anyGestureFinished.exchange (false))
            return;

        for (int parameterIndex {0}; parameterIndex < trackedParameters.size(); ++parameterIndex)
        {
            auto& gesture = gestures[static_cast<size_t> (parameterIndex)];
            if (! gesture.hasFinished.exchange (false))
                continue;

            const auto startValue = gesture.startValue.load();
            const auto endValue = gesture.endValue.load();
            if (juce::approximatelyEqual (startValue, endValue))
                continue;

            beginNewTransaction();
            perform (new ParameterChangeAction (*trackedParameters[parameterIndex], startValue, endValue));
        }
    }


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

    struct Gesture
    {
        std::atomic<float> startValue {0.f};
        std::atomic<float> endValue {0.f};
        std::atomic<bool> hasFinished {false};
    };

    juce::Array<juce::AudioProcessorParameter*> trackedParameters;
    std::vector<Gesture> gestures;
    std::atomic<bool> anyGestureFinished {false};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AAUndoManager)
};
