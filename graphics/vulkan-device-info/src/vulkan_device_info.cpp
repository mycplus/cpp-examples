// Creates a Vulkan instance, lists every GPU (physical device) and its queue
// families, and reports whether it supports hardware ray tracing.
// Companion code for the MYCPLUS article on AAA game graphics in C++.
#include <vulkan/vulkan.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

static const char* type_name(VkPhysicalDeviceType t) {
    switch (t) {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "integrated GPU";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return "discrete GPU";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: return "virtual GPU";
        case VK_PHYSICAL_DEVICE_TYPE_CPU: return "CPU (software)";
        default: return "other";
    }
}

static bool has_extension(VkPhysicalDevice gpu, const char* name) {
    uint32_t count = 0;
    vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> exts(count);
    vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, exts.data());
    for (const auto& e : exts)
        if (std::strcmp(e.extensionName, name) == 0) return true;
    return false;
}

int main() {
    // 1. Describe the application and the Vulkan version it needs.
    VkApplicationInfo app{};
    app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app.pApplicationName = "mycplus-vulkan-device-info";
    app.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    app.apiVersion = VK_API_VERSION_1_1;

    // 2. Create the instance: the connection between the program and the driver(s).
    VkInstanceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pApplicationInfo = &app;

    VkInstance instance = VK_NULL_HANDLE;
    if (VkResult r = vkCreateInstance(&info, nullptr, &instance); r != VK_SUCCESS) {
        std::fprintf(stderr, "vkCreateInstance failed: %d (is a Vulkan driver installed?)\n",
                     static_cast<int>(r));
        return 1;
    }

    // 3. Ask how many GPUs there are, then fetch them (Vulkan's two-call pattern).
    uint32_t gpu_count = 0;
    vkEnumeratePhysicalDevices(instance, &gpu_count, nullptr);
    std::vector<VkPhysicalDevice> gpus(gpu_count);
    vkEnumeratePhysicalDevices(instance, &gpu_count, gpus.data());
    std::printf("Vulkan devices: %u\n", gpu_count);

    for (VkPhysicalDevice gpu : gpus) {
        VkPhysicalDeviceProperties props{};
        vkGetPhysicalDeviceProperties(gpu, &props);
        std::printf("\n%s (%s)\n", props.deviceName, type_name(props.deviceType));
        std::printf("  Vulkan API %u.%u.%u\n", VK_API_VERSION_MAJOR(props.apiVersion),
                    VK_API_VERSION_MINOR(props.apiVersion), VK_API_VERSION_PATCH(props.apiVersion));
        std::printf("  max 2D texture: %u x %u\n", props.limits.maxImageDimension2D,
                    props.limits.maxImageDimension2D);

        // 4. Queue families: the kinds of work this GPU accepts.
        uint32_t qcount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &qcount, nullptr);
        std::vector<VkQueueFamilyProperties> families(qcount);
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &qcount, families.data());
        for (uint32_t i = 0; i < qcount; ++i) {
            const VkQueueFlags f = families[i].queueFlags;
            std::printf("  queue family %u: %u queue(s)%s%s%s\n", i, families[i].queueCount,
                        (f & VK_QUEUE_GRAPHICS_BIT) ? " graphics" : "",
                        (f & VK_QUEUE_COMPUTE_BIT) ? " compute" : "",
                        (f & VK_QUEUE_TRANSFER_BIT) ? " transfer" : "");
        }

        // 5. Hardware ray tracing is an optional device extension.
        const bool rt = has_extension(gpu, "VK_KHR_ray_tracing_pipeline") &&
                        has_extension(gpu, "VK_KHR_acceleration_structure");
        std::printf("  ray tracing pipeline: %s\n", rt ? "yes" : "no");
    }

    vkDestroyInstance(instance, nullptr);
    return gpu_count > 0 ? 0 : 2;
}
