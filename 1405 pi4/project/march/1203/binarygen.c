#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>

// Function to convert image to binary string with reduced color depth
void convert_to_binary(const char *filename) {
    int width, height, channels;
    unsigned char *img = stbi_load(filename, &width, &height, &channels, 0); // Load image in color

    if (img == NULL) {
        printf("Error loading image\n");
        return;
    }

    // Allocate memory for binary string
    char *binary_string = (char *)malloc(width * height * channels * 3 + 1); // 3 bits per channel
    if (binary_string == NULL) {
        printf("Memory allocation failed\n");
        stbi_image_free(img);
        return;
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

    // Print binary string
    printf("Binary string:\n%s\n", binary_string);

    // Free allocated memory
    free(binary_string);
    stbi_image_free(img);
}

int main() {
    const char *filename = "50x50.jpg";
    convert_to_binary(filename);
    return 0;
}
