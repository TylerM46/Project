#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>

void convert_jpg_to_binary(const char *filename, unsigned char **binary_array, int *size) {
    int width, height, channels;
    unsigned char *img = stbi_load(filename, &width, &height, &channels, 0);
    if (img == NULL) {
        fprintf(stderr, "Error loading image %s\n", filename);
        exit(1);
    }

    *size = width * height * channels;
    *binary_array = (unsigned char *)malloc(*size);

    for (int i = 0; i < *size; i++) {
        (*binary_array)[i] = img[i] > 127 ? 1 : 0;
    }

    stbi_image_free(img);
}

int main() {
    const char *filename = "minisquare3.jpg";
    unsigned char *binary_array;
    int size;

    convert_jpg_to_binary(filename, &binary_array, &size);

    // Print the binary array
    for (int i = 0; i < size; i++) {
        printf("%d", binary_array[i]);
        if ((i + 1) % 8 == 0) printf(" ");
    }

    free(binary_array);
    return 0;
}
