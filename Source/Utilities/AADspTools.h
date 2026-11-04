//
// Created by Aaron Petrini on 03/11/2026.
//

#pragma once

#include <algorithm>
#include <vector>
#include <cassert>

namespace AADspTools
{
    class SmoothedBypass
    {
    public:
        void prepare (double newSampleRate, int maxBlockSize, int numChannels, bool startBypassed)
        {
            assert(newSampleRate > 0.0 && maxBlockSize > 0 && numChannels > 0);
            maxChunkSize = std::max (1, maxBlockSize);
            dryChannels.assign (static_cast<size_t> (numChannels), std::vector<float> (static_cast<size_t> (maxChunkSize), 0.f));
            chunkChannels.assign (static_cast<size_t> (numChannels), nullptr);

            sampleRate = newSampleRate;
            updateStepPerSample();

            targetWet = startBypassed ? 0.f : 1.f;
            currentWet = targetWet;
        }

        void setRampSeconds (double newRampSeconds)
        {
            assert (newRampSeconds >= 0.0);
            rampSeconds = newRampSeconds;
            updateStepPerSample();
        }

        void setBypassed (const bool shouldBeBypassed) { targetWet = shouldBeBypassed ? 0.f : 1.f; }
        bool isFullyBypassed() const             { return currentWet == 0.f && targetWet == 0.f; }

        template <typename  ProcessFunction>
        void process (float* const* channelsData, int numChannels, int numSamples, ProcessFunction&& processUnit)
        {
            assert (numChannels > 0 && numChannels <= static_cast<int> (chunkChannels.size()));
            if (currentWet == 1.f && targetWet == 1.f)
            {
                processUnit(channelsData, numChannels, numSamples);
                return;
            }

            const auto channelsToUse = std::min(numChannels, static_cast<int>(chunkChannels.size()));
            for (int chunkStart {0}; chunkStart < numSamples; chunkStart += maxChunkSize)
            {
                const auto chunkSize = std::min(maxChunkSize, numSamples - chunkStart);
                for (int ch {0}; ch < channelsToUse; ++ch)
                {
                    chunkChannels[static_cast<size_t> (ch)] = channelsData[ch] + chunkStart;
                    std::copy_n(chunkChannels[static_cast<size_t> (ch)], chunkSize, dryChannels[static_cast<size_t> (ch)].begin());
                }

                processUnit (chunkChannels.data(), channelsToUse, chunkSize);

                for (int n {0}; n < chunkSize; ++n)
                {
                    currentWet = currentWet < targetWet ? std::min (currentWet + stepPerSample, targetWet)
                                                        : std::max (currentWet - stepPerSample, targetWet);

                    for (int ch {0}; ch < channelsToUse; ++ch)
                    {
                        const auto drySample = dryChannels[static_cast<size_t> (ch)][static_cast<size_t> (n)];
                        auto& sample = chunkChannels[static_cast<size_t> (ch)][n];
                        sample = drySample + (sample - drySample) * currentWet;
                    }
                }
            }
        }

    private:
        void updateStepPerSample()
        {
            const auto rampSamples = std::max (1, static_cast<int> (sampleRate * rampSeconds));
            stepPerSample = 1.f / static_cast<float> (rampSamples);
        }

        std::vector<std::vector<float>> dryChannels;
        std::vector<float*> chunkChannels;
        int maxChunkSize {1};
        float currentWet{1.f};
        float targetWet{1.f};
        float stepPerSample {0.f};
        double sampleRate {44100.0};
        double rampSeconds {0.015};
    };
}