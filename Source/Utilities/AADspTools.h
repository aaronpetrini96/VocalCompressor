//
// Created by Aaron Petrini on 03/11/2026.
//

#pragma once

#include <algorithm>
#include <vector>
#include <cassert>
#include <cmath>
#include <numbers>
#include <numeric>

namespace AADspTools
{
    inline float dBToGain(const float dBValue) { return std::pow(10.0f, dBValue / 20.0f); }
    inline float gainToDB(const float gainValue) { return 20.0f * std::log10(std::max(gainValue, 1.0e-6f)); }

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

        void prepare (double sampleRate, int maxBlockSize, int numChannels)
        {
            assert (sampleRate > 0.0 && maxBlockSize > 0 && numChannels > 0 && cutoffHz < sampleRate * factor * 0.5);

            maxSamples = maxBlockSize;
            upChannels.assign (static_cast<size_t> (numChannels), std::vector<float> (static_cast<size_t> (maxBlockSize * factor), 0.f));

            upPointers.clear();
            for (auto& channel : upChannels)
                upPointers.push_back (channel.data());

            designTaps(sampleRate * factor);
            channelStates.assign (static_cast<size_t> (numChannels), ChannelState {});
            for (auto& state : channelStates)
            {
                state.upHistory.prepare (evenTaps.size());
                state.downEvenHistory.prepare (evenTaps.size());
                state.downOddHistory.prepare (oddTaps.size());
            }
        }

        void reset()
        {
            for (auto& state : channelStates)
                for (auto* history : {&state.upHistory, &state.downEvenHistory, &state.downOddHistory})
                    history->reset();
        }

        int getLatencySamples() const { return latencySamples; }

        float* const* upsample (const float* const* channelsData, int numChannels, int numSamples)
        {
            assert (numChannels <= static_cast<int> (upChannels.size()) && numSamples <= maxSamples);
            for (int ch {0}; ch < numChannels; ++ch)
            {
                auto& history = channelStates[static_cast<size_t> (ch)].upHistory;
                auto* upData = upPointers[static_cast<size_t> (ch)];
                for (int n {0}; n < numSamples; ++n)
                {
                    history.push(channelsData[ch][n]);
                    const auto* window = history.oldestFirst();
                    const auto upGain = static_cast<float> (factor);
                    upData[factor * n] = upGain * std::inner_product(evenTaps.begin(), evenTaps.end(), window, 0.f);
                    upData[factor * n + 1] = upGain * std::inner_product(oddTaps.begin(), oddTaps.end(), window + 1, 0.f);
                }
            }
            return upPointers.data();
        }

        void downsample (float* const* channelsData, int numChannels, int numSamples)
        {
            assert (numChannels <= static_cast<int> (upChannels.size()) && numSamples <= maxSamples);
            for (int ch {0}; ch < numChannels; ++ch)
            {
                auto& state = channelStates[static_cast<size_t> (ch)];
                const auto* upData = upPointers[static_cast<size_t> (ch)];
                for (int n {0}; n < numSamples; ++n)
                {
                    state.downEvenHistory.push(upData[factor * n]);
                    const auto evenSum = std::inner_product(evenTaps.begin(), evenTaps.end(), state.downEvenHistory.oldestFirst(), 0.f);
                    const auto oddSum = std::inner_product(oddTaps.begin(), oddTaps.end(), state.downOddHistory.oldestFirst(), 0.f);
                    state.downOddHistory.push(upData[factor * n + 1]);
                    channelsData[ch][n] = evenSum + oddSum;
                }
            }
        }

    private:
        static constexpr double cutoffHz {21000.0};
        static constexpr double transitionWidthHz {1000.0};

        struct SampleHistory
        {
            void prepare(size_t newLength)
            {
                length = newLength;
                samples.assign(length * 2, 0.f);
                writeIndex = 0;
            }

            void reset()
            {
                std::fill(samples.begin(), samples.end(), 0.f);
                writeIndex = 0;
            }

            void push(float sample)
            {
                samples[writeIndex] = samples[writeIndex + length] = sample;
                writeIndex = (writeIndex + 1) % length;
            }

            const float* oldestFirst() const {return samples.data() + writeIndex;}

            std::vector<float> samples;
            size_t length {0};
            size_t writeIndex {0};
        };

        struct ChannelState
        {
            SampleHistory upHistory, downEvenHistory, downOddHistory;
        };

        void designTaps (double processingRate)
        {
            const auto minimumTaps = 4.0 * processingRate/transitionWidthHz;
            const auto halfLength = factor * static_cast<int>(std::ceil(minimumTaps / (2.0 * factor)));
            const auto numTaps = 2 * halfLength + 1;
            const auto normalisedCutoff = 2.0 * cutoffHz / processingRate;

            std::vector<double> taps (static_cast<size_t> (numTaps));
            for (int index {0}; index < numTaps; ++index)
            {
                const auto sincPhase = std::numbers::pi * normalisedCutoff * (index - halfLength);
                const auto sinc = index == halfLength ? 1.0 : std::sin (sincPhase) / sincPhase;
                // 4-term Blackman-Harris window
                const auto windowPhase = 2.0 * std::numbers::pi * index / (numTaps - 1);
                const auto window = 0.35875 - 0.48829 * std::cos (windowPhase) + 0.14128 * std::cos (2.0 * windowPhase) - 0.01168 * std::cos (3.0 * windowPhase);
                taps[static_cast<size_t> (index)] = normalisedCutoff * sinc * window;
            }

            const auto tapsSum = std::accumulate(taps.begin(), taps.end(), 0.0);
            evenTaps.clear();
            oddTaps.clear();
            for (size_t index {0}; index < taps.size(); ++index)
            {
                if (index % static_cast<size_t>(factor) == 0)
                    evenTaps.push_back(static_cast<float>(taps[index]/ tapsSum));
                else
                    oddTaps.push_back(static_cast<float>(taps[index]/ tapsSum));
            }

            latencySamples = 2 * halfLength / factor;
        }

        std::vector<std::vector<float>> upChannels;
        std::vector<float*> upPointers;
        std::vector<float> evenTaps, oddTaps;
        std::vector<ChannelState> channelStates;
        int maxSamples {0};
        int latencySamples {0};
    };
}