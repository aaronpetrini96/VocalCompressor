cmake_minimum_required(VERSION 3.22)

# Must be set before project()
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS version")

project(VocalCompressor VERSION 1.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ---------------------------------------------------------------------------
# JUCE
# Use, in order:
#   1. -DJUCE_PATH=/path/to/JUCE      (an existing JUCE checkout)
#   2. ./JUCE                         (a submodule or local copy)
#   3. JUCE 8.0.11 downloaded automatically with FetchContent
# ---------------------------------------------------------------------------
set(JUCE_PATH "" CACHE PATH "Path to an existing JUCE checkout (optional)")

if(JUCE_PATH AND EXISTS "${JUCE_PATH}/CMakeLists.txt")
    add_subdirectory("${JUCE_PATH}" JUCE EXCLUDE_FROM_ALL)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
    add_subdirectory(JUCE EXCLUDE_FROM_ALL)
else()
    include(FetchContent)
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 8.0.11
        GIT_SHALLOW ON
    )
    FetchContent_MakeAvailable(JUCE)
endif()

# ---------------------------------------------------------------------------
# Plugin
# ---------------------------------------------------------------------------
juce_add_plugin(VocalCompressor
    COMPANY_NAME "Aaron Petrini"
    BUNDLE_ID com.AaronPetrini.VocalCompressor
    PRODUCT_NAME "VocalCompressor"
    VERSION ${PROJECT_VERSION}
    PLUGIN_MANUFACTURER_CODE Apau
    PLUGIN_CODE Vcmp
    FORMATS VST3 AU Standalone
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES Fx Dynamics
    AU_MAIN_TYPE kAudioUnitType_Effect
)

target_sources(VocalCompressor PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/DSP/1176Compressor.cpp
    Source/DSP/LA2ACompressor.cpp
    Source/GUI/ImageKnob.cpp
    Source/GUI/ImageKnobLookAndFeel.cpp
    Source/Utilities/PresetManagerV2.cpp
)

# Images are embedded in the plugin (BinaryData::name_png)
juce_add_binary_data(VocalCompressorAssets SOURCES
        Source/Assets/Images/Bypass.png
        Source/Assets/Images/clearSilverBackground.png
        Source/Assets/Images/darkSilverBackground.png
        Source/Assets/Images/screw25x25.png
        Source/Assets/Images/slotScrew256.png
        Source/Assets/Images/spriteSheet1176.png
        Source/Assets/Images/spriteSheetLa2a.png
        Source/Assets/Images/switchDOWN.png
        Source/Assets/Images/switchUP.png
)

target_include_directories(VocalCompressor PRIVATE Source)

target_compile_definitions(VocalCompressor PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_STRICT_REFCOUNTEDPOINTER=1
)

target_link_libraries(VocalCompressor
    PRIVATE
        VocalCompressorAssets
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# The code includes <JuceHeader.h>, so generate it for CMake builds
# (call after target_link_libraries so it picks up the linked modules and assets)
juce_generate_juce_header(VocalCompressor)
