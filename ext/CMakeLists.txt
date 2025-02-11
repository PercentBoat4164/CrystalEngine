# CPM Options
set(CPM_SOURCE_CACHE ${CMAKE_CURRENT_SOURCE_DIR}/deps)
set(CPM_USE_LOCAL_PACKAGES ${IE_PREFER_LOCAL_LIBS})

function(download_with_cache ARG_DOWNLOAD_URL ARG_OUTPUT_FILE)
    file(DOWNLOAD ${ARG_DOWNLOAD_URL} STATUS _DOWNLOAD_RESULTS)
    list(GET _DOWNLOAD_RESULTS 0 _ERROR_CODE)
    if (_ERROR_CODE GREATER 0)
        list(GET _DOWNLOAD_RESULTS 1 _ERROR_MESSAGE)
        message(WARNING "Could not get CPM.cmake because: ${_ERROR_MESSAGE}")
    else ()
        file(DOWNLOAD ${ARG_DOWNLOAD_URL} ${ARG_OUTPUT_FILE})
    endif ()
endfunction()

# Download the latest CPM.cmake file

download_with_cache(https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake ${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

# All options for Dependencies should go here
# AssImp Options
set(ASSIMP_HUNTER_ENABLED OFF)
set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
if (APPLE)
    set(ASSIMP_BUILD_FRAMEWORK OFF)
endif ()
set(ASSIMP_DOUBLE_PRECISION OFF)
set(ASSIMP_OPT_BUILD_PACKAGES OFF)
set(ASSIMP_ANDROID_JNIIOSYSTEM OFF)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_ZLIB ON)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT ON)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_SAMPLES OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_COVERALLS OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
set(ASSIMP_ASAN OFF)
set(ASSIMP_UBSAN OFF)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF)
set(ASSIMP_IGNORE_GIT_HASH ON)
set(ASSIMP_INSTALL_PDB OFF)
set(USE_STATIC_CRT OFF)
set(ASSIMP_BUILD_DRACO OFF)  # Fails as of AssImp v5.2.5
if (WINDOWS)
    set(ASSIMP_BUILD_ASSIMP_VIEW OFF)
endif ()

# GLM Options
# Uncomment options that should be enabled.
#add_compile_definitions(GLM_FORCE_MESSAGES)
#add_compile_definitions(GLM_FORCE_COMPILER_UNKNOWN)
#add_compile_definitions(GLM_FORCE_ARCH_UNKNOWN)
#add_compile_definitions(GLM_FORCE_CXX_UNKNOWN)
#add_compile_definitions(GLM_FORCE_CXX)
#add_compile_definitions(GLM_FORCE_EXPLICIT_CTOR)
#add_compile_definitions(GLM_FORCE_INLINE)
#add_compile_definitions(GLM_FORCE_ALIGNED_GENTYPES)
#add_compile_definitions(GLM_FORCE_DEFAULT_ALIGNED_GENTYPES)
#add_compile_definitions(GLM_FORCE_INTRINSICS)
#add_compile_definitions(GLM_FORCE_PRECISION_LOWP_FLOAT)
#add_compile_definitions(GLM_FORCE_PRECISION_MEDIUMP_FLOAT)
#add_compile_definitions(GLM_FORCE_PRECISION_HIGHP_FLOAT)
#add_compile_definitions(GLM_FORCE_PRECISION_LOWP_DOUBLE)
#add_compile_definitions(GLM_FORCE_PRECISION_MEDIUMP_DOUBLE)
#add_compile_definitions(GLM_FORCE_PRECISION_HIGHP_DOUBLE)
#add_compile_definitions(GLM_FORCE_PRECISION_LOWP_INT)
#add_compile_definitions(GLM_FORCE_PRECISION_MEDIUMP_INT)
#add_compile_definitions(GLM_FORCE_PRECISION_HIGHP_INT)
#add_compile_definitions(GLM_FORCE_PRECISION_LOWP_UINT)
#add_compile_definitions(GLM_FORCE_PRECISION_MEDIUMP_UINT)
#add_compile_definitions(GLM_FORCE_PRECISION_HIGHP_UINT)
#add_compile_definitions(GLM_FORCE_SINGLE_ONLY)
#add_compile_definitions(GLM_FORCE_SWIZZLE)
#add_compile_definitions(GLM_FORCE_XYZW_ONLY)
#add_compile_definitions(GLM_FORCE_LEFT_HANDED)
#add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE)
#add_compile_definitions(GLM_FORCE_SIZE_T_LENGTH)
#add_compile_definitions(GLM_FORCE_UNRESTRICTED_GENTYPE)
#add_compile_definitions(GLM_FORCE_SILENT_WARNINGS)
#add_compile_definitions(GLM_FORCE_QUAT_DATA_WXYZ)


# GLEW Options
set(GLEW_VERBOSE OFF)
set(GLEW_USE_STATIC_LIBS NOT ${BUILD_SHARED_LIBS})

# GLFW Options
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_INSTALL OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_VULKAN_STATIC OFF)
if (WINDOWS)
    set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF)
    set(GLFW_USE_HYBRID_HPG OFF)
endif ()
if (LINUX)
    set(GLFW_USE_WAYLAND OFF)
endif ()

# SpdLog Options

# VkBootstrap Options

# VMA Options
set(VMA_RECORDING_ENABLED)
set(VMA_USE_STL_CONTAINERS)
set(VMA_STATIC_VULKAN_FUNCTIONS)
set(VMA_DYNAMIC_VULKAN_FUNCTIONS)
set(VMA_DEBUG_ALWAYS_DEDICATED_MEMORY)
set(VMA_DEBUG_INITIALIZE_ALLOCATIONS)
set(VMA_DEBUG_GLOBAL_MUTEX)
set(VMA_DEBUG_DONT_EXCEED_MAX_MEMORY_ALLOCATION_COUNT)

# Shaderc Options
set(SHADERC_ENABLE_WGSL_OUTPUT OFF)
set(SHADERC_SKIP_TESTS ON)
set(SHADERC_SKIP_EXAMPLES ON)
set(SHADERC_SKIP_COPYRIGHT_CHECK OFF)
set(SHADERC_SKIP_INSTALL ON)

# CCache Options
if (NOT DEFINED USE_CCACHE)
    set(USE_CCACHE ON)
endif ()
set(CCACHE_VERSION 4.8.1)

include(../cmake/Ccache.cmake)

# Add all packages
find_package(OpenGL REQUIRED)
find_package(Vulkan REQUIRED shaderc_combined)

set(OpenGL_GL_PREFERENCE "GLVND")
CPMAddPackage(
        NAME GLEW
        URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz
        VERSION 2.2.0
        SOURCE_SUBDIR build/cmake
)
CPMAddPackage("gh:assimp/assimp@5.2.5")
CPMAddPackage("gh:g-truc/glm#master")
CPMAddPackage("gh:glfw/glfw#3.3.8")
CPMAddPackage("gh:gabime/spdlog@1.11.0")
CPMAddPackage("gh:charles-lunarg/vk-bootstrap@0.7")
CPMAddPackage("gh:GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator@3.0.1")
CPMAddPackage("gh:KhronosGroup/SPIRV-Cross#main")  # This should be main. The latest guaranteed working version is kept there. All tags are for older versions.

add_library(EXT_AssImp dummy.cpp)
add_library(EXT_SpdLog dummy.cpp)
add_library(EXT_OpenGL dummy.cpp)
add_library(EXT_Vulkan dummy.cpp)
add_library(INT_src dummy.cpp)
target_link_libraries(EXT_AssImp PUBLIC assimp)
target_link_libraries(EXT_SpdLog PUBLIC spdlog)
target_link_libraries(EXT_OpenGL PUBLIC OpenGL::GL glew_s glfw glm::glm)
target_include_directories(EXT_OpenGL PUBLIC ${GLEW_SOURCE_DIR}/include)
target_link_libraries(EXT_Vulkan PUBLIC Vulkan::Vulkan Vulkan::shaderc_combined vk-bootstrap VulkanMemoryAllocator glfw glm::glm ${Vulkan_shaderc_combined_LIBRARY} spirv-cross-reflect spirv-cross-cpp)
target_include_directories(INT_src PUBLIC ${IE_BASE_DIR}/src)
