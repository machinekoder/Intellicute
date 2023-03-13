cmake_minimum_required(VERSION 3.16)

project(Intellicute VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 COMPONENTS Core Multimedia Quick Network TextToSpeech REQUIRED)

qt_add_executable(intellicute
    main.cpp
    openai_request.cpp
    openai_message.cpp
    openai_message_model.cpp
    audio_recorder.cpp
    watson_tts_request.cpp
    fileio.cpp
    qml.qrc
    resources.qrc
)

if (ANDROID)
    set(ANDROID_SSL_PATH /home/alexander/bin/android/android_openssl/latest/arm/)
    set_property(TARGET intellicute PROPERTY QT_ANDROID_EXTRA_LIBS
        ${ANDROID_SSL_PATH}/libcrypto_1_1.so
        ${ANDROID_SSL_PATH}/libssl_1_1.so)
    set_property(TARGET intellicute PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
        ${PROJECT_SOURCE_DIR}/android)
endif()

qt_add_qml_module(intellicute
    URI Intellicute
    VERSION 1.0
)


set_target_properties(intellicute PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER intellicute.machinekoder.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(intellicute
    PRIVATE Qt6::Core Qt6::Multimedia Qt6::Quick Qt6::Network Qt6::TextToSpeech)

install(TARGETS intellicute
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
