#pragma once

#include <string>
#include <vector>

#ifdef ILLUMINATION_ENGINE_VULKAN
#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#ifndef VMA_IMPLEMENTATION
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#endif
#endif

#ifdef ILLUMINATION_ENGINE_OPENGL
#ifndef GLEW_IMPLEMENTATION
#define GLEW_IMPLEMENTATION
#include <GL/glew.h>
#endif
#endif

#include <GLFW/glfw3.h>

#include "Settings.hpp"

class RenderEngineLink{
public:
    class API{
    public:
        std::string name{"OpenGL"};
        Version version{};

        Version getVersion() {
            #ifdef ILLUMINATION_ENGINE_VULKAN
            if (name == "Vulkan") {
                auto vkEnumerateDeviceInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"));
                if (vkEnumerateDeviceInstanceVersion == nullptr) { version = Version{"1.0.0"}; } else {
                    uint32_t instanceVersion;
                    vkEnumerateDeviceInstanceVersion(&instanceVersion);
                    version = Version{VK_VERSION_MAJOR(instanceVersion), VK_VERSION_MINOR(instanceVersion), VK_VERSION_PATCH(instanceVersion)};
                }
            }
            #endif
            #ifdef ILLUMINATION_ENGINE_OPENGL
            if (name == "OpenGL") {
                if (GLEW_VERSION_1_1) { version = Version{"1.1.0"}; }
                if (GLEW_VERSION_1_2) { version = Version{"1.2.0"}; }
                if (GLEW_VERSION_1_2_1) { version = Version{"1.2.1"}; }
                if (GLEW_VERSION_1_3) { version = Version{"1.3.0"}; }
                if (GLEW_VERSION_1_4) { version = Version{"1.4.0"}; }
                if (GLEW_VERSION_1_5) { version = Version{"1.5.0"}; }
                if (GLEW_VERSION_2_0) { version = Version{"2.0.0"}; }
                if (GLEW_VERSION_2_1) { version = Version{"2.1.0"}; }
                if (GLEW_VERSION_3_0) { version = Version{"3.0.0"}; }
                if (GLEW_VERSION_3_1) { version = Version{"3.1.0"}; }
                if (GLEW_VERSION_3_2) { version = Version{"3.2.0"}; }
                if (GLEW_VERSION_3_3) { version = Version{"3.3.0"}; }
                if (GLEW_VERSION_4_0) { version = Version{"4.0.0"}; }
                if (GLEW_VERSION_4_1) { version = Version{"4.1.0"}; }
                if (GLEW_VERSION_4_2) { version = Version{"4.2.0"}; }
                if (GLEW_VERSION_4_3) { version = Version{"4.3.0"}; }
                if (GLEW_VERSION_4_4) { version = Version{"4.4.0"}; }
                if (GLEW_VERSION_4_5) { version = Version{"4.5.0"}; }
                if (GLEW_VERSION_4_6) { version = Version{"4.6.0"}; }
                if (version.name == "0.0.0") {
                    glfwInit();
                    GLFWwindow *temporaryWindow = glfwCreateWindow(1, 1, "Gathering OpenGL Data...", nullptr, nullptr);
                    glfwMakeContextCurrent(temporaryWindow);
                    version = Version{std::string(reinterpret_cast<const char *const>(glGetString(GL_VERSION)))};
                    glfwDestroyWindow(temporaryWindow);
                    glfwTerminate();
                }
            }
            #endif
            return version;
        }
    };

    class PhysicalDevice {
    public:
        struct APIComponents {
            #ifdef ILLUMINATION_ENGINE_VULKAN
            // Extension Properties
            VkPhysicalDeviceMemoryProperties memoryProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2};
            void *pNextHighestProperty = &memoryProperties;

            // Device Features
            VkPhysicalDeviceFeatures features{};

            // Extension Features
            // NOTE: Ray tracing features are on the bottom of the pNext stack so that a pointer to higher up on the stack can grab only the structures supported by RenderDoc.
            VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptorIndexingFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT};
            // All the below are ray tracing features, and cannot be loaded by RenderDoc.
            VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR, &descriptorIndexingFeatures};
            VkPhysicalDeviceBufferDeviceAddressFeaturesEXT bufferDeviceAddressFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES, &accelerationStructureFeatures};
            VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR, &bufferDeviceAddressFeatures};
            VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR, &rayQueryFeatures};
            // Automatically checks for RenderDoc capture, and simulates a device that does not support anything that RenderDoc does not support.
            void *pNextHighestFeature = std::getenv("ENABLE_VULKAN_RENDERDOC_CAPTURE") != nullptr ? static_cast<void *>(&descriptorIndexingFeatures) : static_cast<void *>(&rayTracingPipelineFeatures);
            #endif
            // Engine Features
            bool anisotropicFiltering{false};
            bool msaaSmoothing{false};
            bool rayTracing{false};
        };

        class Info {
        public:
            API *api;
            #ifdef ILLUMINATION_ENGINE_VULKAN
            VkPhysicalDeviceProperties properties{};
            #endif
            std::string vendor{};
            std::string name{};

            Info generateInfo() {
            #ifdef ILLUMINATION_ENGINE_VULKAN
                if (api->name == "Vulkan") {
                    api->version = Version{VK_VERSION_MAJOR(properties.apiVersion), VK_VERSION_MINOR(properties.apiVersion), VK_VERSION_PATCH(properties.apiVersion)};
                    vendor = std::to_string(properties.vendorID);
                    name = properties.deviceName;
                    name = name.substr(name.find_first_of(' ') + 1, name.length() - name.find_first_of(' ') - 1);
                }
                #endif
                #ifdef ILLUMINATION_ENGINE_OPENGL
                if (api->name == "OpenGL") {
                    vendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
                    name = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
                    name = name.substr(name.find_first_of(' ') + 1, name.find_first_of('/') - name.find_first_of(' ') - 1);
                }
                #endif
                return *this;
            }
        };

        Info info;
        APIComponents enabledAPIComponents;
        APIComponents supportedAPIComponents;

        #ifdef ILLUMINATION_ENGINE_VULKAN

        VkBool32 enableFeature(VkBool32 *feature) {
            *feature = *testFeature(feature);
            return *feature;
        }

        std::vector<VkBool32> enableFeature(const std::vector<VkBool32 *> &features) {
            std::vector<VkBool32> results{VK_FALSE};
            results.reserve(static_cast<unsigned int>(features.size() + 1));
            results.push_back(VK_FALSE);
            for (VkBool32 *feature: features) { results.push_back(enableFeature(feature)); }
            for (VkBool32 result: results) { if (!result) { return results; }}
            results[0] = VK_TRUE;
            return results;
        }

        VkBool32 *testFeature(const VkBool32 *feature) {
            return feature - (VkBool32 *)&enabledAPIComponents + (VkBool32 *)&supportedAPIComponents;
        }

        std::vector<VkBool32> testFeature(const std::vector<VkBool32 *> &features) {
            std::vector<VkBool32> results{VK_FALSE};
            results.reserve(static_cast<unsigned int>(features.size() + 1));
            results.push_back(VK_FALSE);
            for (VkBool32 *feature: features) { results.push_back(*testFeature(feature)); }
            for (VkBool32 result: results) { if (!result) { return results; }}
            results[0] = VK_TRUE;
            return results;
        }
        #endif
    };

    struct Created{
        bool glfw{};
        bool window{};
        bool link{};
        #ifdef ILLUMINATION_ENGINE_VULKAN
        bool instance{};
        bool device{};
        bool surface{};
        #endif
    };

    void create() {
        physicalDevice.info.api = &api;
        #ifdef ILLUMINATION_ENGINE_VULKAN
        if (api.name == "Vulkan") {
            vkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(device.device, "vkGetBufferDeviceAddressKHR"));
            vkCmdBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(device.device, "vkCmdBuildAccelerationStructuresKHR"));
            vkCreateAccelerationStructureKHR = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(vkGetDeviceProcAddr(device.device, "vkCreateAccelerationStructureKHR"));
            vkDestroyAccelerationStructureKHR = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(vkGetDeviceProcAddr(device.device, "vkDestroyAccelerationStructureKHR"));
            vkGetAccelerationStructureBuildSizesKHR = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(vkGetDeviceProcAddr(device.device, "vkGetAccelerationStructureBuildSizesKHR"));
            vkGetAccelerationStructureDeviceAddressKHR = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(vkGetDeviceProcAddr(device.device, "vkGetAccelerationStructureDeviceAddressKHR"));
            vkAcquireNextImageKhr = reinterpret_cast<PFN_vkAcquireNextImageKHR>(vkGetDeviceProcAddr(device.device, "vkAcquireNextImageKHR"));
            VkPhysicalDeviceProperties2 physicalDeviceProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
            vkGetPhysicalDeviceProperties2(device.physical_device.physical_device, &physicalDeviceProperties);
            physicalDevice.info.properties = physicalDeviceProperties.properties;
            VkPhysicalDeviceFeatures2 physicalDeviceFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
            physicalDeviceFeatures.pNext = physicalDevice.supportedAPIComponents.pNextHighestFeature;
            vkGetPhysicalDeviceFeatures2(device.physical_device.physical_device, &physicalDeviceFeatures);
            physicalDevice.supportedAPIComponents.features = physicalDeviceFeatures.features;
            vkGetPhysicalDeviceMemoryProperties(device.physical_device.physical_device, &physicalDevice.supportedAPIComponents.memoryProperties);
        }
        #endif
    }

    API api;
    PhysicalDevice physicalDevice;
    Settings settings;
    GLFWwindow *window{};
    bool framebufferResized{false};
    Created created{};
#ifdef ILLUMINATION_ENGINE_VULKAN
    vkb::Device device{};
    vkb::Instance instance{};
    VmaAllocator allocator{};
    VkSurfaceKHR surface{};
    PFN_vkGetBufferDeviceAddress vkGetBufferDeviceAddressKHR{};
    PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR{};
    PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR{};
    PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR{};
    PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR{};
    PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR{};
    PFN_vkAcquireNextImageKHR vkAcquireNextImageKhr{};
    #endif

    ~RenderEngineLink() {
        #ifdef ILLUMINATION_ENGINE_VULKAN
        if (api.name == "Vulkan") {
            vkDestroySurfaceKHR(instance.instance, surface, nullptr);
            vkb::destroy_device(device);
            vkb::destroy_instance(instance);
        }
        #endif
    }
};