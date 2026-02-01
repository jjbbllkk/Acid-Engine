cmake_minimum_required(VERSION 3.22)

if(NOT "${METAMODULE_SDK_DIR}" STREQUAL "")
    message("METAMODULE_SDK_DIR set by CMake variable ${METAMODULE_SDK_DIR}")
elseif (DEFINED ENV{METAMODULE_SDK_DIR})
    set(METAMODULE_SDK_DIR "$ENV{METAMODULE_SDK_DIR}")
    message("METAMODULE_SDK_DIR set by environment variable ${METAMODULE_SDK_DIR}")
else()
    set(METAMODULE_SDK_DIR "${CMAKE_CURRENT_LIST_DIR}/../metamodule-plugin-sdk")
    message("METAMODULE_SDK_DIR set to default: ${METAMODULE_SDK_DIR}")
endif()

include(${METAMODULE_SDK_DIR}/plugin.cmake)

project(Vulpes79Ports
    VERSION 2.0.1
    DESCRIPTION "Vulpes79 ports"
    LANGUAGES C CXX ASM
)

add_library(Vulpes79Ports STATIC)

target_sources(Vulpes79Ports
    PRIVATE
    src/plugin.cpp
    src/AcidusVersio.cpp
    src/open303/fft4g.c
    src/open303/GlobalFunctions.cpp
    src/open303/rosic_AnalogEnvelope.cpp
    src/open303/rosic_BiquadFilter.cpp
    src/open303/rosic_BlendOscillator.cpp
    src/open303/rosic_Complex.cpp
    src/open303/rosic_DecayEnvelope.cpp
    src/open303/rosic_FourierTransformerRadix2.cpp
    src/open303/rosic_FunctionTemplates.cpp
    src/open303/rosic_LeakyIntegrator.cpp
    src/open303/rosic_MidiNoteEvent.cpp
    src/open303/rosic_MipMappedWaveTable.cpp
    src/open303/rosic_NumberManipulations.cpp
    src/open303/rosic_OnePoleFilter.cpp
    src/open303/rosic_Open303.cpp
    src/open303/rosic_RealFunctions.cpp
    src/open303/rosic_TeeBeeFilter.cpp
)

# Add includes and compile options for source repo
target_include_directories(Vulpes79Ports
    PRIVATE
    src/
)

add_compile_options(-fno-exceptions -fno-strict-aliasing)

# Call this to link and create the plugin file
create_plugin(
    SOURCE_LIB      Vulpes79Ports                                        # The cmake target name (defined in add_target)
    PLUGIN_NAME     Vulpes79Ports                                        # This must match the brand "slug" used in VCV Rack
    PLUGIN_JSON     ${CMAKE_CURRENT_LIST_DIR}/plugin.json                       # Path to the plugin.json file used by VCV Rack
    SOURCE_ASSETS   ${CMAKE_CURRENT_LIST_DIR}/assets                # Path to the assets/ dir containing the PNGs
    DESTINATION     ${CMAKE_CURRENT_LIST_DIR}/metamodule-plugins    # Path to where you want the plugin file output
)