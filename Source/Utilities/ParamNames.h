#pragma once
#include <juce_core/juce_core.h>

namespace ParamNames
{
    struct ParamName
    {
        juce::String id;
        juce::String name;
    };

    // === FET-76 ===
    inline ParamName getFET76InputNames() { return {"FET76Input", "FET-76 Input"}; }
    inline ParamName getFET76OutputNames() { return {"FET76Output", "FET-76 Output"}; }
    inline ParamName getFET76AttackNames() { return {"FET76Attack", "FET-76 Attack"}; }
    inline ParamName getFET76ReleaseNames() { return {"FET76Release", "FET-76 Release"}; }
    inline ParamName getFET76RatioNames() { return {"FET76Ratio", "FET-76 Ratio"}; }
    inline ParamName getFET76MixNames() { return {"FET76Mix", "FET-76 Mix"}; }
    inline ParamName getFET76BypassNames() { return {"FET76Bypass", "FET-76 Bypass"}; }

    // === OPT-2A ===
    inline ParamName getOPT2AGainNames() { return {"OPT2AGain", "OPT-2A Gain"}; }
    inline ParamName getOPT2APeakReductionNames() { return {"OPT2APeakReduction", "OPT-2A Peak Reduction"}; }
    inline ParamName getOPT2AModeNames() { return {"OPT2AMode", "OPT-2A Mode"}; }
    inline ParamName getOPT2AMixNames() { return {"OPT2AMix", "OPT-2A Mix"}; }
    inline ParamName getOPT2ABypassNames() { return {"OPT2ABypass", "OPT-2A Bypass"}; }

    // === General ===
    inline ParamName getPluginBypassNames() { return {"PluginBypass", "Plugin Bypass"}; }
    inline ParamName getReverseSignalOrderNames() { return {"ReverseSignalOrder", "Reverse Signal Flow Order"}; }
}
