#include <stdio.h>
#include <pigpio.h>

#define GPIO_PIN 5  // Change to your desired GPIO pin number
#define ARRAY_SIZE 8 // Size of the integer array

int main(void) {
    if (gpioInitialise() < 0) {
        printf("pigpio initialization failed\n");
        return 1;
    }

    gpioSetMode(GPIO_PIN, PI_OUTPUT);

    // Example integer array of 1s and 0s
    int sequence[ARRAY_SIZE] = {1, 0, 1, 1, 0, 0, 1, 0};

    gpioPulse_t pulses[ARRAY_SIZE * 2];
    int pulseIndex = 0;

    for (int i = 0; i < ARRAY_SIZE; i++) {
        pulses[pulseIndex].gpioOn = sequence[i] ? (1 << GPIO_PIN) : 0;
        pulses[pulseIndex].gpioOff = sequence[i] ? 0 : (1 << GPIO_PIN);
        pulses[pulseIndex].usDelay = 1;  // 1 microsecond delay for 500kHz
        pulseIndex++;
    }

    gpioWaveClear();
    gpioWaveAddGeneric(ARRAY_SIZE, pulses);
    int wave_id = gpioWaveCreate();

    if (wave_id >= 0) {
        gpioWaveTxSend(wave_id, PI_WAVE_MODE_REPEAT);
    } else {
        printf("Wave creation failed\n");
    }

    // Keep the program running to maintain the waveform
    while (1) {
        time_sleep(1);
    }

    gpioTerminate();
    return 0;
}
