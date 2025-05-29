#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pigpio.h>

#define GPIO_pin_5 5
#define GPIO_pin_6 6

// Image constants (must match receiver)
#define WIDTH 25
#define HEIGHT 25
#define CHANNELS 3
#define BITS_PER_CHANNEL 3
#define BITS_PER_PIXEL (CHANNELS * BITS_PER_CHANNEL)  // 9 bits per pixel
#define PIXELS_PER_PACKET 50
#define PACKET_DATA_BITS (PIXELS_PER_PACKET * BITS_PER_PIXEL)  // 450 bits

// Sync patterns
const char *PILOT_PATTERN = "110010101010101010101100";  // once at start
const char *PACKET_HEADER = "1010101100";                // before each packet

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

// Convert image to binary string with 3 bits per channel, 3 channels assumed
char *convert_to_binary(const char *filename) {
    int width, height, channels;
    unsigned char *img = stbi_load(filename, &width, &height, &channels, 3);  // force 3 channels
    if (img == NULL) {
        printf("Error loading image\n");
        return NULL;
    }

    // Check image dimensions and channels
    if (width != WIDTH || height != HEIGHT || channels != CHANNELS) {
        printf("Image must be %dx%d with %d channels (converted to RGB)\n", WIDTH, HEIGHT, CHANNELS);
        stbi_image_free(img);
        return NULL;
    }

    size_t total_pixels = (size_t)width * height;
    size_t bit_len = total_pixels * CHANNELS * BITS_PER_CHANNEL;  // 25*25*3*3 = 5625 bits

    char *binary_string = (char *)malloc(bit_len + 1);
    if (!binary_string) {
        printf("Memory allocation failed\n");
        stbi_image_free(img);
        return NULL;
    }

    size_t index = 0;
    for (size_t i = 0; i < total_pixels * CHANNELS; i++) {
        unsigned char reduced_color = img[i] / 32; // Reduce 0-255 to 0-7 (3 bits)
        for (int bit = 2; bit >= 0; bit--) {
            binary_string[index++] = (reduced_color & (1 << bit)) ? '1' : '0';
        }
    }
    binary_string[index] = '\0';

    stbi_image_free(img);
    return binary_string;
}

// Function to build final bitstream: pilot + repeated (packet_header + packet_data)
char *build_final_bitstream(const char *flipped_bits) {
    size_t total_bits = strlen(flipped_bits);
    size_t packets_count = (total_bits + PACKET_DATA_BITS - 1) / PACKET_DATA_BITS;

    size_t pilot_len = strlen(PILOT_PATTERN);
    size_t header_len = strlen(PACKET_HEADER);

    // Calculate total length:
    // pilot + (packet_header + packet_data)*packets_count
    size_t final_len = pilot_len + packets_count * (header_len + PACKET_DATA_BITS);

    char *final_stream = (char *)malloc(final_len + 1);
    if (!final_stream) {
        printf("Memory allocation failed for final stream\n");
        return NULL;
    }

    size_t pos = 0;
    // Copy pilot pattern
    memcpy(final_stream + pos, PILOT_PATTERN, pilot_len);
    pos += pilot_len;

    // For each packet:
    for (size_t p = 0; p < packets_count; p++) {
        // Copy packet header
        memcpy(final_stream + pos, PACKET_HEADER, header_len);
        pos += header_len;

        // Copy packet data bits (handle last partial packet)
        size_t data_start = p * PACKET_DATA_BITS;
        size_t remaining_bits = total_bits - data_start;
        size_t copy_len = remaining_bits < PACKET_DATA_BITS ? remaining_bits : PACKET_DATA_BITS;

        memcpy(final_stream + pos, flipped_bits + data_start, copy_len);
        pos += copy_len;
    }

    final_stream[pos] = '\0';
    return final_stream;
}

int main() {
    const char *filename = "25x25.jpg";

    // Step 1: Convert image to binary
    char *binary_string = convert_to_binary(filename);
    if (!binary_string) return 1;

    // Step 2: Flip every 5th bit (bit error mitigation)
    char *flipped_binary = flip_every_5th_bit(binary_string);
    free(binary_string);
    if (!flipped_binary) return 1;

    // Step 3: Build final bitstream with pilot and headers
    char *final_stream = build_final_bitstream(flipped_binary);
    free(flipped_binary);
    if (!final_stream) return 1;

    size_t final_len = strlen(final_stream);
    printf("Final bitstream length: %zu bits\n", final_len);

    // Step 4: Initialize pigpio
    if (gpioInitialise() < 0) {
        printf("pigpio initialization failed\n");
        free(final_stream);
        return 1;
    }

    gpioSetMode(GPIO_pin_5, PI_OUTPUT);
    gpioSetMode(GPIO_pin_6, PI_OUTPUT);

    // Step 5: Check max waveform size and build pulses
    const int bit_time_us = 50;
    const unsigned max_waveform_size = 12000;  // safe margin below 12000 pulses

    if (final_len > max_waveform_size) {
        printf("Error: final bitstream too large for a single waveform (max %u bits).\n", max_waveform_size);
        free(final_stream);
        gpioTerminate();
        return 1;
    }

    gpioPulse_t *pulses = malloc(sizeof(gpioPulse_t) * final_len);
    if (!pulses) {
        printf("Memory allocation failed for pulses\n");
        free(final_stream);
        gpioTerminate();
        return 1;
    }

    for (size_t i = 0; i < final_len; i++) {
        int gpio_mask = 0;
        if (final_stream[i] == '1') {
            gpio_mask = (1 << GPIO_pin_5) | (1 << GPIO_pin_6);
        }
        pulses[i].gpioOn = gpio_mask;
        pulses[i].gpioOff = ((1 << GPIO_pin_5) | (1 << GPIO_pin_6)) & (~gpio_mask);
        pulses[i].usDelay = bit_time_us;
    }

    // Step 6: Transmit waveform repeatedly (loop continuously)
    gpioWaveClear();
    gpioWaveAddGeneric(final_len, pulses);
    int wave_id = gpioWaveCreate();

    if (wave_id < 0) {
        printf("Failed to create waveform\n");
        free(pulses);
        free(final_stream);
        gpioTerminate();
        return 1;
    }

    printf("Starting continuous waveform transmission...\n");
    gpioWaveTxSend(wave_id, PI_WAVE_MODE_REPEAT_SYNC);  // loop continuously, sync mode

    // Keep program running while waveform transmits
    while (gpioWaveTxBusy()) {
        time_sleep(0.01);
    }

    // Wait indefinitely to keep waveform alive
    while (1) {
        time_sleep(1);
    }

    // Cleanup (never reached in current design)
    gpioWaveTxStop();
    gpioWaveDelete(wave_id);
    free(pulses);
    free(final_stream);
    gpioTerminate();

    return 0;
}
