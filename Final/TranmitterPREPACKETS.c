#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pigpio.h>
#include <unistd.h>  // for sleep()

#define GPIO_pin_5 5
#define GPIO_pin_6 6
#define MAX_WAVE_BITS 12000

// Function to flip every 5th bit
char *flip_every_5th_bit(const char *binary_str) {
    size_t len = strlen(binary_str);
    char *flipped = (char *)malloc(len + 1);
    if (!flipped) {
        printf("Memory allocation failed in flip_every_5th_bit.\n");
        return NULL;
    }

    for (size_t i = 0; i < len; ++i) {
        if ((i + 1) % 5 == 0) {
            flipped[i] = (binary_str[i] == '0') ? '1' : '0';
        } else {
            flipped[i] = binary_str[i];
        }
    }
    flipped[len] = '\0';
    return flipped;
}

// Function to append a prefix
char *append_prefix(const char *original, const char *prefix) {
    size_t original_len = strlen(original);
    size_t prefix_len = strlen(prefix);

    char *result = (char *)malloc(prefix_len + original_len + 1);
    if (!result) {
        printf("Memory allocation failed for prefix append.\n");
        return NULL;
    }

    strcpy(result, prefix);
    strcat(result, original);
    return result;
}

// Convert image to binary string with reduced color depth
char *convert_to_binary(const char *filename) {
    int width, height, channels;
    unsigned char *img = stbi_load(filename, &width, &height, &channels, 3);  // Force RGB
    if (!img) {
        printf("Error loading image\n");
        return NULL;
    }

    char *binary_string = (char *)malloc(width * height * 3 * 3 + 1);  // 3 bits per R/G/B
    if (!binary_string) {
        printf("Memory allocation failed\n");
        stbi_image_free(img);
        return NULL;
    }

    int index = 0;
    for (int i = 0; i < width * height * 3; i++) {
        unsigned char reduced = img[i] / 32;
        for (int bit = 2; bit >= 0; bit--) {
            binary_string[index++] = (reduced & (1 << bit)) ? '1' : '0';
        }
    }

    binary_string[index] = '\0';
    stbi_image_free(img);
    return binary_string;
}

int main() {
    const char *filename = "25x25.jpg";
    const char *prefix = "110010101010101010101100";

    // Step 1: Generate final binary string
    char *binary_string = convert_to_binary(filename);
    if (!binary_string) return 1;

    char *flipped_binary = flip_every_5th_bit(binary_string);
    free(binary_string);
    if (!flipped_binary) return 1;

    char *final_string = append_prefix(flipped_binary, prefix);
    free(flipped_binary);
    if (!final_string) return 1;

    size_t bit_count = strlen(final_string);
    printf("Final binary string length: %zu\n", bit_count);

    // Waveform size check for Pi 3A+
    if (bit_count > MAX_WAVE_BITS) {
        printf("Error: Binary string too long for pigpio waveform (limit ~%d bits).\n", MAX_WAVE_BITS);
        free(final_string);
        return 1;
    }

    // Step 2: Initialize pigpio
    if (gpioInitialise() < 0) {
        printf("pigpio initialization failed\n");
        free(final_string);
        return 1;
    }

    gpioSetMode(GPIO_pin_5, PI_OUTPUT);
    gpioSetMode(GPIO_pin_6, PI_OUTPUT);

    // Step 3: Build waveform
    const int bit_time_us = 50;
    gpioPulse_t *pulses = malloc(sizeof(gpioPulse_t) * bit_count);
    if (!pulses) {
        printf("Memory allocation for pulses failed\n");
        gpioTerminate();
        free(final_string);
        return 1;
    }

    for (size_t i = 0; i < bit_count; i++) {
        int gpio_mask = 0;
        if (final_string[i] == '1') {
            gpio_mask = (1 << GPIO_pin_5) | (1 << GPIO_pin_6);
        }

        pulses[i].gpioOn  = gpio_mask;
        pulses[i].gpioOff = ((1 << GPIO_pin_5) | (1 << GPIO_pin_6)) & (~gpio_mask);
        pulses[i].usDelay = bit_time_us;
    }

    gpioWaveClear();
    gpioWaveAddGeneric(bit_count, pulses);
    int wave_id = gpioWaveCreate();

    if (wave_id >= 0) {
        printf("Transmitting waveform in repeat mode...\n");
        gpioWaveTxSend(wave_id, PI_WAVE_MODE_REPEAT);

        // Run until manually terminated (Ctrl+C)
        while (1) {
            time_sleep(1);  // Reduce CPU load
        }
    } else {
        printf("Failed to create waveform\n");
    }

    // Cleanup (only reached if loop is broken)
    free(pulses);
    free(final_string);
    gpioTerminate();

    return 0;
}
