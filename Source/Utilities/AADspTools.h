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

    // Sharp-knee soft clip shifted off-centre: adds even and odd harmonics, 0 in gives 0 out, unity gain for small signals
    class AsymmetricSoftClipper
    {
    public:
        void setParameters (const float newCeiling, const float newKneeSharpness, const float newBias, const float newDrive = 1.0f)
        {
            if (std::abs (newCeiling - ceiling) < tolerance
                && std::abs (newKneeSharpness - kneeSharpness) < tolerance
                && std::abs (newBias - bias) < tolerance
                && std::abs (newDrive - drive) < tolerance)
                return;

            ceiling = newCeiling;
            kneeSharpness = newKneeSharpness;
            bias = newBias;
            drive = newDrive;
            biasOffset = clip (bias);
            const auto biasRatio = std::pow (std::abs (bias / ceiling), kneeSharpness);
            smallSignalGain = std::pow (1.0f + biasRatio, -1.0f / kneeSharpness - 1.0f);
        }

        float process (const float x) const
        {
            return (clip (x * drive + bias) - biasOffset) / smallSignalGain / drive;
        }

    private:
        float clip (const float x) const
        {
            return x / std::pow (1.0f + std::pow (std::abs (x / ceiling), kneeSharpness), 1.0f / kneeSharpness);
        }

        static constexpr float tolerance {0.001f};
        float ceiling {1.0f};
        float kneeSharpness {2.0f};
        float bias {0.0f};
        float drive {1.0f};
        float biasOffset {0.0f};
        float smallSignalGain {1.0f};
    };

    // One-pole high-pass that removes the DC offset the asymmetric clipper creates
    class DCBlocker
    {
    public:
        void prepare (const double sampleRate, const double cutoffHz = 5.0)
        {
            coefficient = static_cast<float> (1.0 - 2.0 * std::numbers::pi * cutoffHz / sampleRate);
            reset();
        }

        void reset() { previousInput = previousOutput = 0.0f; }

        float process (const float x)
        {
            const auto y = x - previousInput + coefficient * previousOutput;
            previousInput = x;
            previousOutput = y;
            return y;
        }

    private:
        float coefficient {0.999f}, previousInput {0.0f}, previousOutput {0.0f};
    };

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
            assert (sampleRate > 0.0 && maxBlockSize > 0 && numChannels > 0);

            maxSamples = maxBlockSize;
            upChannels.assign (static_cast<size_t> (numChannels), std::vector<float> (static_cast<size_t> (maxBlockSize * factor), 0.f));

            upPointers.clear();
            for (auto& channel : upChannels)
                upPointers.push_back (channel.data());

            designTaps(sampleRate);
            channelStates.assign (static_cast<size_t> (numChannels), ChannelState {});
            for (auto& state : channelStates)
            {
                state.upHistory.prepare (2 * downTaps.size());
                state.downEvenHistory.prepare (2 * downTaps.size());
                state.downOddHistory.prepare (downTaps.size());
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
                    upData[factor * n] = pairedSum (window, upTaps);
                    upData[factor * n + 1] = window[downTaps.size()];
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
                    const auto evenSum = pairedSum (state.downEvenHistory.oldestFirst(), downTaps);
                    const auto centreSample = 0.5f * state.downOddHistory.oldestFirst()[0];
                    state.downOddHistory.push(upData[factor * n + 1]);
                    channelsData[ch][n] = evenSum + centreSample;
                }
            }
        }

    private:
        static constexpr double passbandHz {20000.0};
        static constexpr double minimumTransitionHz {2000.0};
        static constexpr double transitionScale {7.5};

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

        static float pairedSum (const float* window, const std::vector<float>& taps)
        {
            const auto lastIndex = 2 * taps.size() - 1;
            float sum {0.f};
            for (size_t i {0}; i < taps.size(); ++i)
                sum += taps[i] * (window[i] + window[lastIndex - i]);
            return sum;
        }

        void designTaps (double hostRate)
        {
            const auto processingRate = hostRate * factor;
            const auto transitionHz = std::max (hostRate - 2.0 * passbandHz, minimumTransitionHz);
            const auto minimumTaps = transitionScale * processingRate / transitionHz;
            const auto order = std::max (2, static_cast<int>(std::ceil ((minimumTaps - 3.0) / 4.0)));
            const auto numTaps = 4 * order + 3;
            const auto centre = 2 * order + 1;

            std::vector<double> taps (static_cast<size_t> (order + 1));
            double sumOfEvenTaps {0.0};
            for (int i {0}; i <= order; ++i)
            {
                const auto offset = 2 * i - centre;
                const auto ideal = std::sin (std::numbers::pi * offset / 2.0) / (std::numbers::pi * offset);
                // 4-term Blackman-Harris window
                const auto windowPhase = 2.0 * std::numbers::pi * (2 * i) / (numTaps - 1);
                const auto window = 0.35875 - 0.48829 * std::cos (windowPhase) + 0.14128 * std::cos (2.0 * windowPhase) - 0.01168 * std::cos (3.0 * windowPhase);
                taps[static_cast<size_t> (i)] = ideal * window;
                sumOfEvenTaps += 2.0 * taps[static_cast<size_t> (i)];
            }

            const auto gain = 0.5 / sumOfEvenTaps;
            downTaps.clear();
            upTaps.clear();
            for (const auto tap : taps)
            {
                downTaps.push_back(static_cast<float>(tap * gain));
                upTaps.push_back(static_cast<float>(2.0 * tap * gain));
            }

            latencySamples = centre;
        }

        std::vector<std::vector<float>> upChannels;
        std::vector<float*> upPointers;
        std::vector<float> downTaps, upTaps;
        std::vector<ChannelState> channelStates;
        int maxSamples {0};
        int latencySamples {0};
    };
}