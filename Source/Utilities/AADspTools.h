//
// Created by Aaron Petrini on 03/11/2026.
//

#pragma once

#include <algorithm>
#include <array>
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

    class Oversampler2x
    {
    public:
        static constexpr int factor {2};

        void prepare (int maxBlockSize, int numChannels)
        {
            assert (maxBlockSize > 0 && numChannels > 0);
            maxSamples = maxBlockSize;
            upChannels.assign (static_cast<size_t> (numChannels), std::vector<float> (static_cast<size_t> (maxBlockSize * factor), 0.f));
            upPointers.clear();
            for (auto& channel : upChannels)
                upPointers.push_back (channel.data());
            upFilters.assign (static_cast<size_t> (numChannels), HalfBandFilter {});
            downFilters.assign (static_cast<size_t> (numChannels), HalfBandFilter {});
        }

        void reset()
        {
            for (auto& filter : upFilters)   filter.reset();
            for (auto& filter : downFilters) filter.reset();
        }

        int getLatencySamples() const { return 3; }

        float* const* upsample (const float* const* channelsData, int numChannels, int numSamples)
        {
            assert (numChannels <= static_cast<int> (upChannels.size()) && numSamples <= maxSamples);
            for (int ch {0}; ch < numChannels; ++ch)
            {
                auto& filter = upFilters[static_cast<size_t> (ch)];
                auto* upData = upPointers[static_cast<size_t> (ch)];
                for (int n {0}; n < numSamples; ++n)
                    filter.upsample (channelsData[ch][n], upData[2 * n], upData[2 * n + 1]);
            }
            return upPointers.data();
        }

        void downsample (float* const* channelsData, int numChannels, int numSamples)
        {
            assert (numChannels <= static_cast<int> (upChannels.size()) && numSamples <= maxSamples);
            for (int ch {0}; ch < numChannels; ++ch)
            {
                auto& filter = downFilters[static_cast<size_t> (ch)];
                const auto* upData = upPointers[static_cast<size_t> (ch)];
                for (int n {0}; n < numSamples; ++n)
                    channelsData[ch][n] = filter.downsample (upData[2 * n], upData[2 * n + 1]);
            }
        }

    private:
        struct AllpassSection
        {
            float coefficient {0.f};
            float previousInput {0.f};
            float previousOutput {0.f};

            float process (const float input)
            {
                const auto output = coefficient * (input - previousOutput) + previousInput;
                previousInput = input;
                previousOutput = output;
                return output;
            }
        };

        struct HalfBandFilter
        {
            HalfBandFilter()
            {
                for (size_t index {0}; index < pathA.size(); ++index)
                {
                    pathA[index].coefficient = coefficients[index * 2];
                    pathB[index].coefficient = coefficients[index * 2 + 1];
                }
            }

            void upsample (const float input, float& evenOutput, float& oddOutput)
            {
                evenOutput = processPath (pathA, input);
                oddOutput = processPath (pathB, input);
            }

            float downsample (const float evenInput, const float oddInput)
            {
                return 0.5f * (processPath (pathA, oddInput) + processPath (pathB, evenInput));
            }

            void reset()
            {
                for (auto* path : {&pathA, &pathB})
                    for (auto& section : *path)
                        section.previousInput = section.previousOutput = 0.f;
            }

            static float processPath (std::array<AllpassSection, 4>& path, float sample)
            {
                for (auto& section : path)
                    sample = section.process (sample);
                return sample;
            }

            std::array<AllpassSection, 4> pathA, pathB;
        };

        static constexpr std::array<float, 8> coefficients {0.0406334609f, 0.1505051290f, 0.3007570560f, 0.4607745050f,
                                                            0.6095243149f, 0.7385038411f, 0.8492238104f, 0.9497427837f};

        std::vector<std::vector<float>> upChannels;
        std::vector<float*> upPointers;
        std::vector<HalfBandFilter> upFilters, downFilters;
        int maxSamples {0};
    };
}