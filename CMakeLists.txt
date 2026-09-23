cmake_minimum_required(VERSION 3.22)
project(FoxStudioDaw VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_gui_app(FoxStudioDaw
    PRODUCT_NAME "FOX STUDIO DAW"
    COMPANY_NAME "Fox Studio"
    BUNDLE_ID com.foxstudio.daw
    VERSION "${PROJECT_VERSION}")

juce_generate_juce_header(FoxStudioDaw)

target_sources(FoxStudioDaw PRIVATE
    Source/Main.cpp
    Source/MainComponent.cpp
    Source/FoxStudioEngine.cpp
    Source/TransportBar.cpp
    Source/TrackList.cpp
    Source/TimelineComponent.cpp
    Source/MixerComponent.cpp
    Source/PianoRollComponent.cpp
    Source/DSP/DspChain.cpp
    Source/DSP/GainProcessor.cpp
    Source/DSP/EqProcessor.cpp
    Source/DSP/CompressorProcessor.cpp
    Source/DSP/LimiterProcessor.cpp
    Source/DSP/SaturationProcessor.cpp)

target_compile_definitions(FoxStudioDaw PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="FOX STUDIO DAW"
    JUCE_APPLICATION_VERSION_STRING="${PROJECT_VERSION}"
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0)

target_link_libraries(FoxStudioDaw PRIVATE
    juce::juce_audio_utils
    juce::juce_gui_extra
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags)

if(MSVC)
    target_compile_options(FoxStudioDaw PRIVATE /W4 /permissive-)
else()
    target_compile_options(FoxStudioDaw PRIVATE -Wall -Wextra -Wpedantic)
endif()
