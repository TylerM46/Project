#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // Required for strlen, strcpy, strcat

// Inserts "10" between every 10 bits in the binary string
char *insert_separators_every_10_bits(const char *binary_str) {
    size_t len = strlen(binary_str);
    size_t num_groups = len / 10;

    // Each "10" adds 2 chars between groups (except after last)
    size_t new_len = len + (num_groups > 0 ? (num_groups - 1) * 2 : 0) + 1;

    char *result = (char *)malloc(new_len);
    if (!result) {
        printf("Memory allocation failed in insert_separators_every_10_bits.\n");
        return NULL;
    }

    size_t src_index = 0, dst_index = 0;

    for (size_t group = 0; group < num_groups; group++) {
        for (int i = 0; i < 10; i++) {
            result[dst_index++] = binary_str[src_index++];
        }
        if (group < num_groups - 1) {
            result[dst_index++] = '1';
            result[dst_index++] = '0';
        }
    }

    // Copy remaining bits if any
    while (src_index < len) {
        result[dst_index++] = binary_str[src_index++];
    }
    result[dst_index] = '\0';

    return result;
}

// Function to append only a prefix to a string
char *append_prefix(const char *original, const char *prefix) {
    size_t original_len = strlen(original);
    size_t prefix_len = strlen(prefix);

    char *result = (char *)malloc(prefix_len + original_len + 1);
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
    unsigned char *img = stbi_load(filename, &width, &height, &channels, 0);
    if (img == NULL) {
        printf("Error loading image\n");
        return NULL;
    }

    char *binary_string = (char *)malloc(width * height * channels * 3 + 1);
    if (binary_string == NULL) {
        printf("Memory allocation failed\n");
        stbi_image_free(img);
        return NULL;
    }

    int index = 0;
    for (int i = 0; i < width * height * channels; i++) {
        unsigned char reduced_color = img[i] / 32;
        for (int bit = 2; bit >= 0; bit--) {
            binary_string[index++] = (reduced_color & (1 << bit)) ? '1' : '0';
        }
    }
    binary_string[index] = '\0';

    stbi_image_free(img);
    return binary_string;
}

int main() {
    const char *filename = "25x25.jpg";
    const char *prefix = "110010101010101010101100";

    char *binary_string = convert_to_binary(filename);
    if (binary_string == NULL) {
        return 1;
    }

    //NOT NEEDED, Flip instead!
    /* Insert "10" separators every 10 bits
    char *with_separators = insert_separators_every_10_bits(binary_string);
    free(binary_string);
    if (with_separators == NULL) {
        return 1;
    }*/

    // Append prefix only
    char *final_string = append_prefix(binary_string, prefix);
    //free(with_separators);
    if (final_string == NULL) {
        return 1;
    }

    // Print final binary string
    printf("Binary string with prefix:\n%s\n", final_string);
    printf("Length of final binary string: %zu\n", strlen(final_string));

    free(final_string);
    return 0;
}
//100001001011001001011001001100001001101010010