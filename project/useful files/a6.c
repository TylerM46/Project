#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>

// Function to convert image to binary string
void convert_to_binary(const char *filename) {
    int width, height, channels;
    unsigned char *img = stbi_load(filename, &width, &height, &channels, 0); // Load image in color

    if (img == NULL) {
        printf("Error loading image\n");
        return;
    }

    // Allocate memory for binary string
    char *binary_string = (char *)malloc(width * height * channels * 8 + 1);
    if (binary_string == NULL) {
        printf("Memory allocation failed\n");
        stbi_image_free(img);
        return;
    }

    // Convert image to binary string
    int index = 0;
    for (int i = 0; i < width * height * channels; i++) {
        for (int bit = 7; bit >= 0; bit--) {
            binary_string[index++] = (img[i] & (1 << bit)) ? '1' : '0';
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
    const char *filename = "minisquare3.jpg";
    convert_to_binary(filename);
    return 0;
}
