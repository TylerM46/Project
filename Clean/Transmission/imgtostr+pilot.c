#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // Required for strlen, strcpy, strcat

// Function to append only a prefix to a string
char *append_prefix(const char *original, const char *prefix) {
    size_t original_len = strlen(original);
    size_t prefix_len = strlen(prefix);

    char *result = (char *)malloc(prefix_len + original_len + 1); // +1 for null terminator
    if (result == NULL) {
        printf("Memory allocation failed for prefix append.\n");
        return NULL;
    }

    strcpy(result, prefix);
    strcat(result, original);

    return result;
}

// Function to convert image to binary string with reduced color depth
char *convert_to_binary(const char *filename) {
    int width, height, channels;
    unsigned char *img = stbi_load(filename, &width, &height, &channels, 0); // Load image in color

    if (img == NULL) {
        printf("Error loading image\n");
        return NULL;
    }

    // Allocate memory for binary string
    char *binary_string = (char *)malloc(width * height * channels * 3 + 1); // 3 bits per channel
    if (binary_string == NULL) {
        printf("Memory allocation failed\n");
        stbi_image_free(img);
        return NULL;
    }

    // Convert image to binary string with reduced color depth
    int index = 0;
    for (int i = 0; i < width * height * channels; i++) {
        unsigned char reduced_color = img[i] / 32; // Reduce color depth to 3 bits per channel
        for (int bit = 2; bit >= 0; bit--) {
            binary_string[index++] = (reduced_color & (1 << bit)) ? '1' : '0';
        }
    }
    binary_string[index] = '\0'; // Null-terminate the string

    stbi_image_free(img);
    return binary_string;
}

int main() {
    const char *filename = "25x25.jpg";
    const char *prefix = "10101010101010101010";

    // Generate binary string
    char *binary_string = convert_to_binary(filename);
    if (binary_string == NULL) {
        return 1; // Error already printed
    }

    // Append prefix only
    char *final_string = append_prefix(binary_string, prefix);
    if (final_string == NULL) {
        free(binary_string);
        return 1;
    }

    // Print final binary string
    printf("Binary string with prefix:\n%s\n", final_string);

    // Free memory
    free(binary_string);
    free(final_string);

    return 0;
}
