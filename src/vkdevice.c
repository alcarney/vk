#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>

const char* vkPhysicalDeviceType_as_string(VkPhysicalDeviceType type) {
    switch (type)
    {
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        return "Other";

    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return "Integrated GPU";

    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        return "Discrete GPU";

    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        return "Virtualized GPU";

    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        return "CPU";

    default:
        return "Unknown";
    }
}

int main()
{
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "vkDevice Info",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = VK_API_VERSION_1_1,
    };

    VkInstanceCreateInfo vk_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
    };

    VkInstance vk = NULL;
    VkResult res = vkCreateInstance(&vk_info, NULL, &vk);

    if (res != VK_SUCCESS) {
        fprintf(stderr, "Unable to create VkInstance!\n");
        return 0;
    }

    /* Check to see how many Vulkan compatible devices there are. */
    uint32_t count = 0;
    res = vkEnumeratePhysicalDevices(vk, &count, NULL);
    if (res != VK_SUCCESS) {
        fprintf(stderr, "Unable to enumerate physical devices\n");
        goto cleanup_instance;
    }

    VkPhysicalDevice* physical_devices = malloc(count * sizeof(VkPhysicalDevice));
    if (physical_devices == NULL) {
        fprintf(stderr, "Unable to enumerate physical devices\n");
        goto cleanup_instance;
    }

    res = vkEnumeratePhysicalDevices(vk, &count, physical_devices);
    if (res != VK_SUCCESS) {
        fprintf(stderr, "Unable to enumerate physical devices\n");
        goto cleanup_devices;
    }

    // Now, a VkPhysicalDevice doesn't actually tell us anything but is just a way to
    // reference a particular device. To find out its details we need to make additional
    // calls...
    for (uint32_t i = 0; i < count; i++) {
        VkPhysicalDeviceProperties properties = {};
        vkGetPhysicalDeviceProperties(physical_devices[i], &properties);

        uint32_t vk_major = VK_VERSION_MAJOR(properties.apiVersion);
        uint32_t vk_minor = VK_VERSION_MINOR(properties.apiVersion);
        uint32_t vk_patch = VK_VERSION_PATCH(properties.apiVersion);

        uint32_t driver_major = VK_VERSION_MAJOR(properties.driverVersion);
        uint32_t driver_minor = VK_VERSION_MINOR(properties.driverVersion);
        uint32_t driver_patch = VK_VERSION_PATCH(properties.driverVersion);

        printf("Device Name:     \t%s\n", properties.deviceName);
        printf("  Type:          \t%s\n", vkPhysicalDeviceType_as_string(properties.deviceType));
        printf("  Vendor ID:     \t%d\n", properties.vendorID);
        printf("  Device ID:     \t%d\n", properties.deviceID);
        printf("  API Version:   \tv%d.%d.%d\n", vk_major, vk_minor, vk_patch);
        printf("  Driver Version:\tv%d.%d.%d\n", driver_major, driver_minor, driver_patch);
    }

    cleanup_devices:
        free(physical_devices);
        physical_devices = NULL;

    cleanup_instance:
        vkDestroyInstance(vk, NULL);

    return 0;
}
