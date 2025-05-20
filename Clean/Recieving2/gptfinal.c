import pigpio
import time
import numpy as np
from PIL import Image

pi = pigpio.pi()
if not pi.connected:
    exit()

# GPIO setup
GPIO_PIN = 26  # Replace with your GPIO pin number

timearray = []
dataarray = []
data = []

# Set the GPIO pin as an input
pi.set_mode(GPIO_PIN, pigpio.INPUT)

# Callback function to record edges with timestamp and level
def read_gpio(gpio, level, tick):
    timearray.append(tick)
    dataarray.append(level)

# Start callback to monitor GPIO edges
cb = pi.callback(GPIO_PIN, pigpio.EITHER_EDGE, read_gpio)

# Data rate / timing parameters
bitlength = 50  # in microseconds (μs), corresponds to 20kHz data rate (adjustable)

# Capture data for specified duration (seconds)
duration = 0.5
time.sleep(duration)

# Stop capturing data
cb.cancel()
pi.stop()

# Phase-Locked Loop (PLL) based decoder
def pll_decode(timearray, dataarray, bitlength):
    """
    Decode data stream using PLL to track timing errors.
    Args:
        timearray: list of edge timestamps (μs)
        dataarray: list of corresponding levels (0 or 1)
        bitlength: nominal bit duration in μs
    Returns:
        data_bits: list of decoded bits (0 or 1)
    """
    data_bits = []
    phase = 0.0
    remainder = 0.0
    Kp = 0.1  # Proportional gain - adjust for PLL responsiveness
    Ki = 0.01 # Integral gain - adjust for steady-state error correction
    integral = 0.0
    nominal_bitlength = float(bitlength)

    for i in range(len(timearray) - 1):
        # Time interval between edges
        interval = (timearray[i+1] - timearray[i]) + remainder
        # Number of nominal bits covered by this interval (float)
        bits_covered = interval / nominal_bitlength
        
        # Split into integer number of bits and fractional remainder
        int_bits = int(bits_covered)
        remainder = interval - int_bits * nominal_bitlength

        # Append bits with current level (invert if needed to match original logic)
        for _ in range(int_bits):
            data_bits.append(abs(dataarray[i] - 1))

        # PLL error is difference between measured interval and ideal bitlength * int_bits
        # Use error per bit to adjust phase and integral for next iteration
        if int_bits > 0:
            measured_bit_length = interval / int_bits
            error = measured_bit_length - nominal_bitlength

            # Update integral and phase for next iteration
            integral += error
            phase_correction = Kp * error + Ki * integral

            # Adjust remainder to simulate phase correction
            remainder -= phase_correction

    return data_bits

# Decode the data using PLL
data = pll_decode(timearray, dataarray, bitlength)

# Join bits into a binary string
readdata = ''.join(map(str, data))
print(f"Decoded data length: {len(readdata)} bits")

# Improved extractdata function, matching your style and clear logic
def extractdata(binary_string, pattern):
    """
    Extracts binary segments found between repeating pattern headers.
    Args:
        binary_string: full binary string from received data
        pattern: known header pattern to locate start of data blocks
    Returns:
        List of binary segments found between header patterns
    """
    results = []
    positions = []
    index = 0

    # Find all occurrences of the pattern
    while index <= len(binary_string) - len(pattern):
        if binary_string[index:index + len(pattern)] == pattern:
            positions.append(index)
            index += len(pattern)  # Skip past this pattern to avoid overlapping
        else:
            index += 1

    # Extract segments between successive pattern matches
    for i in range(len(positions) - 1):
        start = positions[i] + len(pattern)
        end = positions[i + 1]
        results.append(binary_string[start:end])

    return results

# Function to flip every 5th bit to correct error (as per your existing code)
def flip_every_5th_bit(binary_str):
    bit_list = list(binary_str)
    for i in range(4, len(bit_list), 5):
        bit_list[i] = '0' if bit_list[i] == '1' else '1'
    return ''.join(bit_list)

# Image reconstruction from binary string
def binary_to_image(binary_string, width, height, channels):
    byte_array = bytearray()
    # Process every 3 bits as a small color value segment (as per your method)
    for i in range(0, len(binary_string), 3):
        byte = binary_string[i:i+3]
        if len(byte) < 3:
            break  # avoid partial bytes at the end
        byte_array.append(int(byte, 2) * 32)  # scale to 0-255 range

    img_array = np.array(byte_array, dtype=np.uint8).reshape((height, width, channels))
    img = Image.fromarray(img_array, mode='RGB')
    img.save('./flip.jpg')
    print("Image saved as flip.jpg")

# --- Main usage ---

# Known header pattern to locate data start (adjust to your actual pattern)
pattern = "110010101010101010101100"

# Extract data segments using header pattern
segments = extractdata(readdata, pattern)

if not segments:
    raise ValueError("No data segments found — check signal or pattern.")

# Use the first extracted data segment
segment = segments[0]

# Correct errors by flipping every 5th bit
corrected_segment = flip_every_5th_bit(segment)

# Define your image dimensions and channels (adjust accordingly)
width = 25
height = 25
channels = 3

# Validate bit length (each pixel channel is 3 bits)
expected_length = width * height * channels * 3
if len(corrected_segment) != expected_length:
    raise ValueError(f"Incorrect bit length. Got {len(corrected_segment)} bits, expected {expected_length}.")

# Reconstruct and save the image
binary_to_image(corrected_segment, width, height, channels)

print(f"Processed data length: {len(corrected_segment)} bits")
print("Done.")
