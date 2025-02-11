cmake_minimum_required(VERSION 3.14 FATAL_ERROR)  # 3.14 is required for CPM
project(IlluminationEngine VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 23)

set(BUILD_SHARED_LIBS OFF)

# Detect compiler and OS
if (${CMAKE_CXX_COMPILER_FRONTEND_VARIANT})
    message("Using ${CMAKE_CXX_COMPILER_FRONTEND_VARIANT} ${CMAKE_CXX_COMPILER_ID} v${CMAKE_CXX_COMPILER_VERSION} on ${CMAKE_SYSTEM_NAME}.")
else ()
    message("Using ${CMAKE_CXX_COMPILER_ID} v${CMAKE_CXX_COMPILER_VERSION} on ${CMAKE_SYSTEM_NAME}.")
endif ()
add_compile_definitions("COMPILER=${CMAKE_CXX_COMPILER_ID}")
add_compile_definitions("OS=${CMAKE_SYSTEM_NAME}")
add_compile_definitions("COMPILER_FRONTEND=${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}")


set(IE_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})  # Directory Illumination Engine is being built in
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)  # Directory to put library files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)  # Directory to put runtime files
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)  # Directory to put static libraries

add_custom_target(CopyResources)
add_custom_command(
        TARGET CopyResources
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${IE_BASE_DIR}/res
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res &
        DEPENDS ${IE_BASE_DIR}/res
        COMMENT "Copying resource files...")

set(IE_PREFER_LOCAL_LIBS ON)

# Compilation and linking options
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_definitions("Clang")

    # Added to prevent known errors from clouding the log.
    add_compile_options("-Wno-deprecated-non-prototype")
    add_compile_options("-Wno-nullability-completeness")

    if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        add_compile_options("/EHsc" "-Wno-unused-command-line-argument" "-Wno-error=unused-function")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (WIN32)
        set(BUILD_SHARED_LIBS ON)
    endif ()

    add_compile_definitions("GCC")
    add_compile_options("-Wno-unknown-pragmas")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_definitions("MSVC")

    # Eliminate the unnecessary warnings.
    string(REGEX REPLACE " /W[3|4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} CACHE STRING "" FORCE)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    add_compile_definitions("AppleClang")

    # Added to prevent known errors from clouding the log.
    add_compile_options("-Wno-nullability-completeness")
elseif ()
    message(WARNING "Unsupported compiler in use: ${CMAKE_CXX_COMPILER_ID}")
endif ()

# Compilation Options for similar interface compilers
if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC" OR CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/Zi" "/Od")  # Complete debugging information with no optimization
else ()
    add_compile_options("-g" "-O0")  # Debugging information with no optimization
endif ()

add_subdirectory(ext)  # Generate external dependencies from source
add_subdirectory(src)  # Generate Illumination Engine from source
