#include <stdio.h>
#include <stdlib.h>

void convertToBinaryString(const char *filename, const char *outputFilename) {
    FILE *file = fopen(filename, "rb");
    FILE *outputFile = fopen(outputFilename, "w");
    if (!file || !outputFile) {
        perror("File opening failed");
        return;
    }

    unsigned char buffer;
    while (fread(&buffer, sizeof(unsigned char), 1, file)) {
        for (int i = 7; i >= 0; i--) {
            fprintf(outputFile, "%d", (buffer >> i) & 1);
        }
    }

    fclose(file);
    fclose(outputFile);
}

int main() {
    const char *inputFilename = "minisquare3.jpg";
    const char *outputFilename = "output.txt";
    convertToBinaryString(inputFilename, outputFilename);
    return 0;
}
