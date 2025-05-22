#include <stdlib.h>
#include <stdio.h>

int main() {
    // 📸 Output image file
    const char *output = "photo.jpg";

    // 🔧 SETTINGS — change these:
    float crop_percent = 0.5f;        // ✂️ Crop size as a fraction of square area (e.g., 0.5 = 50%)
    int out_w = 512, out_h = 512;     // 📉 Final output resolution (width x height)

    // 📐 Sensor resolution (Camera Module 3 = 4608x2592)
    // Square crop based on height (2592)
    float roi_size = crop_percent;
    float roi_x = (1 - roi_size * 2592.0f / 4608.0f) / 2.0f;  // X offset to center crop
    float roi_y = (1 - roi_size) / 2.0f;                      // Y offset to center crop
    float roi_w = roi_size * 2592.0f / 4608.0f;               // Width as fraction of sensor
    float roi_h = roi_size;                                   // Height as fraction of sensor

    // 🧾 Build the libcamera-still command
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
        "libcamera-still "
        "--roi %.4f,%.4f,%.4f,%.4f "  // Centered square crop (x,y,width,height)
        "--width %d --height %d "     // Resize output
        "-o %s -t 1000 -n",           // Output file, 1s delay, no preview
        roi_x, roi_y, roi_w, roi_h,
        out_w, out_h,
        output);

    // ▶️ Run the command
    return system(cmd);
}
