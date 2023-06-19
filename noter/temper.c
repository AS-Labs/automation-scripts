#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH 256

float readTemperature(const char* device) {
    FILE* file;
    char path[MAX_PATH];
    float temperature = 0.0f;

    snprintf(path, sizeof(path), "/sys/class/thermal/%s/temp", device);

    file = fopen(path, "r");
    if (file != NULL) {
        int temp;
        if (fscanf(file, "%d", &temp) == 1) {
            temperature = temp / 1000.0f;
        }
        fclose(file);
    }

    return temperature;
}

int main() {
    float cpuTemperature = readTemperature("thermal_zone0");
    float gpuTemperature = readTemperature("thermal_zone1");

    printf("CPU Temperature: %.2f°C\n", cpuTemperature);
    printf("GPU Temperature: %.2f°C\n", gpuTemperature);

    return 0;
}

