#include <stdio.h>

int main() {
    FILE *file;
    unsigned char buffer[1024];
    size_t bytesRead;

    // Open the JPEG file
    file = fopen("64x64.jpg", "rb");
    if (file == NULL) {
        perror("Failed to open file");
        return 1;
    }

    // Transmit the buffer content (e.g., save to another file or send over a network)
    // For demonstration, we'll save it to a binary file
    FILE *output_file = fopen("binary_output.bin", "wb");

    // Read the file content into the buffer and write to the output file
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        fwrite(buffer, 1, bytesRead, output_file);
    }

    // Clean up
    fclose(output_file);
    fclose(file);

    printf("JPEG to binary conversion successful.\n");

    return 0;
}
