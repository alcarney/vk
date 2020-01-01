#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>

void getVkPhysicalDevices(VkInstance vk, VkPhysicalDevice** physical_devices, uint32_t* count)
{
    // First check to see how many Vulkan compatiable devices are available
    VkResult res = vkEnumeratePhysicalDevices(vk, count, NULL);
    if (res != VK_SUCCESS) {
        fprintf(stderr, "Unable to enumerate physical devices\n");
        return;
    }

    // printf("Found %d devices\n", *count);
    *physical_devices = (VkPhysicalDevice*) malloc(*count * sizeof(VkPhysicalDevice));
    if (*physical_devices == NULL) {
        fprintf(stderr, "Unable to enumerate physical devices\n");
        return;
    }

    res = vkEnumeratePhysicalDevices(vk, count, *physical_devices);
    if (res != VK_SUCCESS) {
        fprintf(stderr, "Unable to enumerate physical devices\n");
        free(physical_devices);
        physical_devices = NULL;
    }
}

void getVkPhysicalDeviceProperties(VkPhysicalDeviceProperties** device_properties, VkPhysicalDevice* physical_devices, uint32_t count)
{
    *device_properties = (VkPhysicalDeviceProperties*) malloc(count * sizeof(VkPhysicalDeviceProperties));
    if (*device_properties == NULL) {
        fprintf(stderr, "Unable to get physical device properties\n");
        return;
    }

    for (uint32_t i = 0; i < count; i++) {
        vkGetPhysicalDeviceProperties(physical_devices[i], device_properties[i]);
    }
}

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
        .applicationVersion = 0x010000,
        .pEngineName = "vkEngine",
        .engineVersion = 0x010000,
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
    VkPhysicalDevice* physical_devices = NULL;
    getVkPhysicalDevices(vk, &physical_devices, &count);

    if (physical_devices == NULL) {
        goto cleanup;
    }

    // Now, a VkPhysicalDevice doesn't actually tell us anything but is just a way to
    // reference a particular device. To find out its details we need to make additional
    // calls...
    VkPhysicalDeviceProperties* device_properties = NULL;
    getVkPhysicalDeviceProperties(&device_properties, physical_devices, count);

    if (device_properties == NULL) {
        free(physical_devices);
        goto cleanup;
    }

    for (uint32_t i = 0; i < count; i++) {

        uint32_t vk_major = VK_VERSION_MAJOR(device_properties[i].apiVersion);
        uint32_t vk_minor = VK_VERSION_MINOR(device_properties[i].apiVersion);
        uint32_t vk_patch = VK_VERSION_PATCH(device_properties[i].apiVersion);

        uint32_t driver_major = VK_VERSION_MAJOR(device_properties[i].driverVersion);
        uint32_t driver_minor = VK_VERSION_MINOR(device_properties[i].driverVersion);
        uint32_t driver_patch = VK_VERSION_PATCH(device_properties[i].driverVersion);

        printf("Device Name:     \t%s\n", device_properties[i].deviceName);
        printf("  Type:          \t%s\n", vkPhysicalDeviceType_as_string(device_properties[i].deviceType));
        printf("  Vendor ID:     \t%d\n", device_properties[i].vendorID);
        printf("  Device ID:     \t%d\n", device_properties[i].deviceID);
        printf("  API Version:   \tv%d.%d.%d\n", vk_major, vk_minor, vk_patch);
        printf("  Driver Version:\tv%d.%d.%d\n", driver_major, driver_minor, driver_patch);
    }

    free(physical_devices);
    free(device_properties);

    cleanup:
        vkDestroyInstance(vk, NULL);

    return 0;
}
