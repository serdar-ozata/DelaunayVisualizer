//
// Created by serdar on 5/15/25.
//

#ifndef GPUSTATS_H
#define GPUSTATS_H
#ifdef ENABLE_GPUSTATS
#include <nvml.h>
#include <string>
#include <iostream>

struct GpuStats {
    std::string name = "Unknown";
    unsigned long long memoryUsedMB = 0;
    unsigned long long memoryTotalMB = 0;
    unsigned int gpuUtilPercent = 0;
    unsigned int memUtilPercent = 0;
    unsigned int temperatureC = 0;
};

class NVMLWrapper {
public:
    NVMLWrapper(int gpuIndex = 0) {
        nvmlReturn_t result = nvmlInit();
        if (result != NVML_SUCCESS) {
            std::cerr << "[NVML] Failed to initialize NVML: " << nvmlErrorString(result) << std::endl;
            return;
        }

        result = nvmlDeviceGetHandleByIndex(gpuIndex, &device);
        if (result != NVML_SUCCESS) {
            std::cerr << "[NVML] Failed to get handle for GPU " << gpuIndex << ": " << nvmlErrorString(result) << std::endl;
            nvmlShutdown();
            return;
        }

        initialized = true;
    }

    ~NVMLWrapper() {
        if (initialized) {
            nvmlShutdown();
        }
    }

    bool isInitialized() const {
        return initialized;
    }

    bool queryGpuStats(GpuStats& stats) const {
        if (!initialized) return false;

        char name[NVML_DEVICE_NAME_BUFFER_SIZE];
        if (nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE) != NVML_SUCCESS)
            return false;
        stats.name = name;

        nvmlMemory_v2_t memInfo;
        if (nvmlDeviceGetMemoryInfo_v2(device, &memInfo) != NVML_SUCCESS)
            return false;
        stats.memoryUsedMB = memInfo.used / (1024 * 1024);
        stats.memoryTotalMB = memInfo.total / (1024 * 1024);

        nvmlUtilization_t utilization;
        if (nvmlDeviceGetUtilizationRates(device, &utilization) != NVML_SUCCESS)
            return false;
        stats.gpuUtilPercent = utilization.gpu;
        stats.memUtilPercent = utilization.memory;

        unsigned int temp;
        if (nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp) == NVML_SUCCESS)
            stats.temperatureC = temp;

        return true;
    }

private:
    bool initialized = false;
    nvmlDevice_t device = nullptr;
};
#endif // DISABLE_GPUSTATS
#endif // GPUSTATS_H