#pragma once

//#include <JuceHeader.h>

// This namespace holds ALL parameter ID strings in one place
namespace ParamNames
{
// === 1176 ===
inline juce::String get1176InputName() {return "1176Input";}
inline juce::String get1176OutputName() {return "1176Output";}
inline juce::String get1176AttackName() {return "1176Attack";}
inline juce::String get1176ReleaseName() {return "1176Release";}
inline juce::String get1176RatioName() {return "1176Ratio";}
inline juce::String get1176MixName() {return "1176Mix";}
inline juce::String get1176BypassName() {return "1176Bypass";}

// === LA2a ===
inline juce::String getLA2AGainName() {return "LA2AGain";}
inline juce::String getLA2APeakReductionName() {return "LA2APeakReduction";}
inline juce::String getLA2ALimitCompressModeName() {return "LA2AMode";}
inline juce::String getLA2ABypassName() {return "LA2ABypass";}

// === In & Out Gain ===
inline juce::String getInGainName() {return "InGain";}
inline juce::String getOutGainName() {return "OutGain";}

// === General ===
inline juce::String getGeneralBypassName()  { return "PluginBypass"; }
inline juce::String getReverseSignalOrderName() {return "ReverseSignalOrder";}
}
