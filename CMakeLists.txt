cmake_minimum_required(VERSION 3.31)

project(ECoresExcluder)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILES
    Source/Logging/Logger.cpp
    Source/dllmain.cpp
    Source/ECoresExcluder.cpp
    Source/pch.cpp
)

set(HEADER_FILES
    Source/Logging/Defines.h
    Source/Logging/Logger.h
    Source/pch.h
    Source/framework.h
    Source/ECoresExcluder.h
)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(ECoresExcluder SHARED ${HEADER_FILES} ${SOURCE_FILES})