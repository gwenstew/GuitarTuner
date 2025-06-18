
#current version on machine 3.31.5; unsure what is best
cmake_minimum_required(VERSION 3.22)

#specify project name
project(GuitarTuner VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)

#download cpm if not installed
set(CPM_DOWNLOAD_VERSION 0.34.0)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if (NOT EXISTS ${CPM_DOWNLOAD_LOCATION})
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif ()

#set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

#using cpm as package manager
include(${CPM_DOWNLOAD_LOCATION})

#add JUCE through CPM
CPMAddPackage(
    NAME juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG origin/master
)

set(SourceFiles
        src/Main.cpp
        src/MainComponent.cpp
        src/MainComponent.h
)

#add executable target and set preferences for application
juce_add_gui_app(${PROJECT_NAME}
    COMPANY_NAME gwenstewart
    PRODUCT_NAME "Guitar Tuner"
)

#organize source files in IDE
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SourceFiles})

#add source files to be built into target
target_sources(${PROJECT_NAME} PRIVATE ${SourceFiles})

#preprocessor definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
        #do not need web browser support or network requests
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        #setting macros for resuability
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:${PROJECT_NAME},JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:${PROJECT_NAME},JUCE_VERSION>"
)

#JUCE libraries needed for project
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        #binary data for later
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)


